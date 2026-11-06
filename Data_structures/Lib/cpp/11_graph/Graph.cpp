#include "Graph.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <queue>
#include <stack>
#include <stdexcept>

Graph::Graph(int n, bool directed) : n_(n), directed_(directed), adj_(n) {}

void Graph::add_edge(int u, int v, Weight w)
{
    if (u < 0 || u >= n_ || v < 0 || v >= n_)
        throw std::out_of_range("Graph::add_edge: vertex out of range");
    adj_[u].push_back({v, w});
    if (!directed_ && u != v)
        adj_[v].push_back({u, w});
    edges_.push_back({u, v, w});
}

std::vector<std::vector<Graph::Weight>> Graph::adjacency_matrix() const
{
    std::vector<std::vector<Weight>> m(n_, std::vector<Weight>(n_, INF));
    for (int i = 0; i < n_; ++i)
        m[i][i] = 0;
    for (int u = 0; u < n_; ++u)
        for (const Edge& e : adj_[u])
            m[u][e.to] = std::min(m[u][e.to], e.w); // 重边取最小
    return m;
}

// ---------------- 遍历 ----------------

std::vector<int> Graph::bfs(int source) const
{
    std::vector<int> order;
    std::vector<bool> seen(n_, false);
    std::queue<int> q;
    q.push(source);
    seen[source] = true; // 入队时就标记，避免重复入队
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (const Edge& e : adj_[u]) {
            if (!seen[e.to]) {
                seen[e.to] = true;
                q.push(e.to);
            }
        }
    }
    return order;
}

void Graph::dfs_visit(int u, std::vector<bool>& seen, std::vector<int>& out) const
{
    seen[u] = true;
    out.push_back(u);
    for (const Edge& e : adj_[u])
        if (!seen[e.to])
            dfs_visit(e.to, seen, out);
}

std::vector<int> Graph::dfs(int source) const
{
    std::vector<int> order;
    std::vector<bool> seen(n_, false);
    dfs_visit(source, seen, order);
    return order;
}

std::vector<int> Graph::dfs_iterative(int source) const
{
    // 栈中保存 (顶点, 下一个要检查的邻居下标)，访问顺序与递归版完全一致
    std::vector<int> order;
    std::vector<bool> seen(n_, false);
    std::stack<std::pair<int, std::size_t>> st;
    st.push({source, 0});
    seen[source] = true;
    order.push_back(source);
    while (!st.empty()) {
        auto& [u, idx] = st.top();
        if (idx == adj_[u].size()) {
            st.pop();
            continue;
        }
        int v = adj_[u][idx++].to;
        if (!seen[v]) {
            seen[v] = true;
            order.push_back(v);
            st.push({v, 0});
        }
    }
    return order;
}

std::vector<int> Graph::bfs_distances(int source) const
{
    std::vector<int> dist(n_, -1);
    std::queue<int> q;
    dist[source] = 0;
    q.push(source);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (const Edge& e : adj_[u]) {
            if (dist[e.to] < 0) {
                dist[e.to] = dist[u] + 1;
                q.push(e.to);
            }
        }
    }
    return dist;
}

int Graph::connected_components(std::vector<int>* label) const
{
    std::vector<int> comp(n_, -1);
    int count = 0;
    for (int s = 0; s < n_; ++s) {
        if (comp[s] >= 0)
            continue;
        std::queue<int> q;
        q.push(s);
        comp[s] = count;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const Edge& e : adj_[u]) {
                if (comp[e.to] < 0) {
                    comp[e.to] = count;
                    q.push(e.to);
                }
            }
        }
        ++count;
    }
    if (label)
        *label = std::move(comp);
    return count;
}

// ---------------- 有向图 ----------------

bool Graph::topological_sort(std::vector<int>& order) const
{
    // Kahn 算法：不断取出入度为 0 的顶点
    std::vector<int> indeg(n_, 0);
    for (int u = 0; u < n_; ++u)
        for (const Edge& e : adj_[u])
            ++indeg[e.to];
    // 用最小堆，使入度同时为 0 的顶点按编号从小到大输出，结果唯一、便于测试
    std::priority_queue<int, std::vector<int>, std::greater<int>> ready;
    for (int u = 0; u < n_; ++u)
        if (indeg[u] == 0)
            ready.push(u);
    order.clear();
    while (!ready.empty()) {
        int u = ready.top();
        ready.pop();
        order.push_back(u);
        for (const Edge& e : adj_[u])
            if (--indeg[e.to] == 0)
                ready.push(e.to);
    }
    return static_cast<int>(order.size()) == n_; // 有顶点没输出说明存在环
}

bool Graph::has_cycle() const
{
    if (directed_) {
        std::vector<int> order;
        return !topological_sort(order);
    }
    // 无向图：边数 > 顶点数 - 连通分量数 时必有环（森林的边数恰好是 n - c）
    return static_cast<int>(edges_.size()) > n_ - connected_components();
}

