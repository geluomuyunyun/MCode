#include <string>
#include <vector>

#include "SlidingWindow.h"
#include "check.h"

using namespace window;
using Ints = std::vector<int>;

int main()
{
    TEST_SECTION("variable-size window");
    CHECK(longest_unique_substring("abcabcbb") == 3);
    CHECK(longest_unique_substring("bbbbb") == 1);
    CHECK(longest_unique_substring("pwwkew") == 3);
    CHECK(longest_unique_substring("abba") == 2);
    CHECK(longest_unique_substring("") == 0);
    CHECK(min_subarray_len(7, {2, 3, 1, 2, 4, 3}) == 2);
    CHECK(min_subarray_len(100, {1, 2, 3}) == 0);
    CHECK(longest_with_k_distinct("eceba", 2) == 3);
    CHECK(longest_with_k_distinct("aa", 1) == 2);

    TEST_SECTION("monotonic deque");
    CHECK(max_sliding_window({1, 3, -1, -3, 5, 3, 6, 7}, 3) == (Ints{3, 3, 5, 5, 6, 7}));
    CHECK(max_sliding_window({9, 8, 7}, 1) == (Ints{9, 8, 7}));
    CHECK(max_sliding_window({1, 2}, 5).empty());

    TEST_SECTION("fixed-size window");
    CHECK(find_anagrams("cbaebabacd", "abc") == (Ints{0, 6}));
    CHECK(find_anagrams("abab", "ab") == (Ints{0, 1, 2}));

    TEST_SECTION("minimum window substring");
    CHECK(min_window("ADOBECODEBANC", "ABC") == "BANC");
    CHECK(min_window("a", "aa").empty());
    CHECK(min_window("aa", "aa") == "aa");

    return CHECK_REPORT();
}
