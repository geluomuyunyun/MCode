// Search.h - 二分查找及其变体。
//
// 统一使用左闭右开区间 [lo, hi)，循环条件 lo < hi，这样最不容易写错：
//   lower_bound：第一个 >= target 的位置
//   upper_bound：第一个 >  target 的位置
// 其余变体都可以由这两个函数组合出来。
// "二分答案"：答案具有单调性时（如 x*x <= n），也可以对答案本身二分。
#pragma once

#include <utility>
#include <vector>

namespace search {

int linear_search(const std::vector<int>& a, int target);  // 找不到返回 -1
int binary_search(const std::vector<int>& a, int target);  // 有序数组，找不到返回 -1
int lower_bound(const std::vector<int>& a, int target);
int upper_bound(const std::vector<int>& a, int target);
std::pair<int, int> equal_range(const std::vector<int>& a, int target); // 首尾下标，找不到 {-1,-1}

int search_rotated(const std::vector<int>& a, int target); // 旋转后的有序数组（无重复）
int find_min_rotated(const std::vector<int>& a);           // 旋转数组的最小值下标
int find_peak(const std::vector<int>& a);                  // 任一峰值下标（a[i] 大于左右邻居）
int int_sqrt(long long n);                                 // 下取整平方根（二分答案）
// 把数组切成最多 k 段连续子数组，使最大段和最小，返回这个最小值（二分答案）
long long split_array_min_largest(const std::vector<int>& a, int k);

} // namespace search
