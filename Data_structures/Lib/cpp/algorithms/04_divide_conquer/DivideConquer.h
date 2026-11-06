// DivideConquer.h - 分治：分解成子问题 -> 递归求解 -> 合并结果。
//
// 复杂度用主定理分析，例如 T(n) = 2T(n/2) + O(n) => O(n log n)。
#pragma once

#include <utility>
#include <vector>

namespace dc {

// 快速幂：a^b mod m，O(log b)。m 需小于约 3e9，否则中间乘积会溢出 long long
long long fast_pow(long long a, long long b, long long m);

// 最大子数组和（分治版 O(n log n)，与 dp 版的 Kadane 算法对照）
long long max_subarray(const std::vector<int>& a);

// 逆序对个数：归并排序的副产品
long long count_inversions(std::vector<int> a);

// 快速选择：第 k 小（k 从 0 开始），平均 O(n)
int quick_select(std::vector<int> a, int k);

// 平面最近点对的距离平方，O(n log n)
long long closest_pair_dist2(std::vector<std::pair<long long, long long>> pts);

// 矩阵快速幂求斐波那契 F(n) mod m，O(log n)。m 的限制同 fast_pow
long long fibonacci_mod(long long n, long long m);

} // namespace dc
