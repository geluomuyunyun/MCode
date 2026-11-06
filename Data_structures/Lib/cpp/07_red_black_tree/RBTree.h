// RBTree.h - 红黑树（有序映射 K -> V），std::map 的典型底层实现。
//
// 五条性质：
//   1. 结点是红色或黑色      2. 根是黑色
//   3. 叶子（NIL）是黑色     4. 红结点的孩子都是黑色（不能连续两个红）
//   5. 任一结点到其所有后代 NIL 的路径上，黑结点个数相同（黑高）
// 由此最长路径不超过最短路径的 2 倍，树高 <= 2 log2(n+1)。
//
// 实现参照《算法导论》第 13 章：用一个共享的黑色哨兵 nil_ 代替所有空指针，
// 这样旋转与修复代码中不必判断 nullptr。
// 和 AVL 相比：红黑树平衡要求更宽松，插入删除时旋转次数更少（插入最多 2 次，删除最多 3 次）。
#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

template <typename K, typename V, typename Compare = std::less<K>>
class RBTree {
    enum Color : unsigned char { RED, BLACK };

    struct Node {
        K key;
        V value;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
    };

public:
    RBTree()
    {
        nil_ = new Node{K(), V(), BLACK, nullptr, nullptr, nullptr};
        nil_->left = nil_->right = nil_->parent = nil_;
        root_ = nil_;
    }
    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;
    ~RBTree()
    {
        destroy(root_);
        delete nil_;
    }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    V* find(const K& key)
    {
        Node* n = find_node(key);
        return n == nil_ ? nullptr : &n->value;
    }
    bool contains(const K& key) const { return const_cast<RBTree*>(this)->find_node(key) != nil_; }

    // 插入新键返回 true；键已存在则更新值并返回 false
    bool insert(const K& key, const V& value)
    {
        Node* parent = nil_;
        Node* cur = root_;
        while (cur != nil_) {
            parent = cur;
            if (less_(key, cur->key)) {
                cur = cur->left;
            } else if (less_(cur->key, key)) {
                cur = cur->right;
            } else {
                cur->value = value;
                return false;
            }
        }
        Node* z = new Node{key, value, RED, nil_, nil_, parent}; // 新结点总是红色，不破坏黑高
        if (parent == nil_)
            root_ = z;
        else if (less_(key, parent->key))
            parent->left = z;
        else
            parent->right = z;
        ++size_;
        insert_fixup(z);
        return true;
    }

    bool erase(const K& key)
    {
        Node* z = find_node(key);
        if (z == nil_)
            return false;
        Node* y = z;
        Color y_original = y->color;
        Node* x;
        if (z->left == nil_) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil_) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right); // 后继
            y_original = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y; // x 可能是 nil_，这里给 nil_ 设置 parent 是有意为之
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        --size_;
        if (y_original == BLACK) // 删掉（或移走）了一个黑结点，黑高被破坏
            erase_fixup(x);
        return true;
    }

    std::vector<std::pair<K, V>> to_sorted_vector() const
    {
        std::vector<std::pair<K, V>> out;
        inorder(root_, out);
        return out;
    }

    int height() const { return height(root_); }

    // 依次检查五条性质与 BST 有序性
    bool is_valid() const
    {
        if (root_->color != BLACK || nil_->color != BLACK)
            return false;
        const K* prev = nullptr;
        return black_height(root_, prev) >= 0;
    }

private:
    Node* find_node(const K& key)
    {
        Node* n = root_;
        while (n != nil_) {
            if (less_(key, n->key)) n = n->left;
            else if (less_(n->key, key)) n = n->right;
            else return n;
        }
        return nil_;
    }

    Node* minimum(Node* n) const
    {
        while (n->left != nil_)
            n = n->left;
        return n;
    }

    void left_rotate(Node* x)
    {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void right_rotate(Node* x)
    {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil_)
            y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_) root_ = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insert_fixup(Node* z)
    {
        while (z->parent->color == RED) {
            Node* gp = z->parent->parent;
            if (z->parent == gp->left) {
                Node* uncle = gp->right;
                if (uncle->color == RED) { // 情况 1：叔叔是红色，重新着色，问题上移两层
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->right) { // 情况 2：转换成情况 3
                        z = z->parent;
                        left_rotate(z);
                    }
                    z->parent->color = BLACK; // 情况 3：着色 + 旋转，修复完成
                    gp->color = RED;
                    right_rotate(gp);
                }
            } else { // 镜像
                Node* uncle = gp->left;
                if (uncle->color == RED) {
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    gp->color = RED;
                    z = gp;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = BLACK;
                    gp->color = RED;
                    left_rotate(gp);
                }
            }
        }
        root_->color = BLACK;
    }

    // 用以 v 为根的子树替换以 u 为根的子树
    void transplant(Node* u, Node* v)
    {
        if (u->parent == nil_) root_ = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }

    // x 带着"额外的一重黑色"，通过旋转和着色把它消掉
    void erase_fixup(Node* x)
    {
        while (x != root_ && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right; // 兄弟
                if (w->color == RED) {      // 情况 1：兄弟红 -> 转成兄弟黑
                    w->color = BLACK;
                    x->parent->color = RED;
                    left_rotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) { // 情况 2：兄弟的孩子都黑
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) { // 情况 3：转成情况 4
                        w->left->color = BLACK;
                        w->color = RED;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color; // 情况 4：旋转后修复完成
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    left_rotate(x->parent);
                    x = root_;
                }
            } else { // 镜像
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    right_rotate(x->parent);
                    x = root_;
                }
            }
        }
        x->color = BLACK;
    }

    void inorder(const Node* n, std::vector<std::pair<K, V>>& out) const
    {
        if (n == nil_) return;
        inorder(n->left, out);
        out.emplace_back(n->key, n->value);
        inorder(n->right, out);
    }

    int height(const Node* n) const
    {
        if (n == nil_) return 0;
        int l = height(n->left), r = height(n->right);
        return 1 + (l > r ? l : r);
    }

    // 返回黑高，任何性质被破坏返回 -1
    int black_height(const Node* n, const K*& prev) const
    {
        if (n == nil_) return 1;
        if (n->color == RED && (n->left->color == RED || n->right->color == RED)) return -1;
        if (n->left != nil_ && n->left->parent != n) return -1;
        if (n->right != nil_ && n->right->parent != n) return -1;
        int lh = black_height(n->left, prev);
        if (lh < 0) return -1;
        if (prev && !less_(*prev, n->key)) return -1;
        prev = &n->key;
        int rh = black_height(n->right, prev);
        if (rh < 0 || lh != rh) return -1;
        return lh + (n->color == BLACK ? 1 : 0);
    }

    void destroy(Node* n)
    {
        if (n == nil_) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    Node* nil_;
    Node* root_;
    std::size_t size_ = 0;
    Compare less_;
};
