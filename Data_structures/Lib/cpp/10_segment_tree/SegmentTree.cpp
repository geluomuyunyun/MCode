#include "SegmentTree.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

SegmentTree::SegmentTree(const std::vector<long long>& a)
    : n_(static_cast<int>(a.size())), sum_(4 * a.size() + 4), min_(4 * a.size() + 4), lazy_(4 * a.size() + 4)
{
    if (n_ > 0)
        build(1, 0, n_ - 1, a);
}

void SegmentTree::build(int node, int l, int r, const std::vector<long long>& a)
{
    if (l == r) {
        sum_[node] = min_[node] = a[l];
        return;
    }
    int m = l + (r - l) / 2;
    build(2 * node, l, m, a);
    build(2 * node + 1, m + 1, r, a);
    pull(node);
}

void SegmentTree::pull(int node)
{
    sum_[node] = sum_[2 * node] + sum_[2 * node + 1];
    min_[node] = std::min(min_[2 * node], min_[2 * node + 1]);
}

// 整段加 delta：和增加 delta*长度，最小值增加 delta，并记下懒标记
void SegmentTree::apply(int node, int l, int r, long long delta)
{
    sum_[node] += delta * (r - l + 1);
    min_[node] += delta;
    lazy_[node] += delta;
}

void SegmentTree::push_down(int node, int l, int r)
{
    if (lazy_[node] == 0)
        return;
    int m = l + (r - l) / 2;
    apply(2 * node, l, m, lazy_[node]);
    apply(2 * node + 1, m + 1, r, lazy_[node]);
    lazy_[node] = 0;
}

void SegmentTree::add(int node, int l, int r, int ql, int qr, long long delta)
{
    if (qr < l || r < ql)
        return;
    if (ql <= l && r <= qr) {
        apply(node, l, r, delta);
        return;
    }
    push_down(node, l, r);
    int m = l + (r - l) / 2;
    add(2 * node, l, m, ql, qr, delta);
    add(2 * node + 1, m + 1, r, ql, qr, delta);
    pull(node);
}

long long SegmentTree::sum(int node, int l, int r, int ql, int qr)
{
    if (qr < l || r < ql)
        return 0;
    if (ql <= l && r <= qr)
        return sum_[node];
    push_down(node, l, r);
    int m = l + (r - l) / 2;
    return sum(2 * node, l, m, ql, qr) + sum(2 * node + 1, m + 1, r, ql, qr);
}

long long SegmentTree::min(int node, int l, int r, int ql, int qr)
{
    if (qr < l || r < ql)
        return std::numeric_limits<long long>::max();
    if (ql <= l && r <= qr)
        return min_[node];
    push_down(node, l, r);
    int m = l + (r - l) / 2;
    return std::min(min(2 * node, l, m, ql, qr), min(2 * node + 1, m + 1, r, ql, qr));
}

static void check_range(int l, int r, int n)
{
    if (l < 0 || r >= n || l > r)
        throw std::out_of_range("SegmentTree: bad range");
}

void SegmentTree::range_add(int l, int r, long long delta)
{
    check_range(l, r, n_);
    add(1, 0, n_ - 1, l, r, delta);
}

void SegmentTree::point_set(int i, long long value)
{
    range_add(i, i, value - range_sum(i, i));
}

long long SegmentTree::range_sum(int l, int r)
{
    check_range(l, r, n_);
    return sum(1, 0, n_ - 1, l, r);
}

long long SegmentTree::range_min(int l, int r)
{
    check_range(l, r, n_);
    return min(1, 0, n_ - 1, l, r);
}
