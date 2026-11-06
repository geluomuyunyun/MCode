// Graph.h - 带权图（邻接表存储），顶点编号 0..n-1，可选有向/无向。
//
// 图论算法都作为成员函数放在这里：
//   遍历：      BFS、DFS（递归 + 非递归）、连通分量
//   有向无环图：拓扑排序（Kahn 算法）、环检测
//   最短路：    BFS（无权）、Dijkstra（非负权）、Bellman-Ford（可负权）、Floyd（全源）
//   最小生成树：Kruskal（并查集）、Prim（堆）
#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <vector>

class Graph {
public:
    using Weight = long long;
    static constexpr Weight INF = std::numeric_limits<Weight>::max() / 4;

    struct Edge {
        int to;
        Weight w;
    };
    struct WeightedEdge {
        int u, v;
        Weight w;
        bool operator==(const WeightedEdge& o) const { return u == o.u && v == o.v && w == o.w; }
    };
    struct ShortestPaths {
        std::vector<Weight> dist;  // 不可达为 INF
        std::vector<int> parent;   // 最短路树中的父结点，源点和不可达点为 -1
        bool negative_cycle = false;
        std::vector<int> path_to(int target) const;
    };
    struct MST {
        Weight total = 0;
        std::vector<WeightedEdge> edges;
        bool connected = true; // 图不连通时只能得到最小生成森林
    };

    Graph(int n, bool directed);

    void add_edge(int u, int v, Weight w = 1);
    int vertex_count() const { return n_; }
    std::size_t edge_count() const { return edges_.size(); }
    bool directed() const { return directed_; }
    const std::vector<Edge>& neighbors(int u) const { return adj_[u]; }

    // 邻接矩阵表示：无边为 INF，对角线为 0
    std::vector<std::vector<Weight>> adjacency_matrix() const;

    // ---------- 遍历 ----------
    std::vector<int> bfs(int source) const;
    std::vector<int> dfs(int source) const;
    std::vector<int> dfs_iterative(int source) const;
    std::vector<int> bfs_distances(int source) const;   // 边数意义下的最短距离，不可达为 -1
    int connected_components(std::vector<int>* label = nullptr) const; // 仅无向图

    // ---------- 有向图 ----------
    bool topological_sort(std::vector<int>& order) const; // 有环返回 false
    bool has_cycle() const;

    // ---------- 最短路 ----------
    ShortestPaths dijkstra(int source) const;
    ShortestPaths bellman_ford(int source) const;
    std::vector<std::vector<Weight>> floyd() const;

    // ---------- 最小生成树（仅无向图） ----------
    MST kruskal() const;
    MST prim() const;

private:
    void dfs_visit(int u, std::vector<bool>& seen, std::vector<int>& out) const;

    int n_;
    bool directed_;
    std::vector<std::vector<Edge>> adj_;
    std::vector<WeightedEdge> edges_; // 按添加顺序保存原始边，Kruskal / Bellman-Ford 使用
};
