/*
 * heap.h - 二叉堆 / 优先队列（数组实现的完全二叉树）。
 *
 * 下标关系（从 0 开始）：父 (i-1)/2，左孩子 2i+1，右孩子 2i+2。
 * 核心操作：上浮 sift_up（插入后）、下沉 sift_down（删除堆顶后）。
 *
 * 附带算法：
 *   - heap_build：Floyd 建堆，O(n)
 *   - heap_sort：原地堆排序，O(n log n)
 *   - heap_top_k：用大小为 k 的小顶堆求前 k 大，O(n log k)
 */
#ifndef DSA_C_HEAP_H
#define DSA_C_HEAP_H

#include <stddef.h>

typedef enum { MIN_HEAP, MAX_HEAP } HeapKind;

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
    HeapKind kind;
} Heap;

Heap *heap_create(size_t init_capacity, HeapKind kind);
void heap_destroy(Heap *h);
int heap_push(Heap *h, int val);
int heap_pop(Heap *h, int *out);
int heap_top(const Heap *h, int *out);
size_t heap_size(const Heap *h);
int heap_empty(const Heap *h);
int heap_build(Heap *h, const int *vals, size_t n); /* 用 vals 替换堆中原有内容 */
int heap_is_valid(const Heap *h);

void heap_sort(int *a, size_t n); /* 升序 */
/* 把 a 中最大的 k 个数写入 out（降序），返回实际写入个数 */
size_t heap_top_k(const int *a, size_t n, size_t k, int *out);

#endif
