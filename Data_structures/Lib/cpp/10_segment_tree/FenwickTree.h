// FenwickTree.h - 树状数组（Binary Indexed Tree）：单点加 + 前缀和。
//
// 核心是 lowbit(i) = i & (-i)：tree_[i] 管理区间 (i - lowbit(i), i]。
//   更新：i += lowbit(i) 向上走     查询：i -= lowbit(i) 向下走
// 代码比线段树短得多，但只能处理"可减"的运算（如求和），不能直接求区间最小值。
// 对外接口下标从 0 开始，内部数组从 1 开始。
#pragma once

#include <vector>

class FenwickTree {
public:
    explicit FenwickTree(int n);
    explicit FenwickTree(const std::vector<long long>& a); // O(n) 建树

    int size() const { return n_; }
    void add(int i, long long delta);
    long long prefix_sum(int i) const; // a[0] + ... + a[i]，i = -1 时为 0
    long long range_sum(int l, int r) const;
    // 前缀和第一次 >= target 的下标（要求所有元素非负），不存在返回 size()
    int lower_bound(long long target) const;

    // 应用：用树状数组统计逆序对个数，O(n log n)
    static long long count_inversions(const std::vector<int>& a);

private:
    int n_;
    std::vector<long long> tree_;
};