// ---------------- 最短路 ----------------

std::vector<int> Graph::ShortestPaths::path_to(int target) const
{
    std::vector<int> path;
    if (dist[target] >= INF)
        return path;
    for (int v = target; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path;
}

Graph::ShortestPaths Graph::dijkstra(int source) const
{
    ShortestPaths r;
    r.dist.assign(n_, INF);
    r.parent.assign(n_, -1);
    using Item = std::pair<Weight, int>; // (距离, 顶点)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    r.dist[source] = 0;
    pq.push({0, source});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > r.dist[u])
            continue; // 过期条目（懒删除）
        for (const Edge& e : adj_[u]) {
            if (e.w < 0)
                throw std::invalid_argument("Dijkstra requires non-negative weights");
            if (d + e.w < r.dist[e.to]) {
                r.dist[e.to] = d + e.w;
                r.parent[e.to] = u;
                pq.push({r.dist[e.to], e.to});
            }
        }
    }
    return r;
}

Graph::ShortestPaths Graph::bellman_ford(int source) const
{
    ShortestPaths r;
    r.dist.assign(n_, INF);
    r.parent.assign(n_, -1);
    r.dist[source] = 0;
    // 最短路最多 n-1 条边，所以松弛 n-1 轮一定收敛；第 n 轮还能松弛说明有负环
    for (int round = 0; round < n_; ++round) {
        bool changed = false;
        for (int u = 0; u < n_; ++u) {
            if (r.dist[u] >= INF)
                continue;
            for (const Edge& e : adj_[u]) {
                if (r.dist[u] + e.w < r.dist[e.to]) {
                    r.dist[e.to] = r.dist[u] + e.w;
                    r.parent[e.to] = u;
                    changed = true;
                }
            }
        }
        if (!changed)
            return r;
        if (round == n_ - 1)
            r.negative_cycle = true;
    }
    return r;
}

std::vector<std::vector<Graph::Weight>> Graph::floyd() const
{
    auto d = adjacency_matrix();
    // dp 含义：只允许经过 0..k 作为中转点时的最短距离，k 必须在最外层
    for (int k = 0; k < n_; ++k)
        for (int i = 0; i < n_; ++i) {
            if (d[i][k] >= INF)
                continue;
            for (int j = 0; j < n_; ++j)
                if (d[k][j] < INF && d[i][k] + d[k][j] < d[i][j])
                    d[i][j] = d[i][k] + d[k][j];
        }
    return d;
}

// ---------------- 最小生成树 ----------------

namespace {

// Kruskal 用的简化并查集（完整版见 c/09_union_find）
struct DSU {
    std::vector<int> parent, rank;
    explicit DSU(int n) : parent(n), rank(n, 0) { std::iota(parent.begin(), parent.end(), 0); }
    int find(int x)
    {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]]; // 路径减半
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b)
            return false;
        if (rank[a] < rank[b])
            std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b])
            ++rank[a];
        return true;
    }
};

} // namespace

Graph::MST Graph::kruskal() const
{
    if (directed_)
        throw std::logic_error("MST is defined for undirected graphs");
    // 按权值从小到大选边，跳过会成环的边
    std::vector<WeightedEdge> sorted = edges_;
    std::stable_sort(sorted.begin(), sorted.end(),
                     [](const WeightedEdge& a, const WeightedEdge& b) { return a.w < b.w; });
    MST m;
    DSU dsu(n_);
    for (const WeightedEdge& e : sorted) {
        if (dsu.unite(e.u, e.v)) {
            m.total += e.w;
            m.edges.push_back(e);
        }
    }
    m.connected = n_ == 0 || static_cast<int>(m.edges.size()) == n_ - 1;
    return m;
}

Graph::MST Graph::prim() const
{
    if (directed_)
        throw std::logic_error("MST is defined for undirected graphs");
    // 从已选集合向外扩展，每次选连接集合内外的最小边
    MST m;
    std::vector<bool> in_tree(n_, false);
    using Item = std::tuple<Weight, int, int>; // (权值, 新顶点, 来自的顶点)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    for (int start = 0; start < n_; ++start) { // 外层循环让不连通的图也能得到生成森林
        if (in_tree[start])
            continue;
        pq.push({0, start, -1});
        while (!pq.empty()) {
            auto [w, v, from] = pq.top();
            pq.pop();
            if (in_tree[v])
                continue;
            in_tree[v] = true;
            if (from >= 0) {
                m.total += w;
                m.edges.push_back({from, v, w});
            }
            for (const Edge& e : adj_[v])
                if (!in_tree[e.to])
                    pq.push({e.w, e.to, v});
        }
    }
    m.connected = n_ == 0 || static_cast<int>(m.edges.size()) == n_ - 1;
    return m;
}
