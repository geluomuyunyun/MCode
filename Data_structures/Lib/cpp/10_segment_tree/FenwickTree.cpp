#include "FenwickTree.h"

#include <algorithm>

static int lowbit(int i)
{
    return i & (-i);
}

FenwickTree::FenwickTree(int n) : n_(n), tree_(n + 1, 0) {}

FenwickTree::FenwickTree(const std::vector<long long>& a) : n_(static_cast<int>(a.size())), tree_(a.size() + 1, 0)
{
    // 每个结点把自己的值贡献给直接父结点，整体 O(n)
    for (int i = 1; i <= n_; ++i) {
        tree_[i] += a[i - 1];
        int parent = i + lowbit(i);
        if (parent <= n_)
            tree_[parent] += tree_[i];
    }
}

void FenwickTree::add(int i, long long delta)
{
    for (++i; i <= n_; i += lowbit(i))
        tree_[i] += delta;
}

long long FenwickTree::prefix_sum(int i) const
{
    long long s = 0;
    for (++i; i > 0; i -= lowbit(i))
        s += tree_[i];
    return s;
}

long long FenwickTree::range_sum(int l, int r) const
{
    return prefix_sum(r) - prefix_sum(l - 1);
}

int FenwickTree::lower_bound(long long target) const
{
    // 从高位到低位倍增地确定答案，O(log n)
    int pos = 0;
    int step = 1;
    while (step * 2 <= n_)
        step *= 2;
    for (; step > 0; step /= 2) {
        if (pos + step <= n_ && tree_[pos + step] < target) {
            pos += step;
            target -= tree_[pos];
        }
    }
    return pos; // 内部下标 pos+1，对外下标 pos
}

long long FenwickTree::count_inversions(const std::vector<int>& a)
{
    // 离散化：把值映射为 0..m-1 的排名
    std::vector<int> sorted(a);
    std::sort(sorted.begin(), sorted.end());
    sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
    FenwickTree bit(static_cast<int>(sorted.size()));
    long long inv = 0;
    // 从右往左扫，统计右边已出现的、比当前值小的个数
    for (auto it = a.rbegin(); it != a.rend(); ++it) {
        int rank = static_cast<int>(std::lower_bound(sorted.begin(), sorted.end(), *it) - sorted.begin());
        inv += bit.prefix_sum(rank - 1);
        bit.add(rank, 1);
    }
    return inv;
}
