// BinaryTree.h - 用 std::unique_ptr 管理结点的二叉树模板（对照 c/05_binary_tree）。
//
// 学习要点：
//   - 父结点用 unique_ptr 拥有孩子，整棵树随 root_ 自动释放，无需 bt_destroy。
//   - 遍历函数接收任意可调用对象（lambda），比 C 的"函数指针 + void* ctx"更自然。
//   - 非递归遍历使用 std::stack / std::queue，存的是观察用的裸指针（不拥有）。
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <optional>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

template <typename T>
class BinaryTree {
public:
    struct Node {
        T value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(T v) : value(std::move(v)) {}
    };

    BinaryTree() = default;

    // 层序数组建树，std::nullopt 表示空结点
    static BinaryTree from_level_order(const std::vector<std::optional<T>>& vals)
    {
        BinaryTree t;
        if (vals.empty() || !vals[0])
            return t;
        t.root_ = std::make_unique<Node>(*vals[0]);
        std::queue<Node*> q;
        q.push(t.root_.get());
        std::size_t i = 1;
        while (!q.empty() && i < vals.size()) {
            Node* cur = q.front();
            q.pop();
            if (i < vals.size() && vals[i]) {
                cur->left = std::make_unique<Node>(*vals[i]);
                q.push(cur->left.get());
            }
            ++i;
            if (i < vals.size() && vals[i]) {
                cur->right = std::make_unique<Node>(*vals[i]);
                q.push(cur->right.get());
            }
            ++i;
        }
        return t;
    }

    // 由前序 + 中序重建（结点值互不相同）
    static BinaryTree from_pre_in(const std::vector<T>& pre, const std::vector<T>& in)
    {
        BinaryTree t;
        t.root_ = build_pre_in(pre, 0, in, 0, in.size());
        return t;
    }

    BinaryTree(const BinaryTree& other) : root_(clone(other.root_.get())) {}
    BinaryTree(BinaryTree&&) noexcept = default;
    BinaryTree& operator=(BinaryTree other) noexcept
    {
        root_ = std::move(other.root_);
        return *this;
    }

    const Node* root() const { return root_.get(); }
    bool empty() const { return !root_; }

    // ---------- 递归遍历 ----------
    template <typename F> void preorder(F&& f) const { preorder(root_.get(), f); }
    template <typename F> void inorder(F&& f) const { inorder(root_.get(), f); }
    template <typename F> void postorder(F&& f) const { postorder(root_.get(), f); }

    // ---------- 非递归遍历，返回结果序列 ----------
    std::vector<T> preorder_iter() const
    {
        std::vector<T> out;
        std::stack<const Node*> st;
        if (root_)
            st.push(root_.get());
        while (!st.empty()) {
            const Node* n = st.top();
            st.pop();
            out.push_back(n->value);
            if (n->right) st.push(n->right.get());
            if (n->left) st.push(n->left.get());
        }
        return out;
    }

    std::vector<T> inorder_iter() const
    {
        std::vector<T> out;
        std::stack<const Node*> st;
        const Node* cur = root_.get();
        while (cur || !st.empty()) {
            for (; cur; cur = cur->left.get())
                st.push(cur);
            cur = st.top();
            st.pop();
            out.push_back(cur->value);
            cur = cur->right.get();
        }
        return out;
    }

    // 后序 = "根右左"的逆序，这是另一种常见写法（C 版用的是 last 指针法）
    std::vector<T> postorder_iter() const
    {
        std::vector<T> out;
        std::stack<const Node*> st;
        if (root_)
            st.push(root_.get());
        while (!st.empty()) {
            const Node* n = st.top();
            st.pop();
            out.push_back(n->value);
            if (n->left) st.push(n->left.get());
            if (n->right) st.push(n->right.get());
        }
        std::reverse(out.begin(), out.end());
        return out;
    }

    // 分层返回：每一层一个 vector
    std::vector<std::vector<T>> level_order() const
    {
        std::vector<std::vector<T>> levels;
        std::queue<const Node*> q;
        if (root_)
            q.push(root_.get());
        while (!q.empty()) {
            levels.emplace_back();
            for (std::size_t k = q.size(); k > 0; --k) { // 当前队列里恰好是一整层
                const Node* n = q.front();
                q.pop();
                levels.back().push_back(n->value);
                if (n->left) q.push(n->left.get());
                if (n->right) q.push(n->right.get());
            }
        }
        return levels;
    }

