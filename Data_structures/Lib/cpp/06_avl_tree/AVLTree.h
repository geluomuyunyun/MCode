// AVLTree.h - AVL 平衡二叉搜索树（有序集合）。
//
// 平衡条件：每个结点左右子树高度差（平衡因子）的绝对值不超过 1，
// 因此树高始终是 O(log n)，避免了 c/06_bst 中有序插入退化成链表的问题。
//
// 四种失衡与旋转：
//   LL：右旋            RR：左旋
//   LR：先左旋左孩子，再右旋   RL：先右旋右孩子，再左旋
// 插入和删除都用递归实现：在回溯路径上逐个结点更新高度并 rebalance。
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

template <typename K, typename Compare = std::less<K>>
class AVLTree {
    struct Node {
        K key;
        int height = 1;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(const K& k) : key(k) {}
    };

public:
    AVLTree() = default;
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    AVLTree(AVLTree&& o) noexcept : root_(std::exchange(o.root_, nullptr)), size_(std::exchange(o.size_, 0)) {}
    ~AVLTree() { destroy(root_); }

    bool insert(const K& key)
    {
        bool inserted = false;
        root_ = insert(root_, key, inserted);
        if (inserted)
            ++size_;
        return inserted;
    }

    bool erase(const K& key)
    {
        bool erased = false;
        root_ = erase(root_, key, erased);
        if (erased)
            --size_;
        return erased;
    }

    bool contains(const K& key) const
    {
        const Node* n = root_;
        while (n) {
            if (less_(key, n->key)) n = n->left;
            else if (less_(n->key, key)) n = n->right;
            else return true;
        }
        return false;
    }

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    int height() const noexcept { return h(root_); }

    std::vector<K> to_sorted_vector() const
    {
        std::vector<K> out;
        out.reserve(size_);
        inorder(root_, out);
        return out;
    }

    // 校验：BST 有序性 + 高度字段正确 + 平衡因子合法
    bool is_valid() const
    {
        const K* prev = nullptr;
        return check(root_, prev) >= 0;
    }

private:
    static int h(const Node* n) { return n ? n->height : 0; }
    static int balance_factor(const Node* n) { return h(n->left) - h(n->right); }
    static void update(Node* n) { n->height = 1 + std::max(h(n->left), h(n->right)); }

    //     y            x
    //    / \          / \
    //   x   C  ==>   A   y
    //  / \              / \
    // A   B            B   C
    static Node* rotate_right(Node* y)
    {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        update(y);
        update(x);
        return x;
    }

    static Node* rotate_left(Node* x)
    {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        update(x);
        update(y);
        return y;
    }

    static Node* rebalance(Node* n)
    {
        update(n);
        int bf = balance_factor(n);
        if (bf > 1) {                        // 左边高
            if (balance_factor(n->left) < 0) // LR
                n->left = rotate_left(n->left);
            return rotate_right(n);          // LL
        }
        if (bf < -1) {                        // 右边高
            if (balance_factor(n->right) > 0) // RL
                n->right = rotate_right(n->right);
            return rotate_left(n);            // RR
        }
        return n;
    }

    Node* insert(Node* n, const K& key, bool& inserted)
    {
        if (!n) {
            inserted = true;
            return new Node(key);
        }
        if (less_(key, n->key))
            n->left = insert(n->left, key, inserted);
        else if (less_(n->key, key))
            n->right = insert(n->right, key, inserted);
        else
            return n; // 已存在
        return rebalance(n);
    }

    Node* erase(Node* n, const K& key, bool& erased)
    {
        if (!n)
            return nullptr;
        if (less_(key, n->key)) {
            n->left = erase(n->left, key, erased);
        } else if (less_(n->key, key)) {
            n->right = erase(n->right, key, erased);
        } else {
            erased = true;
            if (!n->left || !n->right) {
                Node* child = n->left ? n->left : n->right;
                delete n;
                return child;
            }
            // 两个孩子：用后继的键替换，再到右子树里删除后继
            Node* succ = n->right;
            while (succ->left)
                succ = succ->left;
            n->key = succ->key;
            bool dummy = false;
            n->right = erase(n->right, n->key, dummy);
        }
        return rebalance(n);
    }

    static void inorder(const Node* n, std::vector<K>& out)
    {
        if (!n) return;
        inorder(n->left, out);
        out.push_back(n->key);
        inorder(n->right, out);
    }

    // 返回高度，不合法返回 -1
    int check(const Node* n, const K*& prev) const
    {
        if (!n) return 0;
        int lh = check(n->left, prev);
        if (lh < 0) return -1;
        if (prev && !less_(*prev, n->key)) return -1;
        prev = &n->key;
        int rh = check(n->right, prev);
        if (rh < 0) return -1;
        if (std::abs(lh - rh) > 1 || n->height != 1 + std::max(lh, rh)) return -1;
        return n->height;
    }

    static void destroy(Node* n)
    {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    Node* root_ = nullptr;
    std::size_t size_ = 0;
    Compare less_;
};
