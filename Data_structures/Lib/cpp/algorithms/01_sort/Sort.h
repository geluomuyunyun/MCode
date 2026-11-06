// Sort.h - 经典排序算法。
//
//   算法        平均        最坏        额外空间   稳定
//   冒泡        O(n^2)      O(n^2)      O(1)       是
//   选择        O(n^2)      O(n^2)      O(1)       否
//   插入        O(n^2)      O(n^2)      O(1)       是     （几乎有序时接近 O(n)）
//   希尔        约 O(n^1.3) O(n^2)      O(1)       否
//   归并        O(n log n)  O(n log n)  O(n)       是
//   快速        O(n log n)  O(n^2)      O(log n)   否
//   堆          O(n log n)  O(n log n)  O(1)       否
//   计数        O(n + k)    O(n + k)    O(n + k)   是     （仅整数，k 为值域）
//   基数        O(d(n + b)) O(d(n + b)) O(n + b)   是     （仅整数）
//
// 比较类排序是模板，实现在本头文件；计数排序与基数排序只针对 int，实现在 Sort.cpp。
#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace sorting {

template <typename T, typename Cmp = std::less<T>>
void bubble_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    for (std::size_t n = a.size(); n > 1; --n) {
        bool swapped = false;
        for (std::size_t i = 0; i + 1 < n; ++i) {
            if (cmp(a[i + 1], a[i])) {
                std::swap(a[i], a[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) // 一轮没有交换说明已经有序，提前结束
            break;
    }
}

template <typename T, typename Cmp = std::less<T>>
void selection_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    for (std::size_t i = 0; i + 1 < a.size(); ++i) {
        std::size_t m = i;
        for (std::size_t j = i + 1; j < a.size(); ++j)
            if (cmp(a[j], a[m]))
                m = j;
        if (m != i)
            std::swap(a[i], a[m]);
    }
}

template <typename T, typename Cmp = std::less<T>>
void insertion_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    for (std::size_t i = 1; i < a.size(); ++i) {
        T key = std::move(a[i]);
        std::size_t j = i;
        while (j > 0 && cmp(key, a[j - 1])) {
            a[j] = std::move(a[j - 1]);
            --j;
        }
        a[j] = std::move(key);
    }
}

template <typename T, typename Cmp = std::less<T>>
void shell_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    // Knuth 增量序列 1, 4, 13, 40, ...
    std::size_t gap = 1;
    while (gap < a.size() / 3)
        gap = gap * 3 + 1;
    for (; gap > 0; gap /= 3) {
        for (std::size_t i = gap; i < a.size(); ++i) {
            T key = std::move(a[i]);
            std::size_t j = i;
            while (j >= gap && cmp(key, a[j - gap])) {
                a[j] = std::move(a[j - gap]);
                j -= gap;
            }
            a[j] = std::move(key);
        }
    }
}

namespace detail {

template <typename T, typename Cmp>
void merge_sort(std::vector<T>& a, std::vector<T>& buf, std::size_t lo, std::size_t hi, Cmp& cmp)
{
    if (hi - lo < 2)
        return;
    std::size_t mid = lo + (hi - lo) / 2;
    merge_sort(a, buf, lo, mid, cmp);
    merge_sort(a, buf, mid, hi, cmp);
    if (!cmp(a[mid], a[mid - 1]))
        return; // 左半最大值 <= 右半最小值，已经有序
    std::size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
        buf[k++] = cmp(a[j], a[i]) ? std::move(a[j++]) : std::move(a[i++]); // 相等取左边，保证稳定
    while (i < mid) buf[k++] = std::move(a[i++]);
    while (j < hi) buf[k++] = std::move(a[j++]);
    for (k = lo; k < hi; ++k)
        a[k] = std::move(buf[k]);
}

// 三数取中选枢轴 + 三路划分（<, ==, >），大量重复元素时也不会退化
template <typename T, typename Cmp>
void quick_sort(std::vector<T>& a, std::ptrdiff_t lo, std::ptrdiff_t hi, Cmp& cmp)
{
    while (hi - lo > 16) {
        std::ptrdiff_t mid = lo + (hi - lo) / 2;
        if (cmp(a[mid], a[lo])) std::swap(a[mid], a[lo]);
        if (cmp(a[hi], a[lo])) std::swap(a[hi], a[lo]);
        if (cmp(a[hi], a[mid])) std::swap(a[hi], a[mid]);
        T pivot = a[mid];

        std::ptrdiff_t lt = lo, i = lo, gt = hi; // [lo,lt) < pivot, [lt,i) == pivot, (gt,hi] > pivot
        while (i <= gt) {
            if (cmp(a[i], pivot)) std::swap(a[lt++], a[i++]);
            else if (cmp(pivot, a[i])) std::swap(a[i], a[gt--]);
            else ++i;
        }
        // 先递归较短的一边，较长的一边用循环处理，栈深度保证 O(log n)
        if (lt - lo < hi - gt) {
            quick_sort(a, lo, lt - 1, cmp);
            lo = gt + 1;
        } else {
            quick_sort(a, gt + 1, hi, cmp);
            hi = lt - 1;
        }
    }
    // 小区间用插入排序更快
    for (std::ptrdiff_t i = lo + 1; i <= hi; ++i) {
        T key = std::move(a[i]);
        std::ptrdiff_t j = i;
        while (j > lo && cmp(key, a[j - 1])) {
            a[j] = std::move(a[j - 1]);
            --j;
        }
        a[j] = std::move(key);
    }
}

template <typename T, typename Cmp>
void sift_down(std::vector<T>& a, std::size_t i, std::size_t n, Cmp& cmp)
{
    for (;;) {
        std::size_t best = i, l = 2 * i + 1, r = l + 1;
        if (l < n && cmp(a[best], a[l])) best = l;
        if (r < n && cmp(a[best], a[r])) best = r;
        if (best == i) return;
        std::swap(a[i], a[best]);
        i = best;
    }
}

} // namespace detail

template <typename T, typename Cmp = std::less<T>>
void merge_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    std::vector<T> buf(a.size());
    detail::merge_sort(a, buf, 0, a.size(), cmp);
}

template <typename T, typename Cmp = std::less<T>>
void quick_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    if (a.size() > 1)
        detail::quick_sort(a, 0, static_cast<std::ptrdiff_t>(a.size()) - 1, cmp);
}

template <typename T, typename Cmp = std::less<T>>
void heap_sort(std::vector<T>& a, Cmp cmp = Cmp())
{
    std::size_t n = a.size();
    for (std::size_t i = n / 2; i-- > 0;)
        detail::sift_down(a, i, n, cmp);
    for (std::size_t end = n; end > 1; --end) {
        std::swap(a[0], a[end - 1]);
        detail::sift_down(a, 0, end - 1, cmp);
    }
}

// 计数排序：统计每个值出现的次数，适合值域不大的整数
void counting_sort(std::vector<int>& a);

// LSD 基数排序：按十进制位从低到高做稳定的计数排序，支持负数
void radix_sort(std::vector<int>& a);

} // namespace sorting
