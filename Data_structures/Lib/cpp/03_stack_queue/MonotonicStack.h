// MonotonicStack.h - 单调栈的两个经典应用（普通函数，声明在这里，实现在 .cpp）。
//
// 单调栈：栈内元素保持单调，新元素入栈前把破坏单调性的元素全部弹出。
// 每个元素最多入栈、出栈一次，所以总体 O(n)。
#pragma once

#include <vector>

namespace mono {

// 对每个位置 i，求右边第一个比 a[i] 大的元素的下标，不存在为 -1
std::vector<int> next_greater_index(const std::vector<int>& a);

// 柱状图中最大矩形面积（LeetCode 84）
long long largest_rectangle(const std::vector<int>& heights);

} // namespace mono
