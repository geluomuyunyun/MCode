#include "Sort.h"

#include <algorithm>

namespace sorting {

void counting_sort(std::vector<int>& a)
{
    if (a.size() < 2)
        return;
    auto [mn, mx] = std::minmax_element(a.begin(), a.end());
    const long long lo = *mn;
    const long long range = static_cast<long long>(*mx) - lo + 1;
    std::vector<std::size_t> count(static_cast<std::size_t>(range), 0);
    for (int x : a)
        ++count[static_cast<std::size_t>(x - lo)];
    std::size_t k = 0;
    for (long long v = 0; v < range; ++v)
        for (std::size_t c = count[static_cast<std::size_t>(v)]; c > 0; --c)
            a[k++] = static_cast<int>(v + lo);
}

// 对非负数做 LSD 基数排序
static void radix_non_negative(std::vector<long long>& a)
{
    if (a.empty())
        return;
    long long mx = *std::max_element(a.begin(), a.end());
    std::vector<long long> out(a.size());
    for (long long exp = 1; mx / exp > 0; exp *= 10) {
        std::size_t count[10] = {0};
        for (long long x : a)
            ++count[(x / exp) % 10];
        for (int d = 1; d < 10; ++d)
            count[d] += count[d - 1]; // 前缀和 = 该位数字的结束位置
        for (std::size_t i = a.size(); i-- > 0;) // 倒序放置以保持稳定
            out[--count[(a[i] / exp) % 10]] = a[i];
        a.swap(out);
    }
}

void radix_sort(std::vector<int>& a)
{
    if (a.size() < 2)
        return;
    // 整体平移成非负数（用 long long 防止 INT_MIN 取反溢出）
    long long mn = *std::min_element(a.begin(), a.end());
    std::vector<long long> tmp(a.begin(), a.end());
    for (long long& x : tmp)
        x -= mn;
    radix_non_negative(tmp);
    for (std::size_t i = 0; i < a.size(); ++i)
        a[i] = static_cast<int>(tmp[i] + mn);
}

} // namespace sorting
