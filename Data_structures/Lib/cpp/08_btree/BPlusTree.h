// BPlusTree.h - B+ 树（有序映射 K -> V），数据库索引最常用的结构。
//
// 与 B 树的区别：
//   - 所有值只存放在叶子结点；内部结点只存"路标"键，用来引导查找
//   - 叶子结点用 next 指针串成有序链表，范围查询只需找到起点然后顺着链表走
//
// 阶数 Order = 每个结点最多的孩子数，最多 Order-1 个键；
// 非根结点至少 (Order-1)/2 个键。
// 不变式：内部结点 keys[i-1] <= children[i] 中的所有键 < keys[i]。
#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

template <typename K, typename V, int Order = 4>
class BPlusTree {
    static_assert(Order >= 3, "B+ tree order must be at least 3");
    static constexpr std::size_t kMaxKeys = Order - 1;
    static constexpr std::size_t kMinKeys = kMaxKeys / 2;

    struct Node {
        bool leaf;
        std::vector<K> keys;
        std::vector<V> values;        // 仅叶子使用
        std::vector<Node*> children;  // 仅内部结点使用
        Node* next = nullptr;         // 仅叶子使用
        explicit Node(bool is_leaf) : leaf(is_leaf) {}
    };

public:
    BPlusTree() = default;
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree& operator=(const BPlusTree&) = delete;
    ~BPlusTree() { destroy(root_); }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    std::optional<V> find(const K& key) const
    {
        if (!root_)
            return std::nullopt;
        const Node* leaf = find_leaf(key);
        auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        if (it == leaf->keys.end() || key < *it)
            return std::nullopt;
        return leaf->values[it - leaf->keys.begin()];
    }

    // 插入新键返回 true；已存在则更新值并返回 false
    bool insert(const K& key, const V& value)
    {
        if (!root_)
            root_ = new Node(true);
        bool inserted = false;
        auto split = insert(root_, key, value, inserted);
        if (split) { // 根分裂：树长高一层
            Node* r = new Node(false);
            r->keys.push_back(split->first);
            r->children = {root_, split->second};
            root_ = r;
        }
        if (inserted)
            ++size_;
        return inserted;
    }

    bool erase(const K& key)
    {
        if (!root_ || !erase(root_, key))
            return false;
        --size_;
        if (root_->keys.empty()) {
            Node* old = root_;
            root_ = root_->leaf ? nullptr : root_->children[0];
            old->children.clear();
            delete old;
        }
        return true;
    }

    // 闭区间 [lo, hi] 内的所有键值对，按键升序
    std::vector<std::pair<K, V>> range(const K& lo, const K& hi) const
    {
        std::vector<std::pair<K, V>> out;
        if (!root_ || hi < lo)
            return out;
        const Node* leaf = find_leaf(lo);
        std::size_t i = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), lo) - leaf->keys.begin();
        for (; leaf; leaf = leaf->next, i = 0) {
            for (; i < leaf->keys.size(); ++i) {
                if (hi < leaf->keys[i])
                    return out;
                out.emplace_back(leaf->keys[i], leaf->values[i]);
            }
        }
        return out;
    }

    int height() const
    {
        int h = 0;
        for (const Node* n = root_; n; n = n->leaf ? nullptr : n->children[0])
            ++h;
        return h;
    }

    // 校验结构不变式，并确认叶子链表按序覆盖全部元素
    bool is_valid() const
    {
        if (!root_)
            return size_ == 0;
        int leaf_depth = -1;
        if (!check(root_, nullptr, nullptr, 0, leaf_depth, true))
            return false;
        const Node* leaf = root_;
        while (!leaf->leaf)
            leaf = leaf->children[0];
        std::size_t count = 0;
        const K* prev = nullptr;
        for (; leaf; leaf = leaf->next) {
            for (const K& k : leaf->keys) {
                if (prev && !(*prev < k))
                    return false;
                prev = &k;
                ++count;
            }
        }
        return count == size_;
    }

