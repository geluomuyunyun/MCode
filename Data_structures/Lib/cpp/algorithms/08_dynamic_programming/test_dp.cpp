#include <string>
#include <vector>

#include "DP.h"
#include "check.h"

using namespace dp;

static std::vector<std::vector<int>> make_tree(int n, const std::vector<std::pair<int, int>>& edges)
{
    std::vector<std::vector<int>> adj(n);
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return adj;
}

int main()
{
    TEST_SECTION("linear DP");
    CHECK(fibonacci(0) == 0 && fibonacci(1) == 1 && fibonacci(50) == 12586269025LL);
    CHECK(climb_stairs(1) == 1 && climb_stairs(3) == 3 && climb_stairs(10) == 89);
    CHECK(max_subarray_kadane({-2, 1, -3, 4, -1, 2, 1, -5, 4}) == 6);
    CHECK(house_robber({2, 7, 9, 3, 1}) == 12);
    CHECK(house_robber({}) == 0);
    CHECK(unique_paths(3, 7) == 28);
    CHECK(min_path_sum({{1, 3, 1}, {1, 5, 1}, {4, 2, 1}}) == 7);

    TEST_SECTION("knapsack");
    CHECK(knapsack_01({1, 3, 4}, {15, 20, 30}, 4) == 35);
    CHECK(knapsack_01({2, 3, 4, 5}, {3, 4, 5, 6}, 5) == 7);
    CHECK(knapsack_complete({1, 3, 4}, {15, 20, 30}, 4) == 60);
    CHECK(coin_change({1, 2, 5}, 11) == 3);
    CHECK(coin_change({2}, 3) == -1);
    CHECK(coin_change({1, 3, 4}, 6) == 2); // 贪心会得到 3，DP 得到最优的 2
    CHECK(coin_change({1}, 0) == 0);
    CHECK(coin_change_ways({1, 2, 5}, 5) == 4);

    TEST_SECTION("sequence DP");
    CHECK(lcs("ABCBDAB", "BDCABA").size() == 4);
    CHECK(lcs("abcde", "ace") == "ace");
    CHECK(lcs("abc", "xyz").empty());
    CHECK(lis_length({10, 9, 2, 5, 3, 7, 101, 18}) == 4);
    CHECK(lis_length({7, 7, 7}) == 1);
    CHECK(edit_distance("horse", "ros") == 3);
    CHECK(edit_distance("intention", "execution") == 5);
    CHECK(edit_distance("", "abc") == 3);
    CHECK(longest_palindromic_subsequence("bbbab") == 4);
    CHECK(longest_palindromic_subsequence("cbbd") == 2);

    TEST_SECTION("interval DP");
    CHECK(merge_stones({1, 3, 5, 2}) == 22);
    CHECK(merge_stones({4, 1, 1, 4}) == 18);
    CHECK(matrix_chain({10, 30, 5, 60}) == 4500);
    CHECK(matrix_chain({40, 20, 30, 10, 30}) == 26000);

    TEST_SECTION("tree DP");
    //        0(1)
    //       /    \
    //     1(2)   2(3)
    //     / \      \
    //   3(4) 4(5)  5(6)
    auto adj = make_tree(6, {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}});
    CHECK(tree_max_independent_set(adj, {1, 2, 3, 4, 5, 6}, 0) == 4 + 5 + 6 + 1);
    CHECK(tree_max_independent_set(adj, {1, 2, 3, 4, 5, 6}, 3) == 16); // 换个根，答案不变
    CHECK(tree_diameter(adj) == 4); // 3-1-0-2-5
    auto path = make_tree(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}});
    CHECK(tree_diameter(path) == 4);
    CHECK(tree_max_independent_set(path, {1, 1, 1, 1, 1}, 0) == 3);

    return CHECK_REPORT();
}
