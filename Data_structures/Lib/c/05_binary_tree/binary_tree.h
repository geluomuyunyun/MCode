/*
 * binary_tree.h - 普通二叉树及其遍历算法。
 *
 * 遍历结果通过回调函数 visit(val, ctx) 输出，ctx 由调用者自定义。
 * 递归版与非递归版（显式栈）都实现了，便于对照理解。
 */
#ifndef DSA_C_BINARY_TREE_H
#define DSA_C_BINARY_TREE_H

#include <stddef.h>

typedef struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef void (*VisitFn)(int val, void *ctx);

TreeNode *bt_new_node(int val);
void bt_destroy(TreeNode *root);

/*
 * 按层序数组建树，null_marker 表示空结点。
 * 例如 {1, 2, 3, NUL, 4} 表示：1 的左孩子 2、右孩子 3，2 的右孩子 4。
 */
TreeNode *bt_build_level(const int *vals, size_t n, int null_marker);
/* 由前序 + 中序序列重建二叉树（要求结点值互不相同） */
TreeNode *bt_build_pre_in(const int *pre, const int *in, size_t n);

void bt_preorder(const TreeNode *root, VisitFn visit, void *ctx);
void bt_inorder(const TreeNode *root, VisitFn visit, void *ctx);
void bt_postorder(const TreeNode *root, VisitFn visit, void *ctx);

int bt_preorder_iter(const TreeNode *root, VisitFn visit, void *ctx);
int bt_inorder_iter(const TreeNode *root, VisitFn visit, void *ctx);
int bt_postorder_iter(const TreeNode *root, VisitFn visit, void *ctx);
int bt_levelorder(const TreeNode *root, VisitFn visit, void *ctx);

int bt_height(const TreeNode *root); /* 空树高度为 0 */
size_t bt_count(const TreeNode *root);
size_t bt_leaf_count(const TreeNode *root);
void bt_mirror(TreeNode *root);
int bt_is_same(const TreeNode *a, const TreeNode *b);

#endif
