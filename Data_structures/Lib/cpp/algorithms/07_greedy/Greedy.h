// Greedy.h - 贪心：每一步都做当前看来最好的选择，且永不回头。
//
// 贪心不一定正确，使用前要能证明（常用"交换论证"：
// 任何最优解都可以在不变差的前提下换成贪心的选择）。
// 反例：硬币面值 {1, 3, 4} 凑 6，贪心得 4+1+1（3 枚），最优是 3+3（2 枚），
// 这类问题要用动态规划（见 08_dynamic_programming 的 coin_change）。
#pragma once

#include <string>
#include <utility>
#include <vector>

namespace greedy {

using Interval = std::pair<int, int>; // [start, end)

// 活动选择：最多能选多少个互不重叠的区间（按结束时间排序）
int max_non_overlapping(std::vector<Interval> intervals);
// 合并所有重叠区间
std::vector<Interval> merge_intervals(std::vector<Interval> intervals);
// 跳跃游戏：a[i] 表示从 i 最多能跳几步，能否到达末尾
bool can_jump(const std::vector<int>& a);
// 跳跃游戏 II：到达末尾的最少跳跃次数（保证可达）
int min_jumps(const std::vector<int>& a);
// 哈夫曼编码：合并所有频率的最小总代价（即编码后的总比特数）
long long huffman_cost(const std::vector<long long>& freq);
// 分发饼干：每个孩子 g[i] 需要尺寸 >= g[i] 的饼干，最多满足几个孩子
int assign_cookies(std::vector<int> greed, std::vector<int> cookies);
// 加油站：能绕环路一圈的起点，不存在返回 -1
int gas_station(const std::vector<int>& gas, const std::vector<int>& cost);
// 面值规范的硬币系统（如人民币）下的最少硬币数，凑不出返回 -1
int coin_change_greedy(std::vector<int> coins, int amount);
// 移掉 k 位数字使剩下的数最小
std::string remove_k_digits(const std::string& num, int k);

} // namespace greedy
