#include "DP.h"

#include <algorithm>
#include <climits>
#include <stack>
#include <stdexcept>

namespace dp {

// ======================= 线性 DP =======================

long long fibonacci(int n)
{
    // dp[i] = dp[i-1] + dp[i-2]，只依赖前两项，用两个变量滚动即可
    long long a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        long long c = a + b;
        a = b;
        b = c;
    }
    return a;
}

long long climb_stairs(int n)
{
    return fibonacci(n + 1);
}

long long max_subarray_kadane(const std::vector<int>& a)
{
    if (a.empty())
        throw std::invalid_argument("max_subarray_kadane: empty input");
    // cur = 以当前元素结尾的最大子数组和
    long long cur = a[0], best = a[0];
    for (std::size_t i = 1; i < a.size(); ++i) {
        cur = std::max<long long>(a[i], cur + a[i]);
        best = std::max(best, cur);
    }
    return best;
}

int house_robber(const std::vector<int>& a)
{
    // dp[i] = max(dp[i-1], dp[i-2] + a[i])
    int prev2 = 0, prev1 = 0;
    for (int x : a) {
        int cur = std::max(prev1, prev2 + x);
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}

long long unique_paths(int m, int n)
{
    std::vector<long long> row(n, 1); // dp[i][j] = dp[i-1][j] + dp[i][j-1]，压缩成一维
    for (int i = 1; i < m; ++i)
        for (int j = 1; j < n; ++j)
            row[j] += row[j - 1];
    return row[n - 1];
}

int min_path_sum(const std::vector<std::vector<int>>& g)
{
    const int m = static_cast<int>(g.size()), n = static_cast<int>(g[0].size());
    std::vector<int> d(n, INT_MAX);
    d[0] = 0;
    for (int i = 0; i < m; ++i) {
        d[0] += g[i][0];
        for (int j = 1; j < n; ++j)
            d[j] = std::min(d[j], d[j - 1]) + g[i][j];
    }
    return d[n - 1];
}

// ======================= 背包 =======================

int knapsack_01(const std::vector<int>& w, const std::vector<int>& v, int W)
{
    // dp[c] = 容量为 c 时的最大价值；容量倒序遍历，保证每件物品只被用一次
    std::vector<int> d(W + 1, 0);
    for (std::size_t i = 0; i < w.size(); ++i)
        for (int c = W; c >= w[i]; --c)
            d[c] = std::max(d[c], d[c - w[i]] + v[i]);
    return d[W];
}

int knapsack_complete(const std::vector<int>& w, const std::vector<int>& v, int W)
{
    // 与 0-1 背包唯一的区别：容量正序遍历，同一件物品可以被重复加入
    std::vector<int> d(W + 1, 0);
    for (std::size_t i = 0; i < w.size(); ++i)
        for (int c = w[i]; c <= W; ++c)
            d[c] = std::max(d[c], d[c - w[i]] + v[i]);
    return d[W];
}

int coin_change(const std::vector<int>& coins, int amount)
{
    const int INF = amount + 1;
    std::vector<int> d(amount + 1, INF);
    d[0] = 0;
    for (int x = 1; x <= amount; ++x)
        for (int c : coins)
            if (c <= x)
                d[x] = std::min(d[x], d[x - c] + 1);
    return d[amount] >= INF ? -1 : d[amount];
}

long long coin_change_ways(const std::vector<int>& coins, int amount)
{
    // 外层循环硬币、内层循环金额 => 统计的是组合数（{1,2} 与 {2,1} 算一种）
    std::vector<long long> d(amount + 1, 0);
    d[0] = 1;
    for (int c : coins)
        for (int x = c; x <= amount; ++x)
            d[x] += d[x - c];
    return d[amount];
}

// ======================= 序列 DP =======================

std::string lcs(const std::string& a, const std::string& b)
{
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    // d[i][j] = a[0..i) 与 b[0..j) 的 LCS 长度
    std::vector<std::vector<int>> d(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            d[i][j] = a[i - 1] == b[j - 1] ? d[i - 1][j - 1] + 1 : std::max(d[i - 1][j], d[i][j - 1]);
    // 从右下角回溯构造一个解
    std::string out;
    for (int i = n, j = m; i > 0 && j > 0;) {
        if (a[i - 1] == b[j - 1]) {
            out.push_back(a[i - 1]);
            --i;
            --j;
        } else if (d[i - 1][j] >= d[i][j - 1]) {
            --i;
        } else {
            --j;
        }
    }
    std::reverse(out.begin(), out.end());
    return out;
}

int lis_length(const std::vector<int>& a)
{
    // tails[k] = 长度为 k+1 的上升子序列的最小结尾；tails 单调递增，可以二分
    std::vector<int> tails;
    for (int x : a) {
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end())
            tails.push_back(x);
        else
            *it = x;
    }
    return static_cast<int>(tails.size());
}

int edit_distance(const std::string& a, const std::string& b)
{
    const int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
    std::vector<std::vector<int>> d(n + 1, std::vector<int>(m + 1));
    for (int i = 0; i <= n; ++i) d[i][0] = i;
    for (int j = 0; j <= m; ++j) d[0][j] = j;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            d[i][j] = a[i - 1] == b[j - 1]
                          ? d[i - 1][j - 1]
                          : 1 + std::min({d[i - 1][j],       // 删除
                                          d[i][j - 1],       // 插入
                                          d[i - 1][j - 1]}); // 替换
    return d[n][m];
}

int longest_palindromic_subsequence(const std::string& s)
{
    const int n = static_cast<int>(s.size());
    if (n == 0)
        return 0;
    // d[i][j] = s[i..j] 的最长回文子序列；按区间长度从小到大计算
    std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
    for (int i = n - 1; i >= 0; --i) {
        d[i][i] = 1;
        for (int j = i + 1; j < n; ++j)
            d[i][j] = s[i] == s[j] ? d[i + 1][j - 1] + 2 : std::max(d[i + 1][j], d[i][j - 1]);
    }
    return d[0][n - 1];
}

// ======================= 区间 DP =======================

long long merge_stones(const std::vector<int>& piles)
{
    const int n = static_cast<int>(piles.size());
    if (n <= 1)
        return 0;
    std::vector<long long> pre(n + 1, 0);
    for (int i = 0; i < n; ++i)
        pre[i + 1] = pre[i] + piles[i];
    // d[i][j] = 把 i..j 合并成一堆的最小代价 = min(d[i][k] + d[k+1][j]) + sum(i..j)
    std::vector<std::vector<long long>> d(n, std::vector<long long>(n, 0));
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            d[i][j] = LLONG_MAX;
            for (int k = i; k < j; ++k)
                d[i][j] = std::min(d[i][j], d[i][k] + d[k + 1][j]);
            d[i][j] += pre[j + 1] - pre[i];
        }
    }
    return d[0][n - 1];
}

