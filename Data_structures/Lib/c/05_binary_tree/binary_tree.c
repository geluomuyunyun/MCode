#include "binary_tree.h"

#include <stdlib.h>

/* ---------- 内部使用的指针栈 ---------- */

typedef struct {
    const TreeNode **data;
    size_t size;
    size_t capacity;
} PtrStack;

static int ps_init(PtrStack *s)
{
    s->capacity = 16;
    s->size = 0;
    s->data = (const TreeNode **)malloc(s->capacity * sizeof(*s->data));
    return s->data ? 0 : -1;
}

static int ps_push(PtrStack *s, const TreeNode *n)
{
    if (s->size == s->capacity) {
        const TreeNode **p = (const TreeNode **)realloc((void *)s->data, s->capacity * 2 * sizeof(*p));
        if (!p)
            return -1;
        s->data = p;
        s->capacity *= 2;
    }
    s->data[s->size++] = n;
    return 0;
}

static const TreeNode *ps_pop(PtrStack *s)
{
    return s->data[--s->size];
}

static const TreeNode *ps_top(const PtrStack *s)
{
    return s->data[s->size - 1];
}

static void ps_free(PtrStack *s)
{
    free((void *)s->data);
}

/* ---------- 构建与销毁 ---------- */

TreeNode *bt_new_node(int val)
{
    TreeNode *n = (TreeNode *)malloc(sizeof(TreeNode));
    if (n) {
        n->val = val;
        n->left = n->right = NULL;
    }
    return n;
}

void bt_destroy(TreeNode *root)
{
    if (!root)
        return;
    bt_destroy(root->left);
    bt_destroy(root->right);
    free(root);
}

TreeNode *bt_build_level(const int *vals, size_t n, int null_marker)
{
    if (n == 0 || vals[0] == null_marker)
        return NULL;
    /* 用数组模拟队列：nodes[head] 是当前等待挂孩子的结点 */
    TreeNode **nodes = (TreeNode **)malloc(n * sizeof(TreeNode *));
    if (!nodes)
        return NULL;
    size_t head = 0, tail = 0;
    TreeNode *root = bt_new_node(vals[0]);
    nodes[tail++] = root;
    size_t i = 1;
    while (head < tail && i < n) {
        TreeNode *cur = nodes[head++];
        if (i < n && vals[i] != null_marker) {
            cur->left = bt_new_node(vals[i]);
            nodes[tail++] = cur->left;
        }
        ++i;
        if (i < n && vals[i] != null_marker) {
            cur->right = bt_new_node(vals[i]);
            nodes[tail++] = cur->right;
        }
        ++i;
    }
    free(nodes);
    return root;
}

TreeNode *bt_build_pre_in(const int *pre, const int *in, size_t n)
{
    if (n == 0)
        return NULL;
    /* 前序第一个是根；它在中序中的位置把中序分成左右子树 */
    size_t k = 0;
    while (k < n && in[k] != pre[0])
        ++k;
    if (k == n)
        return NULL; /* 输入不合法 */
    TreeNode *root = bt_new_node(pre[0]);
    if (!root)
        return NULL;
    root->left = bt_build_pre_in(pre + 1, in, k);
    root->right = bt_build_pre_in(pre + 1 + k, in + k + 1, n - k - 1);
    return root;
}

/* ---------- 递归遍历 ---------- */

void bt_preorder(const TreeNode *root, VisitFn visit, void *ctx)
{
    if (!root)
        return;
    visit(root->val, ctx);
    bt_preorder(root->left, visit, ctx);
    bt_preorder(root->right, visit, ctx);
}

void bt_inorder(const TreeNode *root, VisitFn visit, void *ctx)
{
    if (!root)
        return;
    bt_inorder(root->left, visit, ctx);
    visit(root->val, ctx);
    bt_inorder(root->right, visit, ctx);
}

void bt_postorder(const TreeNode *root, VisitFn visit, void *ctx)
{
    if (!root)
        return;
    bt_postorder(root->left, visit, ctx);
    bt_postorder(root->right, visit, ctx);
    visit(root->val, ctx);
}

/* ---------- 非递归遍历 ---------- */

int bt_preorder_iter(const TreeNode *root, VisitFn visit, void *ctx)
{
    if (!root)
        return 0;
    PtrStack s;
    if (ps_init(&s) != 0)
        return -1;
    ps_push(&s, root);
    while (s.size > 0) {
        const TreeNode *n = ps_pop(&s);
        visit(n->val, ctx);
        /* 先压右再压左，出栈时左子树先被访问 */
        if (n->right)
            ps_push(&s, n->right);
        if (n->left)
            ps_push(&s, n->left);
    }
    ps_free(&s);
    return 0;
}

int bt_inorder_iter(const TreeNode *root, VisitFn visit, void *ctx)
{
    PtrStack s;
    if (ps_init(&s) != 0)
        return -1;
    const TreeNode *cur = root;
    while (cur || s.size > 0) {
        while (cur) { /* 一路向左，把路径压栈 */
            ps_push(&s, cur);
            cur = cur->left;
        }
        cur = ps_pop(&s);
        visit(cur->val, ctx);
        cur = cur->right;
    }
    ps_free(&s);
    return 0;
}

int bt_postorder_iter(const TreeNode *root, VisitFn visit, void *ctx)
{
    PtrStack s;
    if (ps_init(&s) != 0)
        return -1;
    const TreeNode *cur = root, *last = NULL; /* last：上一个被访问的结点 */
    while (cur || s.size > 0) {
        while (cur) {
            ps_push(&s, cur);
            cur = cur->left;
        }
        const TreeNode *top = ps_top(&s);
        if (top->right && top->right != last) {
            cur = top->right; /* 右子树还没访问，先去右边 */
        } else {
            visit(top->val, ctx);
            last = ps_pop(&s);
        }
    }
    ps_free(&s);
    return 0;
}

int bt_levelorder(const TreeNode *root, VisitFn visit, void *ctx)
{
    if (!root)
        return 0;
    size_t n = bt_count(root);
    const TreeNode **q = (const TreeNode **)malloc(n * sizeof(*q));
    if (!q)
        return -1;
    size_t head = 0, tail = 0;
    q[tail++] = root;
    while (head < tail) {
        const TreeNode *cur = q[head++];
        visit(cur->val, ctx);
        if (cur->left)
            q[tail++] = cur->left;
        if (cur->right)
            q[tail++] = cur->right;
    }
    free((void *)q);
    return 0;
}

/* ---------- 其他 ---------- */

int bt_height(const TreeNode *root)
{
    if (!root)
        return 0;
    int l = bt_height(root->left), r = bt_height(root->right);
    return 1 + (l > r ? l : r);
}

size_t bt_count(const TreeNode *root)
{
    return root ? 1 + bt_count(root->left) + bt_count(root->right) : 0;
}

size_t bt_leaf_count(const TreeNode *root)
{
    if (!root)
        return 0;
    if (!root->left && !root->right)
        return 1;
    return bt_leaf_count(root->left) + bt_leaf_count(root->right);
}

void bt_mirror(TreeNode *root)
{
    if (!root)
        return;
    TreeNode *t = root->left;
    root->left = root->right;
    root->right = t;
    bt_mirror(root->left);
    bt_mirror(root->right);
}

int bt_is_same(const TreeNode *a, const TreeNode *b)
{
    if (!a || !b)
        return a == b;
    return a->val == b->val && bt_is_same(a->left, b->left) && bt_is_same(a->right, b->right);
}
