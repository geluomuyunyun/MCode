// Backtracking.h - 递归与回溯。
//
// 回溯模板：
//   void backtrack(路径, 选择列表) {
//       if (满足结束条件) { 记录结果; return; }
//       for (选择 : 选择列表) {
//           做选择;  backtrack(路径, 新的选择列表);  撤销选择;
//       }
//   }
// 剪枝：提前判断某个分支不可能产生答案，直接跳过。
#pragma once

#include <string>
#include <vector>

namespace backtracking {

long long factorial(int n);                 // 最简单的递归
void hanoi(int n, char from, char via, char to, std::vector<std::string>& moves); // 汉诺塔

std::vector<std::vector<int>> permutations(const std::vector<int>& nums);
std::vector<std::vector<int>> permutations_unique(std::vector<int> nums); // 含重复元素，结果去重
std::vector<std::vector<int>> subsets(const std::vector<int>& nums);
std::vector<std::vector<int>> combinations(int n, int k);                  // 从 1..n 中选 k 个
std::vector<std::vector<int>> combination_sum(std::vector<int> candidates, int target); // 元素可重复使用
std::vector<std::vector<std::string>> n_queens(int n);
int n_queens_count(int n);                                                 // 位运算加速版
bool solve_sudoku(std::vector<std::string>& board);                        // '.' 表示空格
std::vector<std::string> generate_parentheses(int n);                      // n 对括号的所有合法组合

} // namespace backtracking
