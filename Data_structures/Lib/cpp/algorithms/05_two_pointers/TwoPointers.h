// TwoPointers.h - 双指针。
//
// 两种常见形态：
//   - 对撞指针：left 从头、right 从尾向中间靠拢（要求有序或具有对称性）
//   - 快慢指针：slow 指向"结果区"的末尾，fast 扫描整个数组（原地修改数组）
// 把 O(n^2) 的两重循环降到 O(n)，关键是每一步都能安全地排除一个指针位置。
#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>

namespace two_pointers {

// 有序数组中和为 target 的两个数的下标，不存在返回 {-1, -1}
std::pair<int, int> two_sum_sorted(const std::vector<int>& a, int target);
// 所有和为 0 的不重复三元组
std::vector<std::array<int, 3>> three_sum(std::vector<int> a);
// 盛最多水的容器
long long max_area(const std::vector<int>& height);
// 接雨水
long long trap_rain_water(const std::vector<int>& height);
// 有序数组原地去重，返回新长度
int remove_duplicates(std::vector<int>& a);
// 原地把所有 0 移到末尾，保持非零元素相对顺序
void move_zeroes(std::vector<int>& a);
// 只考虑字母和数字、忽略大小写的回文判断
bool is_palindrome(const std::string& s);
// 合并两个有序数组
std::vector<int> merge_sorted(const std::vector<int>& a, const std::vector<int>& b);
// 荷兰国旗问题：只含 0/1/2 的数组原地排序，一趟扫描
void sort_colors(std::vector<int>& a);

} // namespace two_pointers
