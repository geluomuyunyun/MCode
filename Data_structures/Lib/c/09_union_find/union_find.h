/*
 * union_find.h - 并查集（不相交集合），元素编号 0..n-1。
 *
 * 两个优化：
 *   - 路径压缩：find 时把路径上的结点直接挂到根上
 *   - 按秩合并：矮树挂到高树下面
 * 两者结合后单次操作均摊接近 O(1)（反阿克曼函数）。
 */
#ifndef DSA_C_UNION_FIND_H
#define DSA_C_UNION_FIND_H

typedef struct {
    int *parent;
    int *rank; /* 树高的上界 */
    int *size; /* 只有根结点的 size 有意义 */
    int n;
    int count; /* 连通分量个数 */
} UnionFind;

UnionFind *uf_create(int n);
void uf_destroy(UnionFind *uf);
int uf_find(UnionFind *uf, int x);
int uf_unite(UnionFind *uf, int a, int b); /* 原本不连通、发生合并返回 1 */
int uf_connected(UnionFind *uf, int a, int b);
int uf_count(const UnionFind *uf);
int uf_component_size(UnionFind *uf, int x);

#endif
