#include "union_find.h"

#include <stdlib.h>

UnionFind *uf_create(int n)
{
    if (n <= 0)
        return NULL;
    UnionFind *uf = (UnionFind *)malloc(sizeof(UnionFind));
    if (!uf)
        return NULL;
    uf->parent = (int *)malloc((size_t)n * sizeof(int));
    uf->rank = (int *)calloc((size_t)n, sizeof(int));
    uf->size = (int *)malloc((size_t)n * sizeof(int));
    if (!uf->parent || !uf->rank || !uf->size) {
        uf_destroy(uf);
        return NULL;
    }
    for (int i = 0; i < n; ++i) {
        uf->parent[i] = i;
        uf->size[i] = 1;
    }
    uf->n = n;
    uf->count = n;
    return uf;
}

void uf_destroy(UnionFind *uf)
{
    if (!uf)
        return;
    free(uf->parent);
    free(uf->rank);
    free(uf->size);
    free(uf);
}

int uf_find(UnionFind *uf, int x)
{
    /* 迭代两趟：先找根，再把路径上每个结点直接指向根（不会因递归过深而栈溢出） */
    int root = x;
    while (uf->parent[root] != root)
        root = uf->parent[root];
    while (uf->parent[x] != root) {
        int next = uf->parent[x];
        uf->parent[x] = root;
        x = next;
    }
    return root;
}

int uf_unite(UnionFind *uf, int a, int b)
{
    int ra = uf_find(uf, a), rb = uf_find(uf, b);
    if (ra == rb)
        return 0;
    if (uf->rank[ra] < uf->rank[rb]) {
        int t = ra;
        ra = rb;
        rb = t;
    }
    uf->parent[rb] = ra;
    uf->size[ra] += uf->size[rb];
    if (uf->rank[ra] == uf->rank[rb])
        uf->rank[ra]++;
    uf->count--;
    return 1;
}

int uf_connected(UnionFind *uf, int a, int b)
{
    return uf_find(uf, a) == uf_find(uf, b);
}

int uf_count(const UnionFind *uf)
{
    return uf->count;
}

int uf_component_size(UnionFind *uf, int x)
{
    return uf->size[uf_find(uf, x)];
}
