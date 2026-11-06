#include "TwoPointers.h"

#include <algorithm>
#include <cctype>

namespace two_pointers {

std::pair<int, int> two_sum_sorted(const std::vector<int>& a, int target)
{
    int l = 0, r = static_cast<int>(a.size()) - 1;
    while (l < r) {
        long long s = static_cast<long long>(a[l]) + a[r];
        if (s == target)
            return {l, r};
        if (s < target)
            ++l; // a[l] 和任何数相加都不够大，排除 l
        else
            --r; // a[r] 和任何数相加都太大，排除 r
    }
    return {-1, -1};
}

std::vector<std::array<int, 3>> three_sum(std::vector<int> a)
{
    std::sort(a.begin(), a.end());
    std::vector<std::array<int, 3>> out;
    const int n = static_cast<int>(a.size());
    for (int i = 0; i < n - 2; ++i) {
        if (a[i] > 0)
            break;
        if (i > 0 && a[i] == a[i - 1])
            continue; // 跳过重复的第一个数
        int l = i + 1, r = n - 1;
        while (l < r) {
            long long s = static_cast<long long>(a[i]) + a[l] + a[r];
            if (s < 0) {
                ++l;
            } else if (s > 0) {
                --r;
            } else {
                out.push_back({a[i], a[l], a[r]});
                while (l < r && a[l] == a[l + 1]) ++l;
                while (l < r && a[r] == a[r - 1]) --r;
                ++l;
                --r;
            }
        }
    }
    return out;
}

long long max_area(const std::vector<int>& h)
{
    long long best = 0;
    int l = 0, r = static_cast<int>(h.size()) - 1;
    while (l < r) {
        best = std::max(best, static_cast<long long>(std::min(h[l], h[r])) * (r - l));
        // 移动较矮的一边：容器高度受短板限制，移动高的一边面积只会更小
        if (h[l] < h[r])
            ++l;
        else
            --r;
    }
    return best;
}

long long trap_rain_water(const std::vector<int>& h)
{
    int l = 0, r = static_cast<int>(h.size()) - 1;
    int left_max = 0, right_max = 0;
    long long water = 0;
    while (l < r) {
        // 较矮一侧的水位由它自己那一侧的最大值决定
        if (h[l] < h[r]) {
            left_max = std::max(left_max, h[l]);
            water += left_max - h[l];
            ++l;
        } else {
            right_max = std::max(right_max, h[r]);
            water += right_max - h[r];
            --r;
        }
    }
    return water;
}

int remove_duplicates(std::vector<int>& a)
{
    if (a.empty())
        return 0;
    int slow = 0;
    for (int fast = 1; fast < static_cast<int>(a.size()); ++fast)
        if (a[fast] != a[slow])
            a[++slow] = a[fast];
    return slow + 1;
}

void move_zeroes(std::vector<int>& a)
{
    std::size_t slow = 0;
    for (std::size_t fast = 0; fast < a.size(); ++fast)
        if (a[fast] != 0)
            std::swap(a[slow++], a[fast]);
}

bool is_palindrome(const std::string& s)
{
    int l = 0, r = static_cast<int>(s.size()) - 1;
    auto ok = [](char c) { return std::isalnum(static_cast<unsigned char>(c)) != 0; };
    while (l < r) {
        if (!ok(s[l])) { ++l; continue; }
        if (!ok(s[r])) { --r; continue; }
        if (std::tolower(static_cast<unsigned char>(s[l])) != std::tolower(static_cast<unsigned char>(s[r])))
            return false;
        ++l;
        --r;
    }
    return true;
}

std::vector<int> merge_sorted(const std::vector<int>& a, const std::vector<int>& b)
{
    std::vector<int> out;
    out.reserve(a.size() + b.size());
    std::size_t i = 0, j = 0;
    while (i < a.size() && j < b.size())
        out.push_back(b[j] < a[i] ? b[j++] : a[i++]);
    out.insert(out.end(), a.begin() + i, a.end());
    out.insert(out.end(), b.begin() + j, b.end());
    return out;
}

void sort_colors(std::vector<int>& a)
{
    // [0, lo) 全是 0，[lo, i) 全是 1，(hi, n) 全是 2
    int lo = 0, i = 0, hi = static_cast<int>(a.size()) - 1;
    while (i <= hi) {
        if (a[i] == 0) std::swap(a[lo++], a[i++]);
        else if (a[i] == 2) std::swap(a[i], a[hi--]); // 换过来的数还没检查，i 不动
        else ++i;
    }
}

} // namespace two_pointers