long long matrix_chain(const std::vector<int>& dims)
{
    const int n = static_cast<int>(dims.size()) - 1; // 矩阵个数
    if (n <= 1)
        return 0;
    std::vector<std::vector<long long>> d(n, std::vector<long long>(n, 0));
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i + len - 1 < n; ++i) {
            int j = i + len - 1;
            d[i][j] = LLONG_MAX;
            for (int k = i; k < j; ++k) // 在第 k 个矩阵之后断开
                d[i][j] = std::min(d[i][j], d[i][k] + d[k + 1][j] +
                                                static_cast<long long>(dims[i]) * dims[k + 1] * dims[j + 1]);
        }
    }
    return d[0][n - 1];
}

// ======================= 树形 DP =======================

long long tree_max_independent_set(const std::vector<std::vector<int>>& adj, const std::vector<int>& weight,
                                   int root)
{
    const int n = static_cast<int>(adj.size());
    if (n == 0)
        return 0;
    // take[u]：选 u 时子树的最大值；skip[u]：不选 u 时子树的最大值
    //   take[u] = w[u] + sum(skip[child])
    //   skip[u] = sum(max(take[child], skip[child]))
    // 用显式栈得到 DFS 序，再逆序处理（先孩子后父亲），避免深树递归爆栈
    std::vector<long long> take(n, 0), skip(n, 0);
    std::vector<int> parent(n, -1), order;
    order.reserve(n);
    std::stack<int> st;
    st.push(root);
    parent[root] = root;
    while (!st.empty()) {
        int u = st.top();
        st.pop();
        order.push_back(u);
        for (int v : adj[u]) {
            if (parent[v] == -1) {
                parent[v] = u;
                st.push(v);
            }
        }
    }
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        int u = *it;
        take[u] += weight[u];
        if (u != root) {
            int p = parent[u];
            take[p] += skip[u];
            skip[p] += std::max(take[u], skip[u]);
        }
    }
    return std::max(take[root], skip[root]);
}

int tree_diameter(const std::vector<std::vector<int>>& adj)
{
    const int n = static_cast<int>(adj.size());
    if (n == 0)
        return 0;
    // depth[u] = u 向下的最长链；经过 u 的最长路径 = 最长的两条子链之和
    std::vector<int> depth(n, 0), parent(n, -1), order;
    std::stack<int> st;
    st.push(0);
    parent[0] = 0;
    while (!st.empty()) {
        int u = st.top();
        st.pop();
        order.push_back(u);
        for (int v : adj[u])
            if (parent[v] == -1) {
                parent[v] = u;
                st.push(v);
            }
    }
    int best = 0;
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
        int u = *it, first = 0, second = 0;
        for (int v : adj[u]) {
            if (parent[v] != u || v == u) // 只看孩子
                continue;
            int len = depth[v] + 1;
            if (len > first) {
                second = first;
                first = len;
            } else if (len > second) {
                second = len;
            }
        }
        depth[u] = first;
        best = std::max(best, first + second);
    }
    return best;
}

} // namespace dp
