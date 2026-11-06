// BTree.h - B 树（有序集合），参照《算法导论》第 18 章。
//
// 最小度数 t（模板参数 MinDegree，t >= 2）：
//   - 每个非根结点至少 t-1 个键，所有结点至多 2t-1 个键
//   - 内部结点的孩子数 = 键数 + 1
//   - 所有叶子在同一层
// B 树又矮又胖，一个结点可以对应磁盘上的一页，因此常用于数据库和文件系统。
//
// 插入：自顶向下，遇到满结点先分裂（split_child），保证插入时父结点不满。
// 删除：自顶向下，进入孩子前保证它至少有 t 个键（向兄弟借，或与兄弟合并）。
#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

template <typename K, int MinDegree = 3>
class BTree {
    static_assert(MinDegree >= 2, "B-tree minimum degree must be at least 2");
    static constexpr std::size_t t = MinDegree;

    struct Node {
        std::vector<K> keys;
        std::vector<std::unique_ptr<Node>> children; // 叶子结点为空
        bool leaf() const { return children.empty(); }
        bool full() const { return keys.size() == 2 * t - 1; }
    };

public:
    bool contains(const K& key) const
    {
        const Node* n = root_.get();
        while (n) {
            std::size_t i = std::lower_bound(n->keys.begin(), n->keys.end(), key) - n->keys.begin();
            if (i < n->keys.size() && !(key < n->keys[i]))
                return true;
            n = n->leaf() ? nullptr : n->children[i].get();
        }
        return false;
    }

    bool insert(const K& key)
    {
        if (contains(key))
            return false;
        if (!root_)
            root_ = std::make_unique<Node>();
        if (root_->full()) {
            // 根满了：新建一个根，旧根作为它的孩子再分裂。这是 B 树长高的唯一方式
            auto s = std::make_unique<Node>();
            s->children.push_back(std::move(root_));
            split_child(s.get(), 0);
            root_ = std::move(s);
        }
        insert_nonfull(root_.get(), key);
        ++size_;
        return true;
    }

    bool erase(const K& key)
    {
        if (!root_ || !erase(root_.get(), key))
            return false;
        --size_;
        if (root_->keys.empty()) // 根被合并空了：树变矮
            root_ = root_->leaf() ? nullptr : std::move(root_->children[0]);
        return true;
    }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    int height() const
    {
        int h = 0;
        for (const Node* n = root_.get(); n; n = n->leaf() ? nullptr : n->children[0].get())
            ++h;
        return h;
    }

    std::vector<K> to_sorted_vector() const
    {
        std::vector<K> out;
        inorder(root_.get(), out);
        return out;
    }

    // 校验键数范围、有序性、孩子数、叶子同层
    bool is_valid() const
    {
        if (!root_)
            return size_ == 0;
        int leaf_depth = -1;
        std::size_t count = 0;
        return check(root_.get(), nullptr, nullptr, 0, leaf_depth, true, count) && count == size_;
    }

private:
    // x 的第 i 个孩子 y 已满（2t-1 个键）：把 y 拆成两个 t-1 个键的结点，中间键上移到 x
    static void split_child(Node* x, std::size_t i)
    {
        Node* y = x->children[i].get();
        auto z = std::make_unique<Node>();
        z->keys.assign(y->keys.begin() + t, y->keys.end());
        if (!y->leaf()) {
            for (std::size_t j = t; j < y->children.size(); ++j)
                z->children.push_back(std::move(y->children[j]));
            y->children.resize(t);
        }
        K mid = y->keys[t - 1];
        y->keys.resize(t - 1);
        x->keys.insert(x->keys.begin() + i, mid);
        x->children.insert(x->children.begin() + i + 1, std::move(z));
    }

    static void insert_nonfull(Node* x, const K& key)
    {
        while (!x->leaf()) {
            std::size_t i = std::upper_bound(x->keys.begin(), x->keys.end(), key) - x->keys.begin();
            if (x->children[i]->full()) {
                split_child(x, i);
                if (x->keys[i] < key)
                    ++i;
            }
            x = x->children[i].get();
        }
        x->keys.insert(std::upper_bound(x->keys.begin(), x->keys.end(), key), key);
    }

