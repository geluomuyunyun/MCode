#include "check.h"
#include "union_find.h"

int main(void)
{
    TEST_SECTION("basic union / find");
    UnionFind *uf = uf_create(10);
    CHECK(uf_count(uf) == 10);
    CHECK(uf_unite(uf, 0, 1) == 1);
    CHECK(uf_unite(uf, 2, 3) == 1);
    CHECK(uf_unite(uf, 1, 3) == 1);
    CHECK(uf_unite(uf, 0, 2) == 0); /* 已经连通 */
    CHECK(uf_connected(uf, 0, 3));
    CHECK(!uf_connected(uf, 0, 4));
    CHECK(uf_count(uf) == 7);
    CHECK(uf_component_size(uf, 2) == 4);
    CHECK(uf_component_size(uf, 9) == 1);
    uf_destroy(uf);

    TEST_SECTION("long chain + path compression");
    int n = 100000;
    uf = uf_create(n);
    for (int i = 1; i < n; ++i)
        uf_unite(uf, i - 1, i);
    CHECK(uf_count(uf) == 1);
    CHECK(uf_connected(uf, 0, n - 1));
    CHECK(uf_component_size(uf, 12345) == n);
    /* 压缩后任意结点到根最多一步 */
    int root = uf_find(uf, 0);
    uf_find(uf, n - 1);
    CHECK(uf->parent[n - 1] == root);
    uf_destroy(uf);

    TEST_SECTION("counting islands in a grid");
    /* 1 表示陆地，4 邻接的陆地属于同一个岛 */
    const char *grid[] = {
        "11000",
        "11000",
        "00100",
        "00011",
    };
    int rows = 4, cols = 5, water = 0;
    uf = uf_create(rows * cols);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == '0') {
                ++water;
                continue;
            }
            if (r + 1 < rows && grid[r + 1][c] == '1')
                uf_unite(uf, r * cols + c, (r + 1) * cols + c);
            if (c + 1 < cols && grid[r][c + 1] == '1')
                uf_unite(uf, r * cols + c, r * cols + c + 1);
        }
    }
    CHECK(uf_count(uf) - water == 3);
    uf_destroy(uf);

    return CHECK_REPORT();
}
