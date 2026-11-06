// SegmentTree.h - 线段树：区间加 + 区间求和 + 区间最小值，带懒标记。
//
// 每个结点管理一段区间 [l, r]，根管理整个数组，叶子管理单个元素。
// 懒标记（lazy）：区间修改时如果当前结点的区间被完全覆盖，就只在这里记一笔，
// 等以后真正需要访问它的孩子时再下推（push_down），从而让区间修改也是 O(log n)。
// 下标从 0 开始，所有区间都是闭区间 [l, r]。
#pragma once

#include <vector>

class SegmentTree {
public:
    explicit SegmentTree(const std::vector<long long>& a);

    int size() const { return n_; }
    void range_add(int l, int r, long long delta);
    void point_set(int i, long long value);
    long long range_sum(int l, int r);
    long long range_min(int l, int r);

private:
    void build(int node, int l, int r, const std::vector<long long>& a);
    void apply(int node, int l, int r, long long delta);
    void push_down(int node, int l, int r);
    void pull(int node);
    void add(int node, int l, int r, int ql, int qr, long long delta);
    long long sum(int node, int l, int r, int ql, int qr);
    long long min(int node, int l, int r, int ql, int qr);

    int n_;
    std::vector<long long> sum_, min_, lazy_;
};
