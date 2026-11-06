#include "heap.h"

#include <stdlib.h>
#include <string.h>

/* a 是否应该排在 b 的上面 */
static int higher(HeapKind kind, int a, int b)
{
    return kind == MIN_HEAP ? a < b : a > b;
}

static void swap_int(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

static void sift_up(int *d, size_t i, HeapKind kind)
{
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (!higher(kind, d[i], d[parent]))
            break;
        swap_int(&d[i], &d[parent]);
        i = parent;
    }
}

static void sift_down(int *d, size_t n, size_t i, HeapKind kind)
{
    for (;;) {
        size_t best = i, l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && higher(kind, d[l], d[best]))
            best = l;
        if (r < n && higher(kind, d[r], d[best]))
            best = r;
        if (best == i)
            break;
        swap_int(&d[i], &d[best]);
        i = best;
    }
}

static int reserve(Heap *h, size_t need)
{
    if (need <= h->capacity)
        return 0;
    size_t cap = h->capacity ? h->capacity : 4;
    while (cap < need)
        cap *= 2;
    int *p = (int *)realloc(h->data, cap * sizeof(int));
    if (!p)
        return -1;
    h->data = p;
    h->capacity = cap;
    return 0;
}

Heap *heap_create(size_t init_capacity, HeapKind kind)
{
    Heap *h = (Heap *)malloc(sizeof(Heap));
    if (!h)
        return NULL;
    h->data = NULL;
    h->size = h->capacity = 0;
    h->kind = kind;
    if (reserve(h, init_capacity ? init_capacity : 4) != 0) {
        free(h);
        return NULL;
    }
    return h;
}

void heap_destroy(Heap *h)
{
    if (!h)
        return;
    free(h->data);
    free(h);
}

int heap_push(Heap *h, int val)
{
    if (reserve(h, h->size + 1) != 0)
        return -1;
    h->data[h->size] = val;
    sift_up(h->data, h->size, h->kind);
    h->size++;
    return 0;
}

int heap_pop(Heap *h, int *out)
{
    if (h->size == 0)
        return -1;
    if (out)
        *out = h->data[0];
    h->data[0] = h->data[--h->size]; /* 末尾元素补到堆顶再下沉 */
    sift_down(h->data, h->size, 0, h->kind);
    return 0;
}

int heap_top(const Heap *h, int *out)
{
    if (h->size == 0)
        return -1;
    *out = h->data[0];
    return 0;
}

size_t heap_size(const Heap *h)
{
    return h->size;
}

int heap_empty(const Heap *h)
{
    return h->size == 0;
}

int heap_build(Heap *h, const int *vals, size_t n)
{
    if (reserve(h, n) != 0)
        return -1;
    if (n)
        memcpy(h->data, vals, n * sizeof(int));
    h->size = n;
    /* 从最后一个非叶子结点开始逐个下沉 */
    for (size_t i = n / 2; i-- > 0;)
        sift_down(h->data, n, i, h->kind);
    return 0;
}

int heap_is_valid(const Heap *h)
{
    for (size_t i = 1; i < h->size; ++i)
        if (higher(h->kind, h->data[i], h->data[(i - 1) / 2]))
            return 0;
    return 1;
}

void heap_sort(int *a, size_t n)
{
    if (n < 2)
        return;
    /* 升序用大顶堆：每次把最大值换到末尾，再缩小堆的范围 */
    for (size_t i = n / 2; i-- > 0;)
        sift_down(a, n, i, MAX_HEAP);
    for (size_t end = n - 1; end > 0; --end) {
        swap_int(&a[0], &a[end]);
        sift_down(a, end, 0, MAX_HEAP);
    }
}

size_t heap_top_k(const int *a, size_t n, size_t k, int *out)
{
    if (k == 0)
        return 0;
    if (k > n)
        k = n;
    Heap *h = heap_create(k, MIN_HEAP); /* 堆顶是"当前前 k 大"里最小的 */
    if (!h)
        return 0;
    for (size_t i = 0; i < n; ++i) {
        if (h->size < k) {
            heap_push(h, a[i]);
        } else if (a[i] > h->data[0]) {
            h->data[0] = a[i];
            sift_down(h->data, h->size, 0, MIN_HEAP);
        }
    }
    size_t cnt = h->size;
    for (size_t i = cnt; i-- > 0;)
        heap_pop(h, &out[i]); /* 小顶堆依次弹出是升序，倒着写就是降序 */
    heap_destroy(h);
    return cnt;
}
