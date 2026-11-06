#include <stdlib.h>

#include "check.h"
#include "heap.h"

static int is_sorted(const int *a, size_t n)
{
    for (size_t i = 1; i < n; ++i)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

int main(void)
{
    int vals[] = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
    size_t n = sizeof vals / sizeof vals[0];
    int v;

    TEST_SECTION("min heap push / pop");
    Heap *h = heap_create(2, MIN_HEAP);
    for (size_t i = 0; i < n; ++i)
        heap_push(h, vals[i]);
    CHECK(heap_size(h) == n && heap_is_valid(h));
    CHECK(heap_top(h, &v) == 0 && v == 0);
    for (int i = 0; i < 10; ++i)
        CHECK(heap_pop(h, &v) == 0 && v == i);
    CHECK(heap_empty(h) && heap_pop(h, &v) == -1);
    heap_destroy(h);

    TEST_SECTION("max heap build (Floyd)");
    h = heap_create(0, MAX_HEAP);
    CHECK(heap_build(h, vals, n) == 0);
    CHECK(heap_is_valid(h));
    for (int i = 9; i >= 0; --i)
        CHECK(heap_pop(h, &v) == 0 && v == i);
    heap_destroy(h);

    TEST_SECTION("heap sort");
    int a[] = {5, -3, 8, 8, 1, 0, -10, 42, 7, 7, 3};
    size_t an = sizeof a / sizeof a[0];
    heap_sort(a, an);
    CHECK(is_sorted(a, an));

    int big[1000];
    srand(12345);
    for (int i = 0; i < 1000; ++i)
        big[i] = rand() % 500 - 250;
    heap_sort(big, 1000);
    CHECK(is_sorted(big, 1000));

    TEST_SECTION("top k");
    int top[3];
    CHECK(heap_top_k(vals, n, 3, top) == 3);
    CHECK(top[0] == 9 && top[1] == 8 && top[2] == 7);
    int top_all[20];
    CHECK(heap_top_k(vals, n, 20, top_all) == n);

    return CHECK_REPORT();
}
