#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "FenwickTree.h"
#include "SegmentTree.h"
#include "check.h"

static void test_segment_tree()
{
    TEST_SECTION("segment tree basic");
    SegmentTree st({5, 3, 8, 6, 1, 4});
    CHECK(st.range_sum(0, 5) == 27);
    CHECK(st.range_min(0, 5) == 1);
    CHECK(st.range_sum(1, 3) == 17 && st.range_min(1, 3) == 3);
    st.range_add(0, 2, 10); // 15 13 18 6 1 4
    CHECK(st.range_sum(0, 5) == 57);
    CHECK(st.range_min(0, 2) == 13);
    CHECK(st.range_sum(2, 3) == 24);
    st.point_set(4, 100);   // 15 13 18 6 100 4
    CHECK(st.range_min(3, 5) == 4 && st.range_sum(4, 4) == 100);

    TEST_SECTION("segment tree randomized vs brute force");
    std::mt19937 rng(3);
    const int n = 200;
    std::vector<long long> a(n);
    for (auto& x : a)
        x = static_cast<long long>(rng() % 1000) - 500;
    SegmentTree t(a);
    bool ok = true;
    for (int it = 0; it < 5000; ++it) {
        int l = static_cast<int>(rng() % n), r = static_cast<int>(rng() % n);
        if (l > r) std::swap(l, r);
        if (rng() % 2) {
            long long d = static_cast<long long>(rng() % 200) - 100;
            t.range_add(l, r, d);
            for (int i = l; i <= r; ++i) a[i] += d;
        } else {
            long long s = std::accumulate(a.begin() + l, a.begin() + r + 1, 0LL);
            long long m = *std::min_element(a.begin() + l, a.begin() + r + 1);
            if (t.range_sum(l, r) != s || t.range_min(l, r) != m) ok = false;
        }
    }
    CHECK(ok);
}

static void test_fenwick()
{
    TEST_SECTION("fenwick tree");
    FenwickTree f({1, 2, 3, 4, 5});
    CHECK(f.prefix_sum(4) == 15 && f.prefix_sum(-1) == 0);
    CHECK(f.range_sum(1, 3) == 9);
    f.add(2, 10); // 1 2 13 4 5
    CHECK(f.range_sum(2, 2) == 13 && f.prefix_sum(4) == 25);
    CHECK(f.lower_bound(1) == 0);
    CHECK(f.lower_bound(4) == 2);  // 前缀和 1,3,16,...
    CHECK(f.lower_bound(16) == 2);
    CHECK(f.lower_bound(17) == 3);
    CHECK(f.lower_bound(1000) == 5);

    FenwickTree g(8);
    for (int i = 0; i < 8; ++i)
        g.add(i, i);
    CHECK(g.prefix_sum(7) == 28 && g.range_sum(3, 5) == 12);

    TEST_SECTION("inversions");
    CHECK(FenwickTree::count_inversions({2, 4, 1, 3, 5}) == 3);
    CHECK(FenwickTree::count_inversions({5, 4, 3, 2, 1}) == 10);
    CHECK(FenwickTree::count_inversions({1, 1, 1}) == 0);
    CHECK(FenwickTree::count_inversions({}) == 0);
}

int main()
{
    test_segment_tree();
    test_fenwick();
    return CHECK_REPORT();
}
