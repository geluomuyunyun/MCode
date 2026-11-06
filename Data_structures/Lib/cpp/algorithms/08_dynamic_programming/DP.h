// DP.h - 动态规划。
//
// 解题四步：
//   1. 定义状态：dp[i] / dp[i][j] 表示什么
//   2. 状态转移方程：dp[i] 如何由更小的状态得到
//   3. 初始值与边界
//   4. 计算顺序：保证用到的状态已经算好
// 能用 DP 的前提：最优子结构 + 重叠子问题。
// 本模块按难度递进：线性 DP -> 背包 -> 序列 DP -> 区间 DP -> 树形 DP。
#pragma once

#include <string>
#include <vector>

namespace dp {

// ---------- 入门：线性 DP ----------
long long fibonacci(int n);                        // 自底向上 + 滚动变量
long long climb_stairs(int n);                     // 每次 1 或 2 阶
long long max_subarray_kadane(const std::vector<int>& a);
int house_robber(const std::vector<int>& a);       // 不能偷相邻的房子
long long unique_paths(int m, int n);              // 网格从左上到右下的路径数
int min_path_sum(const std::vector<std::vector<int>>& grid);

// ---------- 背包 ----------
// 0-1 背包：每件物品最多选一次，返回容量 W 内的最大价值
int knapsack_01(const std::vector<int>& weight, const std::vector<int>& value, int W);
// 完全背包：每件物品可选无限次
int knapsack_complete(const std::vector<int>& weight, const std::vector<int>& value, int W);
// 零钱兑换：最少硬币数，凑不出返回 -1（任意面值都正确，对比贪心）
int coin_change(const std::vector<int>& coins, int amount);
// 零钱兑换 II：凑出 amount 的组合数
long long coin_change_ways(const std::vector<int>& coins, int amount);

// ---------- 序列 DP ----------
std::string lcs(const std::string& a, const std::string& b); // 最长公共子序列（返回一个解）
int lis_length(const std::vector<int>& a);                    // 最长严格上升子序列，O(n log n)
int edit_distance(const std::string& a, const std::string& b);
int longest_palindromic_subsequence(const std::string& s);

// ---------- 区间 DP ----------
// 石子合并：每次合并相邻两堆，代价为两堆之和，求合并成一堆的最小总代价
long long merge_stones(const std::vector<int>& piles);
// 矩阵链乘法：dims 长度为 n+1，第 i 个矩阵是 dims[i] x dims[i+1]，求最少标量乘法次数
long long matrix_chain(const std::vector<int>& dims);

// ---------- 树形 DP ----------
// 树上最大权独立集（"没有上司的舞会"）：选中的结点两两不相邻，求最大权值和。
// adj 为无向树的邻接表，root 为任一结点
long long tree_max_independent_set(const std::vector<std::vector<int>>& adj, const std::vector<int>& weight,
                                   int root);
// 树的直径（边数）
int tree_diameter(const std::vector<std::vector<int>>& adj);

} // namespace dp