private:
    const Node* find_leaf(const K& key) const
    {
        const Node* n = root_;
        while (!n->leaf)
            n = n->children[child_index(n, key)];
        return n;
    }

    static std::size_t child_index(const Node* n, const K& key)
    {
        return std::upper_bound(n->keys.begin(), n->keys.end(), key) - n->keys.begin();
    }

    // 返回分裂信息：(上提到父结点的分隔键, 新的右兄弟)
    std::optional<std::pair<K, Node*>> insert(Node* n, const K& key, const V& value, bool& inserted)
    {
        if (n->leaf) {
            auto it = std::lower_bound(n->keys.begin(), n->keys.end(), key);
            std::size_t pos = it - n->keys.begin();
            if (it != n->keys.end() && !(key < *it)) {
                n->values[pos] = value;
                return std::nullopt;
            }
            n->keys.insert(it, key);
            n->values.insert(n->values.begin() + pos, value);
            inserted = true;
            if (n->keys.size() <= kMaxKeys)
                return std::nullopt;
            // 叶子分裂：右半边的第一个键"复制"一份上去（叶子里仍保留它）
            std::size_t mid = n->keys.size() / 2;
            Node* right = new Node(true);
            right->keys.assign(n->keys.begin() + mid, n->keys.end());
            right->values.assign(n->values.begin() + mid, n->values.end());
            n->keys.resize(mid);
            n->values.resize(mid);
            right->next = n->next;
            n->next = right;
            return std::make_pair(right->keys.front(), right);
        }

        std::size_t i = child_index(n, key);
        auto split = insert(n->children[i], key, value, inserted);
        if (!split)
            return std::nullopt;
        n->keys.insert(n->keys.begin() + i, split->first);
        n->children.insert(n->children.begin() + i + 1, split->second);
        if (n->keys.size() <= kMaxKeys)
            return std::nullopt;
        // 内部结点分裂：中间键"移动"上去（不在任何一边保留）
        std::size_t mid = n->keys.size() / 2;
        Node* right = new Node(false);
        K up = n->keys[mid];
        right->keys.assign(n->keys.begin() + mid + 1, n->keys.end());
        right->children.assign(n->children.begin() + mid + 1, n->children.end());
        n->keys.resize(mid);
        n->children.resize(mid + 1);
        return std::make_pair(up, right);
    }

    bool erase(Node* n, const K& key)
    {
        if (n->leaf) {
            auto it = std::lower_bound(n->keys.begin(), n->keys.end(), key);
            if (it == n->keys.end() || key < *it)
                return false;
            n->values.erase(n->values.begin() + (it - n->keys.begin()));
            n->keys.erase(it);
            return true; // 内部结点里残留的同名路标键不影响正确性
        }
        std::size_t i = child_index(n, key);
        if (!erase(n->children[i], key))
            return false;
        if (n->children[i]->keys.size() < kMinKeys)
            fix_underflow(n, i);
        return true;
    }

    // children[i] 键数不足：先尝试向左右兄弟借一个，借不到就和兄弟合并
    void fix_underflow(Node* parent, std::size_t i)
    {
        Node* child = parent->children[i];
        Node* left = i > 0 ? parent->children[i - 1] : nullptr;
        Node* right = i + 1 < parent->children.size() ? parent->children[i + 1] : nullptr;

        if (child->leaf) {
            if (left && left->keys.size() > kMinKeys) {
                child->keys.insert(child->keys.begin(), left->keys.back());
                child->values.insert(child->values.begin(), left->values.back());
                left->keys.pop_back();
                left->values.pop_back();
                parent->keys[i - 1] = child->keys.front();
            } else if (right && right->keys.size() > kMinKeys) {
                child->keys.push_back(right->keys.front());
                child->values.push_back(right->values.front());
                right->keys.erase(right->keys.begin());
                right->values.erase(right->values.begin());
                parent->keys[i] = right->keys.front();
            } else if (left) {
                merge_leaves(parent, i - 1);
            } else {
                merge_leaves(parent, i);
            }
            return;
        }

        if (left && left->keys.size() > kMinKeys) {
            child->keys.insert(child->keys.begin(), parent->keys[i - 1]);
            child->children.insert(child->children.begin(), left->children.back());
            parent->keys[i - 1] = left->keys.back();
            left->keys.pop_back();
            left->children.pop_back();
        } else if (right && right->keys.size() > kMinKeys) {
            child->keys.push_back(parent->keys[i]);
            child->children.push_back(right->children.front());
            parent->keys[i] = right->keys.front();
            right->keys.erase(right->keys.begin());
            right->children.erase(right->children.begin());
        } else if (left) {
            merge_internal(parent, i - 1);
        } else {
            merge_internal(parent, i);
        }
    }

    // 把叶子 children[i+1] 并入 children[i]
    static void merge_leaves(Node* parent, std::size_t i)
    {
        Node* a = parent->children[i];
        Node* b = parent->children[i + 1];
        a->keys.insert(a->keys.end(), b->keys.begin(), b->keys.end());
        a->values.insert(a->values.end(), b->values.begin(), b->values.end());
        a->next = b->next;
        delete b;
        parent->keys.erase(parent->keys.begin() + i);
        parent->children.erase(parent->children.begin() + i + 1);
    }

    // 把内部结点 children[i+1] 连同分隔键 keys[i] 并入 children[i]
    static void merge_internal(Node* parent, std::size_t i)
    {
        Node* a = parent->children[i];
        Node* b = parent->children[i + 1];
        a->keys.push_back(parent->keys[i]);
        a->keys.insert(a->keys.end(), b->keys.begin(), b->keys.end());
        a->children.insert(a->children.end(), b->children.begin(), b->children.end());
        b->children.clear();
        delete b;
        parent->keys.erase(parent->keys.begin() + i);
        parent->children.erase(parent->children.begin() + i + 1);
    }

    bool check(const Node* n, const K* lo, const K* hi, int depth, int& leaf_depth, bool is_root) const
    {
        std::size_t k = n->keys.size();
        if (k > kMaxKeys || (!is_root && k < kMinKeys))
            return false;
        for (std::size_t i = 0; i < k; ++i) {
            if (i > 0 && !(n->keys[i - 1] < n->keys[i])) return false;
            if (lo && n->keys[i] < *lo) return false;   // 允许等于下界
            if (hi && !(n->keys[i] < *hi)) return false;
        }
        if (n->leaf) {
            if (n->values.size() != k) return false;
            if (leaf_depth < 0) leaf_depth = depth;
            return leaf_depth == depth;
        }
        if (k == 0 || n->children.size() != k + 1)
            return false;
        for (std::size_t i = 0; i <= k; ++i) {
            const K* clo = i == 0 ? lo : &n->keys[i - 1];
            const K* chi = i == k ? hi : &n->keys[i];
            if (!check(n->children[i], clo, chi, depth + 1, leaf_depth, false))
                return false;
        }
        return true;
    }

    static void destroy(Node* n)
    {
        if (!n) return;
        for (Node* c : n->children)
            destroy(c);
        delete n;
    }

    Node* root_ = nullptr;
    std::size_t size_ = 0;
};
