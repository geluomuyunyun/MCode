#include "bst.h"
#include "check.h"

typedef struct {
    int buf[64];
    size_t n;
} Collector;

static void collect(int key, void *ctx)
{
    Collector *c = (Collector *)ctx;
    c->buf[c->n++] = key;
}

static int is_sorted(const Collector *c)
{
    for (size_t i = 1; i < c->n; ++i)
        if (c->buf[i - 1] >= c->buf[i])
            return 0;
    return 1;
}

int main(void)
{
    BST *t = bst_create();
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    size_t nkeys = sizeof keys / sizeof keys[0];

    TEST_SECTION("insert / contains");
    for (size_t i = 0; i < nkeys; ++i)
        CHECK(bst_insert(t, keys[i]) == 1);
    CHECK(bst_insert(t, 40) == 0);
    CHECK(bst_size(t) == nkeys);
    CHECK(bst_contains(t, 65));
    CHECK(!bst_contains(t, 66));
    CHECK(bst_is_valid(t));
    CHECK(bst_height(t) == 4);

    TEST_SECTION("min / max / floor / ceil / kth");
    int v;
    CHECK(bst_min(t, &v) == 0 && v == 20);
    CHECK(bst_max(t, &v) == 0 && v == 80);
    CHECK(bst_floor(t, 64, &v) == 0 && v == 60);
    CHECK(bst_floor(t, 65, &v) == 0 && v == 65);
    CHECK(bst_floor(t, 10, &v) == -1);
    CHECK(bst_ceil(t, 41, &v) == 0 && v == 45);
    CHECK(bst_ceil(t, 81, &v) == -1);
    CHECK(bst_kth(t, 1, &v) == 0 && v == 20);
    CHECK(bst_kth(t, 5, &v) == 0 && v == 45);
    CHECK(bst_kth(t, 11, &v) == -1);

    TEST_SECTION("inorder is sorted");
    Collector c = {{0}, 0};
    bst_inorder(t, collect, &c);
    CHECK(c.n == nkeys && is_sorted(&c));

    TEST_SECTION("remove: leaf / one child / two children / root");
    CHECK(bst_remove(t, 30) == 1); /* 两个孩子 20 和 40 */
    CHECK(bst_remove(t, 20) == 1); /* 叶子 */
    CHECK(bst_remove(t, 60) == 1); /* 只有右孩子 65 */
    CHECK(bst_remove(t, 50) == 1); /* 根 */
    CHECK(bst_remove(t, 999) == 0);
    CHECK(bst_size(t) == nkeys - 4);
    CHECK(bst_is_valid(t));
    CHECK(!bst_contains(t, 30) && bst_contains(t, 35) && bst_contains(t, 65));
    c.n = 0;
    bst_inorder(t, collect, &c);
    CHECK(c.n == nkeys - 4 && is_sorted(&c));

    TEST_SECTION("degenerate (sorted insert)");
    BST *chain = bst_create();
    for (int i = 0; i < 20; ++i)
        bst_insert(chain, i);
    CHECK(bst_height(chain) == 20); /* 退化成链表，这就是需要平衡树的原因 */
    for (int i = 0; i < 20; ++i)
        CHECK(bst_remove(chain, i) == 1);
    CHECK(bst_size(chain) == 0 && chain->root == NULL);
    bst_destroy(chain);

    bst_destroy(t);
    return CHECK_REPORT();
}
