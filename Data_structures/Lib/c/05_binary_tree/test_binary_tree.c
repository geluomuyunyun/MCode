#include "binary_tree.h"
#include "check.h"

#define NUL (-999)

typedef struct {
    int buf[64];
    size_t n;
} Collector;

static void collect(int val, void *ctx)
{
    Collector *c = (Collector *)ctx;
    c->buf[c->n++] = val;
}

static int same_seq(const Collector *c, const int *expect, size_t n)
{
    if (c->n != n)
        return 0;
    for (size_t i = 0; i < n; ++i)
        if (c->buf[i] != expect[i])
            return 0;
    return 1;
}

int main(void)
{
    /*
     *         1
     *       /   \
     *      2     3
     *     / \     \
     *    4   5     6
     *       /
     *      7
     */
    int level[] = {1, 2, 3, 4, 5, NUL, 6, NUL, NUL, 7};
    TreeNode *root = bt_build_level(level, sizeof level / sizeof level[0], NUL);

    const int pre[] = {1, 2, 4, 5, 7, 3, 6};
    const int in[] = {4, 2, 7, 5, 1, 3, 6};
    const int post[] = {4, 7, 5, 2, 6, 3, 1};
    const int lvl[] = {1, 2, 3, 4, 5, 6, 7};
    Collector c;

    TEST_SECTION("recursive traversals");
    c.n = 0; bt_preorder(root, collect, &c);  CHECK(same_seq(&c, pre, 7));
    c.n = 0; bt_inorder(root, collect, &c);   CHECK(same_seq(&c, in, 7));
    c.n = 0; bt_postorder(root, collect, &c); CHECK(same_seq(&c, post, 7));

    TEST_SECTION("iterative traversals");
    c.n = 0; bt_preorder_iter(root, collect, &c);  CHECK(same_seq(&c, pre, 7));
    c.n = 0; bt_inorder_iter(root, collect, &c);   CHECK(same_seq(&c, in, 7));
    c.n = 0; bt_postorder_iter(root, collect, &c); CHECK(same_seq(&c, post, 7));
    c.n = 0; bt_levelorder(root, collect, &c);     CHECK(same_seq(&c, lvl, 7));

    TEST_SECTION("properties");
    CHECK(bt_height(root) == 4);
    CHECK(bt_count(root) == 7);
    CHECK(bt_leaf_count(root) == 3);

    TEST_SECTION("rebuild from preorder + inorder");
    TreeNode *rebuilt = bt_build_pre_in(pre, in, 7);
    CHECK(bt_is_same(root, rebuilt));

    TEST_SECTION("mirror");
    bt_mirror(rebuilt);
    CHECK(!bt_is_same(root, rebuilt));
    const int mirrored_in[] = {6, 3, 1, 5, 7, 2, 4};
    c.n = 0; bt_inorder(rebuilt, collect, &c); CHECK(same_seq(&c, mirrored_in, 7));
    bt_mirror(rebuilt);
    CHECK(bt_is_same(root, rebuilt));

    TEST_SECTION("empty tree");
    c.n = 0;
    bt_inorder_iter(NULL, collect, &c);
    bt_postorder_iter(NULL, collect, &c);
    CHECK(c.n == 0);
    CHECK(bt_height(NULL) == 0);

    bt_destroy(rebuilt);
    bt_destroy(root);
    return CHECK_REPORT();
}
