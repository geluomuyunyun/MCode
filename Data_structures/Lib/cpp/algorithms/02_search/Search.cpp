#include "Search.h"

#include <algorithm>
#include <numeric>

namespace search {

int linear_search(const std::vector<int>& a, int target)
{
    for (int i = 0; i < static_cast<int>(a.size()); ++i)
        if (a[i] == target)
            return i;
    return -1;
}

int lower_bound(const std::vector<int>& a, int target)
{
    int lo = 0, hi = static_cast<int>(a.size());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2; // 不写 (lo+hi)/2，防止溢出
        if (a[mid] < target)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

int upper_bound(const std::vector<int>& a, int target)
{
    int lo = 0, hi = static_cast<int>(a.size());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] <= target)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

int binary_search(const std::vector<int>& a, int target)
{
    int i = lower_bound(a, target);
    return i < static_cast<int>(a.size()) && a[i] == target ? i : -1;
}

std::pair<int, int> equal_range(const std::vector<int>& a, int target)
{
    int l = lower_bound(a, target), r = upper_bound(a, target);
    return l == r ? std::make_pair(-1, -1) : std::make_pair(l, r - 1);
}

int search_rotated(const std::vector<int>& a, int target)
{
    int lo = 0, hi = static_cast<int>(a.size()) - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == target)
            return mid;
        if (a[lo] <= a[mid]) { // 左半段有序
            if (a[lo] <= target && target < a[mid])
                hi = mid - 1;
            else
                lo = mid + 1;
        } else { // 右半段有序
            if (a[mid] < target && target <= a[hi])
                lo = mid + 1;
            else
                hi = mid - 1;
        }
    }
    return -1;
}

int find_min_rotated(const std::vector<int>& a)
{
    if (a.empty())
        return -1;
    int lo = 0, hi = static_cast<int>(a.size()) - 1;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] > a[hi])
            lo = mid + 1; // 最小值在右边
        else
            hi = mid;
    }
    return lo;
}

int find_peak(const std::vector<int>& a)
{
    if (a.empty())
        return -1;
    int lo = 0, hi = static_cast<int>(a.size()) - 1;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < a[mid + 1])
            lo = mid + 1; // 往上坡方向走一定能找到峰
        else
            hi = mid;
    }
    return lo;
}

int int_sqrt(long long n)
{
    if (n < 2)
        return static_cast<int>(n < 0 ? 0 : n);
    long long lo = 1, hi = std::min<long long>(n, 3037000499LL); // sqrt(LLONG_MAX)
    while (lo < hi) {
        long long mid = lo + (hi - lo + 1) / 2; // 找最后一个满足条件的，mid 向上取整
        if (mid * mid <= n)
            lo = mid;
        else
            hi = mid - 1;
    }
    return static_cast<int>(lo);
}

long long split_array_min_largest(const std::vector<int>& a, int k)
{
    if (a.empty())
        return 0;
    // 答案在 [最大元素, 总和] 之间；"最大段和不超过 x 时需要的段数"随 x 单调不增
    long long lo = *std::max_element(a.begin(), a.end());
    long long hi = std::accumulate(a.begin(), a.end(), 0LL);
    auto pieces_needed = [&](long long cap) {
        int pieces = 1;
        long long cur = 0;
        for (int x : a) {
            if (cur + x > cap) {
                ++pieces;
                cur = 0;
            }
            cur += x;
        }
        return pieces;
    };
    while (lo < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (pieces_needed(mid) <= k)
            hi = mid;
        else
            lo = mid + 1;
    }
    return lo;
}

} // namespace search
