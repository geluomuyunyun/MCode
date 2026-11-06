// SlidingWindow.h - 滑动窗口（双指针的特例：两个指针同向移动，维护区间 [left, right]）。
//
// 模板：
//   for (right = 0; right < n; ++right) {
//       把 s[right] 加入窗口;
//       while (窗口不满足条件) { 把 s[left] 移出窗口; ++left; }
//       用当前窗口更新答案;
//   }
// 每个元素最多进出窗口各一次，总体 O(n)。
#pragma once

#include <string>
#include <vector>

namespace window {

// 无重复字符的最长子串长度
int longest_unique_substring(const std::string& s);
// 和 >= target 的最短连续子数组长度（元素为正），不存在返回 0
int min_subarray_len(int target, const std::vector<int>& a);
// 每个长度为 k 的窗口的最大值（单调队列）
std::vector<int> max_sliding_window(const std::vector<int>& a, int k);
// s 中所有 p 的字母异位词的起始下标（固定长度窗口）
std::vector<int> find_anagrams(const std::string& s, const std::string& p);
// 覆盖 t 所有字符（含重复）的最小子串，不存在返回空串
std::string min_window(const std::string& s, const std::string& t);
// 最多包含 k 种不同字符的最长子串长度
int longest_with_k_distinct(const std::string& s, int k);

} // namespace window
