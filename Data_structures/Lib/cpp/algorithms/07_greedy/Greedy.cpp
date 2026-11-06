#include "Greedy.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <queue>

namespace greedy {

int max_non_overlapping(std::vector<Interval> intervals)
{
    // 结束得越早，留给后面的空间越多
    std::sort(intervals.begin(), intervals.end(),
              [](const Interval& a, const Interval& b) { return a.second < b.second; });
    int count = 0;
    long long last_end = static_cast<long long>(INT_MIN) - 1;
    for (const auto& [s, e] : intervals) {
        if (s >= last_end) {
            ++count;
            last_end = e;
        }
    }
    return count;
}

std::vector<Interval> merge_intervals(std::vector<Interval> intervals)
{
    std::sort(intervals.begin(), intervals.end());
    std::vector<Interval> out;
    for (const auto& iv : intervals) {
        if (!out.empty() && iv.first <= out.back().second)
            out.back().second = std::max(out.back().second, iv.second);
        else
            out.push_back(iv);
    }
    return out;
}

bool can_jump(const std::vector<int>& a)
{
    long long reach = 0; // 目前能到达的最远位置
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        if (i > reach)
            return false;
        reach = std::max(reach, static_cast<long long>(i) + a[i]);
    }
    return true;
}

int min_jumps(const std::vector<int>& a)
{
    // 类似 BFS 分层：[start, end] 是用 jumps 步能到达的范围
    int jumps = 0;
    long long end = 0, farthest = 0;
    for (int i = 0; i + 1 < static_cast<int>(a.size()); ++i) {
        farthest = std::max(farthest, static_cast<long long>(i) + a[i]);
        if (i == end) {
            ++jumps;
            end = farthest;
        }
    }
    return jumps;
}

long long huffman_cost(const std::vector<long long>& freq)
{
    // 每次合并最小的两个，频率越小的字符在树中越深
    std::priority_queue<long long, std::vector<long long>, std::greater<long long>> pq(freq.begin(), freq.end());
    long long cost = 0;
    while (pq.size() > 1) {
        long long a = pq.top();
        pq.pop();
        long long b = pq.top();
        pq.pop();
        cost += a + b;
        pq.push(a + b);
    }
    return cost;
}

int assign_cookies(std::vector<int> greed, std::vector<int> cookies)
{
    std::sort(greed.begin(), greed.end());
    std::sort(cookies.begin(), cookies.end());
    std::size_t child = 0;
    // 用尽量小的饼干满足胃口最小的孩子
    for (std::size_t c = 0; c < cookies.size() && child < greed.size(); ++c)
        if (cookies[c] >= greed[child])
            ++child;
    return static_cast<int>(child);
}

int gas_station(const std::vector<int>& gas, const std::vector<int>& cost)
{
    long long total = 0, tank = 0;
    int start = 0;
    for (int i = 0; i < static_cast<int>(gas.size()); ++i) {
        total += gas[i] - cost[i];
        tank += gas[i] - cost[i];
        if (tank < 0) { // 从 start 出发到不了 i+1，start..i 之间任何点出发也到不了
            start = i + 1;
            tank = 0;
        }
    }
    return total >= 0 ? start : -1;
}

int coin_change_greedy(std::vector<int> coins, int amount)
{
    std::sort(coins.begin(), coins.end(), std::greater<int>());
    int count = 0;
    for (int c : coins) {
        count += amount / c;
        amount %= c;
    }
    return amount == 0 ? count : -1;
}

std::string remove_k_digits(const std::string& num, int k)
{
    // 单调栈：前面的数字比后面大时，删掉前面的能让数更小
    std::string st;
    for (char c : num) {
        while (k > 0 && !st.empty() && st.back() > c) {
            st.pop_back();
            --k;
        }
        st.push_back(c);
    }
    while (k-- > 0 && !st.empty())
        st.pop_back();
    std::size_t nz = st.find_first_not_of('0');
    return nz == std::string::npos ? "0" : st.substr(nz);
}

} // namespace greedy
