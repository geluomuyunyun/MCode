#include <array>
#include <vector>

#include "TwoPointers.h"
#include "check.h"

using namespace two_pointers;
using Ints = std::vector<int>;

int main()
{
    TEST_SECTION("two sum (sorted) / three sum");
    CHECK(two_sum_sorted({2, 7, 11, 15}, 9) == std::make_pair(0, 1));
    CHECK(two_sum_sorted({1, 3, 4, 6, 9}, 13) == std::make_pair(2, 4));
    CHECK(two_sum_sorted({1, 2}, 10) == std::make_pair(-1, -1));
    auto ts = three_sum({-1, 0, 1, 2, -1, -4});
    CHECK(ts == (std::vector<std::array<int, 3>>{{-1, -1, 2}, {-1, 0, 1}}));
    CHECK(three_sum({0, 0, 0, 0}).size() == 1);

    TEST_SECTION("container / rain water");
    CHECK(max_area({1, 8, 6, 2, 5, 4, 8, 3, 7}) == 49);
    CHECK(trap_rain_water({0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}) == 6);
    CHECK(trap_rain_water({4, 2, 0, 3, 2, 5}) == 9);
    CHECK(trap_rain_water({}) == 0);

    TEST_SECTION("in-place fast / slow pointers");
    Ints d{0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    int len = remove_duplicates(d);
    CHECK(len == 5 && Ints(d.begin(), d.begin() + len) == (Ints{0, 1, 2, 3, 4}));
    Ints z{0, 1, 0, 3, 12};
    move_zeroes(z);
    CHECK(z == (Ints{1, 3, 12, 0, 0}));
    Ints colors{2, 0, 2, 1, 1, 0};
    sort_colors(colors);
    CHECK(colors == (Ints{0, 0, 1, 1, 2, 2}));

    TEST_SECTION("palindrome / merge");
    CHECK(is_palindrome("A man, a plan, a canal: Panama"));
    CHECK(!is_palindrome("race a car"));
    CHECK(is_palindrome("") && is_palindrome(".,"));
    CHECK(merge_sorted({1, 4, 7}, {2, 3, 8, 9}) == (Ints{1, 2, 3, 4, 7, 8, 9}));

    return CHECK_REPORT();
}
