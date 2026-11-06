#include "check.h"
#include "dynamic_array.h"

int main(void)
{
    TEST_SECTION("push / get / grow");
    DynArray *a = da_create(2);
    CHECK(a != NULL);
    for (int i = 0; i < 100; ++i)
        CHECK(da_push_back(a, i) == 0);
    CHECK(da_size(a) == 100);
    CHECK(da_capacity(a) >= 100);
    int v = -1;
    CHECK(da_get(a, 42, &v) == 0 && v == 42);
    CHECK(da_get(a, 100, &v) == -1);

    TEST_SECTION("insert / erase / find");
    CHECK(da_insert(a, 0, -1) == 0);
    CHECK(da_get(a, 0, &v) == 0 && v == -1);
    CHECK(da_insert(a, 101, 999) == 0); /* 插在末尾 */
    CHECK(da_insert(a, 1000, 1) == -1);
    CHECK(da_find(a, 999) == 101);
    CHECK(da_erase(a, 0) == 0);
    CHECK(da_find(a, -1) == -1);
    CHECK(da_set(a, 0, 7) == 0 && da_get(a, 0, &v) == 0 && v == 7);

    TEST_SECTION("pop / shrink");
    while (da_size(a) > 3)
        CHECK(da_pop_back(a, &v) == 0);
    CHECK(da_capacity(a) < 100);
    da_print(a);

    TEST_SECTION("reverse / clear");
    da_reverse(a); /* [7,1,2] -> [2,1,7] */
    CHECK(da_get(a, 0, &v) == 0 && v == 2);
    CHECK(da_get(a, 2, &v) == 0 && v == 7);
    da_clear(a);
    CHECK(da_size(a) == 0);
    CHECK(da_pop_back(a, &v) == -1);

    da_destroy(a);
    return CHECK_REPORT();
}
