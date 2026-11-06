#include "SlidingWindow.h"

#include <algorithm>
#include <array>
#include <climits>
#include <deque>

namespace window {

int longest_unique_substring(const std::string& s)
{
    std::array<int, 256> last{}; // 字符上一次出现的位置 + 1
    int best = 0;
    for (int left = 0, right = 0; right < static_cast<int>(s.size()); ++right) {
        unsigned char c = static_cast<unsigned char>(s[right]);
        left = std::max(left, last[c]); // 出现重复时，左边界直接跳到重复字符之后
        last[c] = right + 1;
        best = std::max(best, right - left + 1);
    }
    return best;
}

int min_subarray_len(int target, const std::vector<int>& a)
{
    int best = INT_MAX;
    long long sum = 0;
    for (int left = 0, right = 0; right < static_cast<int>(a.size()); ++right) {
        sum += a[right];
        while (sum >= target) { // 满足条件时尽量收缩
            best = std::min(best, right - left + 1);
            sum -= a[left++];
        }
    }
    return best == INT_MAX ? 0 : best;
}

std::vector<int> max_sliding_window(const std::vector<int>& a, int k)
{
    std::vector<int> out;
    if (k <= 0)
        return out;
    std::deque<int> dq; // 存下标，对应值单调递减；队头就是当前窗口最大值
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        while (!dq.empty() && a[dq.back()] <= a[i])
            dq.pop_back(); // 比新元素小的旧元素永远不可能再成为最大值
        dq.push_back(i);
        if (dq.front() <= i - k)
            dq.pop_front(); // 队头滑出窗口
        if (i >= k - 1)
            out.push_back(a[dq.front()]);
    }
    return out;
}

std::vector<int> find_anagrams(const std::string& s, const std::string& p)
{
    std::vector<int> out;
    const int n = static_cast<int>(s.size()), m = static_cast<int>(p.size());
    if (m == 0 || m > n)
        return out;
    std::array<int, 256> need{}, have{};
    for (char c : p)
        ++need[static_cast<unsigned char>(c)];
    for (int i = 0; i < n; ++i) {
        ++have[static_cast<unsigned char>(s[i])];
        if (i >= m)
            --have[static_cast<unsigned char>(s[i - m])];
        if (i >= m - 1 && have == need)
            out.push_back(i - m + 1);
    }
    return out;
}

std::string min_window(const std::string& s, const std::string& t)
{
    if (t.empty())
        return "";
    std::array<int, 256> need{};
    for (char c : t)
        ++need[static_cast<unsigned char>(c)];
    int missing = static_cast<int>(t.size()); // 还差多少个字符没覆盖
    int best_start = 0, best_len = INT_MAX;
    for (int left = 0, right = 0; right < static_cast<int>(s.size()); ++right) {
        if (need[static_cast<unsigned char>(s[right])]-- > 0)
            --missing;
        while (missing == 0) {
            if (right - left + 1 < best_len) {
                best_len = right - left + 1;
                best_start = left;
            }
            // need 为负表示窗口里这个字符有富余；移出后变为正说明又缺了
            if (++need[static_cast<unsigned char>(s[left++])] > 0)
                ++missing;
        }
    }
    return best_len == INT_MAX ? "" : s.substr(best_start, best_len);
}

int longest_with_k_distinct(const std::string& s, int k)
{
    std::array<int, 256> cnt{};
    int distinct = 0, best = 0;
    for (int left = 0, right = 0; right < static_cast<int>(s.size()); ++right) {
        if (cnt[static_cast<unsigned char>(s[right])]++ == 0)
            ++distinct;
        while (distinct > k) {
            if (--cnt[static_cast<unsigned char>(s[left++])] == 0)
                --distinct;
        }
        best = std::max(best, right - left + 1);
    }
    return best;
}

} // namespace window