    // 前提：x 是根，或者 x 至少有 t 个键
    bool erase(Node* x, const K& key)
    {
        std::size_t idx = std::lower_bound(x->keys.begin(), x->keys.end(), key) - x->keys.begin();
        bool here = idx < x->keys.size() && !(key < x->keys[idx]);

        if (here) {
            if (x->leaf()) { // 情况 1：键在叶子中，直接删
                x->keys.erase(x->keys.begin() + idx);
                return true;
            }
            Node* left = x->children[idx].get();
            Node* right = x->children[idx + 1].get();
            if (left->keys.size() >= t) { // 情况 2a：用前驱替换
                K pred = max_key(left);
                x->keys[idx] = pred;
                return erase(left, pred);
            }
            if (right->keys.size() >= t) { // 情况 2b：用后继替换
                K succ = min_key(right);
                x->keys[idx] = succ;
                return erase(right, succ);
            }
            merge(x, idx); // 情况 2c：左右孩子都只有 t-1 个键，合并后再删
            return erase(left, key);
        }

        if (x->leaf())
            return false;
        // 情况 3：键在子树 children[idx] 中，下降前保证它至少有 t 个键
        if (x->children[idx]->keys.size() < t) {
            bool was_last = idx == x->keys.size();
            fill(x, idx);
            if (was_last && idx > x->keys.size()) // 最后一个孩子与左兄弟合并了
                --idx;
        }
        return erase(x->children[idx].get(), key);
    }

    static K max_key(const Node* n)
    {
        while (!n->leaf())
            n = n->children.back().get();
        return n->keys.back();
    }

    static K min_key(const Node* n)
    {
        while (!n->leaf())
            n = n->children.front().get();
        return n->keys.front();
    }

    static void fill(Node* x, std::size_t idx)
    {
        if (idx > 0 && x->children[idx - 1]->keys.size() >= t)
            borrow_from_prev(x, idx);
        else if (idx < x->keys.size() && x->children[idx + 1]->keys.size() >= t)
            borrow_from_next(x, idx);
        else if (idx < x->keys.size())
            merge(x, idx);
        else
            merge(x, idx - 1);
    }

    // 父结点的分隔键下移到 child 头部，左兄弟的最大键上移到父结点
    static void borrow_from_prev(Node* x, std::size_t idx)
    {
        Node* child = x->children[idx].get();
        Node* sib = x->children[idx - 1].get();
        child->keys.insert(child->keys.begin(), x->keys[idx - 1]);
        if (!child->leaf()) {
            child->children.insert(child->children.begin(), std::move(sib->children.back()));
            sib->children.pop_back();
        }
        x->keys[idx - 1] = sib->keys.back();
        sib->keys.pop_back();
    }

    static void borrow_from_next(Node* x, std::size_t idx)
    {
        Node* child = x->children[idx].get();
        Node* sib = x->children[idx + 1].get();
        child->keys.push_back(x->keys[idx]);
        if (!child->leaf()) {
            child->children.push_back(std::move(sib->children.front()));
            sib->children.erase(sib->children.begin());
        }
        x->keys[idx] = sib->keys.front();
        sib->keys.erase(sib->keys.begin());
    }

    // children[idx] + keys[idx] + children[idx+1] 合并为一个结点（共 2t-1 个键）
    static void merge(Node* x, std::size_t idx)
    {
        Node* child = x->children[idx].get();
        Node* sib = x->children[idx + 1].get();
        child->keys.push_back(x->keys[idx]);
        child->keys.insert(child->keys.end(), sib->keys.begin(), sib->keys.end());
        for (auto& c : sib->children)
            child->children.push_back(std::move(c));
        x->keys.erase(x->keys.begin() + idx);
        x->children.erase(x->children.begin() + idx + 1);
    }

    static void inorder(const Node* n, std::vector<K>& out)
    {
        if (!n) return;
        for (std::size_t i = 0; i < n->keys.size(); ++i) {
            if (!n->leaf()) inorder(n->children[i].get(), out);
            out.push_back(n->keys[i]);
        }
        if (!n->leaf()) inorder(n->children.back().get(), out);
    }

    static bool check(const Node* n, const K* lo, const K* hi, int depth, int& leaf_depth, bool is_root,
                      std::size_t& count)
    {
        std::size_t k = n->keys.size();
        if (k > 2 * t - 1 || (!is_root && k < t - 1) || (is_root && k == 0))
            return false;
        for (std::size_t i = 0; i < k; ++i) {
            if (i > 0 && !(n->keys[i - 1] < n->keys[i])) return false;
            if (lo && !(*lo < n->keys[i])) return false;
            if (hi && !(n->keys[i] < *hi)) return false;
        }
        count += k;
        if (n->leaf()) {
            if (leaf_depth < 0) leaf_depth = depth;
            return leaf_depth == depth;
        }
        if (n->children.size() != k + 1)
            return false;
        for (std::size_t i = 0; i <= k; ++i) {
            const K* clo = i == 0 ? lo : &n->keys[i - 1];
            const K* chi = i == k ? hi : &n->keys[i];
            if (!check(n->children[i].get(), clo, chi, depth + 1, leaf_depth, false, count))
                return false;
        }
        return true;
    }

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;
};
