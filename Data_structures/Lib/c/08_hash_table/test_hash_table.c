#include "check.h"
#include "hash_chain.h"
#include "hash_open.h"

static void test_chain(void)
{
    TEST_SECTION("chaining");
    HashChain *h = hc_create(4);
    for (int i = 0; i < 1000; ++i)
        CHECK(hc_put(h, i * 7, i) == 1);
    CHECK(hc_size(h) == 1000);
    CHECK(hc_bucket_count(h) > 1000); /* 发生过多次 rehash */
    int v;
    CHECK(hc_get(h, 7 * 123, &v) == 1 && v == 123);
    CHECK(hc_get(h, 5, &v) == 0);
    CHECK(hc_put(h, 0, -1) == 0);
    CHECK(hc_get(h, 0, &v) == 1 && v == -1);
    for (int i = 0; i < 1000; i += 2)
        CHECK(hc_remove(h, i * 7) == 1);
    CHECK(hc_remove(h, 0) == 0);
    CHECK(hc_size(h) == 500);
    CHECK(hc_get(h, 7, &v) == 1 && v == 1);
    CHECK(hc_get(h, 14, &v) == 0);
    CHECK(hc_put(h, -42, 42) == 1 && hc_get(h, -42, &v) == 1 && v == 42);
    hc_destroy(h);
}

static void test_open(void)
{
    TEST_SECTION("open addressing");
    HashOpen *h = ho_create(8);
    for (int i = 0; i < 1000; ++i)
        CHECK(ho_put(h, i * 7, i) == 1);
    CHECK(ho_size(h) == 1000);
    int v;
    CHECK(ho_get(h, 7 * 999, &v) == 1 && v == 999);
    CHECK(ho_put(h, 7, 100) == 0 && ho_get(h, 7, &v) == 1 && v == 100);

    TEST_SECTION("tombstones");
    for (int i = 0; i < 1000; i += 2)
        CHECK(ho_remove(h, i * 7) == 1);
    CHECK(ho_size(h) == 500);
    /* 删除之后，探测链上靠后的键必须依然能找到 */
    for (int i = 1; i < 1000; i += 2)
        CHECK(ho_get(h, i * 7, NULL) == 1);
    CHECK(ho_get(h, 14, NULL) == 0);

    /* 反复插入删除，墓碑应被回收，容量不会无限增长 */
    size_t cap = ho_capacity(h);
    for (int round = 0; round < 50; ++round) {
        for (int i = 0; i < 100; ++i)
            ho_put(h, 100000 + i, i);
        for (int i = 0; i < 100; ++i)
            ho_remove(h, 100000 + i);
    }
    CHECK(ho_size(h) == 500);
    CHECK(ho_capacity(h) <= cap * 2);
    ho_destroy(h);
}

int main(void)
{
    test_chain();
    test_open();
    return CHECK_REPORT();
}