    // ---------- 其他算法 ----------
    int height() const { return height(root_.get()); }
    std::size_t size() const { return size(root_.get()); }
    void mirror() { mirror(root_.get()); }
    bool is_symmetric() const { return !root_ || mirror_equal(root_->left.get(), root_->right.get()); }
    bool is_balanced() const { return balanced_height(root_.get()) >= 0; }

    // 最近公共祖先，任一值不存在时返回 nullopt
    std::optional<T> lowest_common_ancestor(const T& a, const T& b) const
    {
        if (!contains(root_.get(), a) || !contains(root_.get(), b))
            return std::nullopt;
        const Node* n = lca(root_.get(), a, b);
        return n ? std::optional<T>(n->value) : std::nullopt;
    }

    // 根到叶子的最大路径和
    T max_root_to_leaf_sum() const { return max_path(root_.get()); }

    friend bool operator==(const BinaryTree& a, const BinaryTree& b) { return same(a.root_.get(), b.root_.get()); }

private:
    static std::unique_ptr<Node> clone(const Node* n)
    {
        if (!n)
            return nullptr;
        auto c = std::make_unique<Node>(n->value);
        c->left = clone(n->left.get());
        c->right = clone(n->right.get());
        return c;
    }

    static std::unique_ptr<Node> build_pre_in(const std::vector<T>& pre, std::size_t pl,
                                              const std::vector<T>& in, std::size_t il, std::size_t n)
    {
        if (n == 0)
            return nullptr;
        std::size_t k = 0;
        while (k < n && !(in[il + k] == pre[pl]))
            ++k;
        auto node = std::make_unique<Node>(pre[pl]);
        node->left = build_pre_in(pre, pl + 1, in, il, k);
        node->right = build_pre_in(pre, pl + 1 + k, in, il + k + 1, n - k - 1);
        return node;
    }

    template <typename F> static void preorder(const Node* n, F& f)
    {
        if (!n) return;
        f(n->value);
        preorder(n->left.get(), f);
        preorder(n->right.get(), f);
    }
    template <typename F> static void inorder(const Node* n, F& f)
    {
        if (!n) return;
        inorder(n->left.get(), f);
        f(n->value);
        inorder(n->right.get(), f);
    }
    template <typename F> static void postorder(const Node* n, F& f)
    {
        if (!n) return;
        postorder(n->left.get(), f);
        postorder(n->right.get(), f);
        f(n->value);
    }

    static int height(const Node* n)
    {
        return n ? 1 + std::max(height(n->left.get()), height(n->right.get())) : 0;
    }
    static std::size_t size(const Node* n)
    {
        return n ? 1 + size(n->left.get()) + size(n->right.get()) : 0;
    }
    static void mirror(Node* n)
    {
        if (!n) return;
        std::swap(n->left, n->right);
        mirror(n->left.get());
        mirror(n->right.get());
    }
    static bool mirror_equal(const Node* a, const Node* b)
    {
        if (!a || !b) return a == b;
        return a->value == b->value && mirror_equal(a->left.get(), b->right.get()) &&
               mirror_equal(a->right.get(), b->left.get());
    }
    // 平衡返回高度，不平衡返回 -1（一次遍历同时完成两件事）
    static int balanced_height(const Node* n)
    {
        if (!n) return 0;
        int l = balanced_height(n->left.get());
        int r = balanced_height(n->right.get());
        if (l < 0 || r < 0 || std::abs(l - r) > 1) return -1;
        return 1 + std::max(l, r);
    }
    static bool contains(const Node* n, const T& v)
    {
        return n && (n->value == v || contains(n->left.get(), v) || contains(n->right.get(), v));
    }
    static const Node* lca(const Node* n, const T& a, const T& b)
    {
        if (!n || n->value == a || n->value == b) return n;
        const Node* l = lca(n->left.get(), a, b);
        const Node* r = lca(n->right.get(), a, b);
        if (l && r) return n; // a、b 分居两侧，当前结点就是答案
        return l ? l : r;
    }
    static T max_path(const Node* n)
    {
        if (!n) return T();
        if (!n->left) return n->value + max_path(n->right.get());
        if (!n->right) return n->value + max_path(n->left.get());
        return n->value + std::max(max_path(n->left.get()), max_path(n->right.get()));
    }
    static bool same(const Node* a, const Node* b)
    {
        if (!a || !b) return a == b;
        return a->value == b->value && same(a->left.get(), b->left.get()) && same(a->right.get(), b->right.get());
    }

    std::unique_ptr<Node> root_;
};
