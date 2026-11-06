#include <vector>

#include "Greedy.h"
#include "check.h"

using namespace greedy;

int main()
{
    TEST_SECTION("intervals");
    CHECK(max_non_overlapping({{1, 4}, {3, 5}, {0, 6}, {5, 7}, {3, 9}, {5, 9}, {6, 10}, {8, 11}, {8, 12}, {2, 14}, {12, 16}}) == 4);
    CHECK(max_non_overlapping({}) == 0);
    CHECK(merge_intervals({{1, 3}, {8, 10}, {2, 6}, {15, 18}}) == (std::vector<Interval>{{1, 6}, {8, 10}, {15, 18}}));
    CHECK(merge_intervals({{1, 4}, {4, 5}}) == (std::vector<Interval>{{1, 5}}));

    TEST_SECTION("jump game");
    CHECK(can_jump({2, 3, 1, 1, 4}));
    CHECK(!can_jump({3, 2, 1, 0, 4}));
    CHECK(min_jumps({2, 3, 1, 1, 4}) == 2);
    CHECK(min_jumps({0}) == 0);

    TEST_SECTION("huffman / cookies / gas");
    CHECK(huffman_cost({5, 9, 12, 13, 16, 45}) == 224);
    CHECK(huffman_cost({7}) == 0);
    CHECK(assign_cookies({1, 2, 3}, {1, 1}) == 1);
    CHECK(assign_cookies({1, 2}, {1, 2, 3}) == 2);
    CHECK(gas_station({1, 2, 3, 4, 5}, {3, 4, 5, 1, 2}) == 3);
    CHECK(gas_station({2, 3, 4}, {3, 4, 3}) == -1);

    TEST_SECTION("coins (greedy works only for canonical systems)");
    CHECK(coin_change_greedy({1, 5, 10, 20, 50, 100}, 188) == 8); // 100+50+20+10+5+1+1+1
    CHECK(coin_change_greedy({1, 3, 4}, 6) == 3);                  // 贪心 4+1+1，但最优是 3+3
    CHECK(coin_change_greedy({5, 10}, 3) == -1);

    TEST_SECTION("remove k digits");
    CHECK(remove_k_digits("1432219", 3) == "1219");
    CHECK(remove_k_digits("10200", 1) == "200");
    CHECK(remove_k_digits("10", 2) == "0");

    return CHECK_REPORT();
}
