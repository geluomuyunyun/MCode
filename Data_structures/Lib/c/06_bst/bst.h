/*
 * bst.h - 二叉搜索树（BST），键为 int，不允许重复。
 *
 * 性质：左子树所有键 < 根 < 右子树所有键，中序遍历得到升序序列。
 * 平均 O(log n)，退化成链时 O(n)（平衡树见 cpp/06_avl_tree）。
 *
 * 删除的三种情况：叶子、只有一个孩子、有两个孩子（用后继替换）。
 */
#ifndef DSA_C_BST_H
#define DSA_C_BST_H

#include <stddef.h>

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    size_t size;
} BST;

typedef void (*BSTVisitFn)(int key, void *ctx);

BST *bst_create(void);
void bst_destroy(BST *t);

int bst_insert(BST *t, int key); /* 插入返回 1，已存在返回 0，内存不足返回 -1 */
int bst_contains(const BST *t, int key);
int bst_remove(BST *t, int key); /* 删除返回 1，不存在返回 0 */

int bst_min(const BST *t, int *out);
int bst_max(const BST *t, int *out);
int bst_floor(const BST *t, int key, int *out); /* 小于等于 key 的最大键 */
int bst_ceil(const BST *t, int key, int *out);  /* 大于等于 key 的最小键 */
int bst_kth(const BST *t, size_t k, int *out);  /* 第 k 小（k 从 1 开始） */

void bst_inorder(const BST *t, BSTVisitFn visit, void *ctx);
int bst_height(const BST *t);
size_t bst_size(const BST *t);
int bst_is_valid(const BST *t); /* 检查 BST 性质，用于测试 */

#endif
