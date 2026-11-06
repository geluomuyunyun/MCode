#include "bst.h"

#include <limits.h>
#include <stdlib.h>

BST *bst_create(void)
{
    BST *t = (BST *)malloc(sizeof(BST));
    if (t) {
        t->root = NULL;
        t->size = 0;
    }
    return t;
}

static void free_nodes(BSTNode *n)
{
    if (!n)
        return;
    free_nodes(n->left);
    free_nodes(n->right);
    free(n);
}

void bst_destroy(BST *t)
{
    if (!t)
        return;
    free_nodes(t->root);
    free(t);
}

int bst_insert(BST *t, int key)
{
    /* link 指向"应该挂新结点的那个指针"，迭代写法无需记录父结点 */
    BSTNode **link = &t->root;
    while (*link) {
        if (key == (*link)->key)
            return 0;
        link = key < (*link)->key ? &(*link)->left : &(*link)->right;
    }
    BSTNode *n = (BSTNode *)malloc(sizeof(BSTNode));
    if (!n)
        return -1;
    n->key = key;
    n->left = n->right = NULL;
    *link = n;
    t->size++;
    return 1;
}

int bst_contains(const BST *t, int key)
{
    const BSTNode *n = t->root;
    while (n) {
        if (key == n->key)
            return 1;
        n = key < n->key ? n->left : n->right;
    }
    return 0;
}

int bst_remove(BST *t, int key)
{
    BSTNode **link = &t->root;
    while (*link && (*link)->key != key)
        link = key < (*link)->key ? &(*link)->left : &(*link)->right;
    if (!*link)
        return 0;

    BSTNode *node = *link;
    if (node->left && node->right) {
        /* 两个孩子：找右子树最小结点（后继），把它的键搬上来，再删掉后继 */
        BSTNode **succ = &node->right;
        while ((*succ)->left)
            succ = &(*succ)->left;
        BSTNode *s = *succ;
        node->key = s->key;
        *succ = s->right; /* 后继没有左孩子 */
        free(s);
    } else {
        *link = node->left ? node->left : node->right;
        free(node);
    }
    t->size--;
    return 1;
}

int bst_min(const BST *t, int *out)
{
    const BSTNode *n = t->root;
    if (!n)
        return -1;
    while (n->left)
        n = n->left;
    *out = n->key;
    return 0;
}

int bst_max(const BST *t, int *out)
{
    const BSTNode *n = t->root;
    if (!n)
        return -1;
    while (n->right)
        n = n->right;
    *out = n->key;
    return 0;
}

int bst_floor(const BST *t, int key, int *out)
{
    const BSTNode *n = t->root;
    int found = 0;
    while (n) {
        if (n->key == key) {
            *out = key;
            return 0;
        }
        if (n->key < key) {
            *out = n->key; /* 候选答案，继续往右找更大的 */
            found = 1;
            n = n->right;
        } else {
            n = n->left;
        }
    }
    return found ? 0 : -1;
}

int bst_ceil(const BST *t, int key, int *out)
{
    const BSTNode *n = t->root;
    int found = 0;
    while (n) {
        if (n->key == key) {
            *out = key;
            return 0;
        }
        if (n->key > key) {
            *out = n->key;
            found = 1;
            n = n->left;
        } else {
            n = n->right;
        }
    }
    return found ? 0 : -1;
}

static const BSTNode *kth_node(const BSTNode *n, size_t *k)
{
    if (!n)
        return NULL;
    const BSTNode *r = kth_node(n->left, k);
    if (r)
        return r;
    if (--*k == 0)
        return n;
    return kth_node(n->right, k);
}

int bst_kth(const BST *t, size_t k, int *out)
{
    if (k == 0 || k > t->size)
        return -1;
    const BSTNode *n = kth_node(t->root, &k);
    *out = n->key;
    return 0;
}

static void inorder(const BSTNode *n, BSTVisitFn visit, void *ctx)
{
    if (!n)
        return;
    inorder(n->left, visit, ctx);
    visit(n->key, ctx);
    inorder(n->right, visit, ctx);
}

void bst_inorder(const BST *t, BSTVisitFn visit, void *ctx)
{
    inorder(t->root, visit, ctx);
}

static int height(const BSTNode *n)
{
    if (!n)
        return 0;
    int l = height(n->left), r = height(n->right);
    return 1 + (l > r ? l : r);
}

int bst_height(const BST *t)
{
    return height(t->root);
}

size_t bst_size(const BST *t)
{
    return t->size;
}

/* 每个结点的键必须落在 (lo, hi) 开区间内；用 long long 避免 INT_MIN/INT_MAX 边界问题 */
static int valid(const BSTNode *n, long long lo, long long hi)
{
    if (!n)
        return 1;
    if (n->key <= lo || n->key >= hi)
        return 0;
    return valid(n->left, lo, n->key) && valid(n->right, n->key, hi);
}

int bst_is_valid(const BST *t)
{
    return valid(t->root, (long long)INT_MIN - 1, (long long)INT_MAX + 1);
}
