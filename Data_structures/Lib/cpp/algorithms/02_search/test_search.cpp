#include <algorithm>
#include <cmath>
#include <vector>

#include "Search.h"
#include "check.h"

int main()
{
    const std::vector<int> a{1, 3, 3, 3, 5, 8, 13};

    TEST_SECTION("linear / binary search");
    CHECK(search::linear_search(a, 8) == 5);
    CHECK(search::linear_search(a, 4) == -1);
    CHECK(search::binary_search(a, 13) == 6);
    CHECK(search::binary_search(a, 1) == 0);
    CHECK(search::binary_search(a, 0) == -1 && search::binary_search(a, 14) == -1);
    CHECK(search::binary_search({}, 1) == -1);

    TEST_SECTION("lower / upper bound match the STL");
    bool ok = true;
    for (int t = -1; t <= 15; ++t) {
        if (search::lower_bound(a, t) != std::lower_bound(a.begin(), a.end(), t) - a.begin()) ok = false;
        if (search::upper_bound(a, t) != std::upper_bound(a.begin(), a.end(), t) - a.begin()) ok = false;
    }
    CHECK(ok);
    CHECK(search::equal_range(a, 3) == std::make_pair(1, 3));
    CHECK(search::equal_range(a, 4) == std::make_pair(-1, -1));

    TEST_SECTION("rotated array");
    const std::vector<int> r{15, 18, 22, 2, 5, 8, 11};
    for (int i = 0; i < static_cast<int>(r.size()); ++i)
        CHECK(search::search_rotated(r, r[i]) == i);
    CHECK(search::search_rotated(r, 3) == -1);
    CHECK(search::find_min_rotated(r) == 3);
    CHECK(search::find_min_rotated({1, 2, 3}) == 0);

    TEST_SECTION("peak");
    const std::vector<int> p{1, 3, 20, 4, 1, 0};
    CHECK(search::find_peak(p) == 2);
    CHECK(search::find_peak({5}) == 0);

    TEST_SECTION("binary search on the answer");
    CHECK(search::int_sqrt(0) == 0 && search::int_sqrt(1) == 1);
    CHECK(search::int_sqrt(15) == 3 && search::int_sqrt(16) == 4);
    CHECK(search::int_sqrt(2147395600LL) == 46340);
    CHECK(search::int_sqrt(1000000000000LL) == 1000000);
    CHECK(search::split_array_min_largest({7, 2, 5, 10, 8}, 2) == 18);
    CHECK(search::split_array_min_largest({1, 2, 3, 4, 5}, 1) == 15);
    CHECK(search::split_array_min_largest({1, 2, 3, 4, 5}, 5) == 5);

    return CHECK_REPORT();
}
