#include <algorithm>
#include <climits>
#include <random>
#include <vector>

#include "DivideConquer.h"
#include "check.h"

int main()
{
    TEST_SECTION("fast power");
    CHECK(dc::fast_pow(2, 10, 1000000007) == 1024);
    CHECK(dc::fast_pow(3, 0, 7) == 1);
    CHECK(dc::fast_pow(2, 100, 1000000007) == 976371285);
    CHECK(dc::fast_pow(5, 3, 1) == 0);

    TEST_SECTION("max subarray");
    CHECK(dc::max_subarray({-2, 1, -3, 4, -1, 2, 1, -5, 4}) == 6);
    CHECK(dc::max_subarray({-3, -1, -2}) == -1);
    CHECK(dc::max_subarray({5}) == 5);

    TEST_SECTION("inversions");
    CHECK(dc::count_inversions({2, 4, 1, 3, 5}) == 3);
    CHECK(dc::count_inversions({5, 4, 3, 2, 1}) == 10);
    CHECK(dc::count_inversions({}) == 0);

    TEST_SECTION("quick select");
    std::vector<int> a{7, 10, 4, 3, 20, 15, 4};
    std::vector<int> sorted = a;
    std::sort(sorted.begin(), sorted.end());
    for (int k = 0; k < static_cast<int>(a.size()); ++k)
        CHECK(dc::quick_select(a, k) == sorted[k]);

    TEST_SECTION("closest pair vs brute force");
    std::mt19937 rng(8);
    bool ok = true;
    for (int round = 0; round < 20; ++round) {
        std::vector<std::pair<long long, long long>> pts(200);
        for (auto& p : pts)
            p = {static_cast<long long>(rng() % 10000), static_cast<long long>(rng() % 10000)};
        long long brute = LLONG_MAX;
        for (std::size_t i = 0; i < pts.size(); ++i)
            for (std::size_t j = i + 1; j < pts.size(); ++j) {
                long long dx = pts[i].first - pts[j].first, dy = pts[i].second - pts[j].second;
                brute = std::min(brute, dx * dx + dy * dy);
            }
        if (dc::closest_pair_dist2(pts) != brute)
            ok = false;
    }
    CHECK(ok);
    CHECK(dc::closest_pair_dist2({{0, 0}, {3, 4}}) == 25);

    TEST_SECTION("fibonacci by matrix power");
    CHECK(dc::fibonacci_mod(0, 1000) == 0 && dc::fibonacci_mod(1, 1000) == 1);
    CHECK(dc::fibonacci_mod(10, 1000000007) == 55);
    CHECK(dc::fibonacci_mod(50, 1000000007) == 12586269025LL % 1000000007);
    CHECK(dc::fibonacci_mod(1000000000000LL, 1000000007) >= 0); // n = 10^12 也只需约 40 次矩阵乘法

    return CHECK_REPORT();
}
