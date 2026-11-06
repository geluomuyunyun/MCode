#include <vector>

#include "Graph.h"
#include "check.h"

using V = std::vector<int>;

static void test_traversal()
{
    TEST_SECTION("BFS / DFS");
    //   0 - 1 - 3
    //   |   |
    //   2 - 4   5 - 6
    Graph g(7, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(1, 4);
    g.add_edge(2, 4);
    g.add_edge(5, 6);
    CHECK(g.bfs(0) == (V{0, 1, 2, 3, 4}));
    CHECK(g.dfs(0) == (V{0, 1, 3, 4, 2}));
    CHECK(g.dfs_iterative(0) == g.dfs(0));
    CHECK(g.bfs_distances(0) == (V{0, 1, 1, 2, 2, -1, -1}));

    TEST_SECTION("connected components / cycle");
    V label;
    CHECK(g.connected_components(&label) == 2);
    CHECK(label[0] == label[4] && label[5] == label[6] && label[0] != label[5]);
    CHECK(g.has_cycle()); // 0-1-4-2-0

    Graph tree(4, false);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    CHECK(!tree.has_cycle());

    TEST_SECTION("adjacency matrix");
    auto m = tree.adjacency_matrix();
    CHECK(m[0][1] == 1 && m[1][0] == 1 && m[0][2] == Graph::INF && m[2][2] == 0);
}

static void test_dag()
{
    TEST_SECTION("topological sort");
    // 课程依赖：0->1, 0->2, 1->3, 2->3, 3->4
    Graph dag(5, true);
    dag.add_edge(0, 1);
    dag.add_edge(0, 2);
    dag.add_edge(1, 3);
    dag.add_edge(2, 3);
    dag.add_edge(3, 4);
    V order;
    CHECK(dag.topological_sort(order));
    CHECK(order == (V{0, 1, 2, 3, 4}));
    CHECK(!dag.has_cycle());
    dag.add_edge(4, 1); // 1->3->4->1 成环
    CHECK(!dag.topological_sort(order));
    CHECK(dag.has_cycle());
}

static void test_shortest_paths()
{
    TEST_SECTION("Dijkstra");
    Graph g(6, true);
    g.add_edge(0, 1, 7);
    g.add_edge(0, 2, 9);
    g.add_edge(0, 5, 14);
    g.add_edge(1, 2, 10);
    g.add_edge(1, 3, 15);
    g.add_edge(2, 3, 11);
    g.add_edge(2, 5, 2);
    g.add_edge(3, 4, 6);
    g.add_edge(5, 4, 9);
    auto sp = g.dijkstra(0);
    CHECK(sp.dist == (std::vector<long long>{0, 7, 9, 20, 20, 11}));
    CHECK(sp.path_to(4) == (V{0, 2, 5, 4}));
    CHECK(sp.path_to(0) == (V{0}));

    TEST_SECTION("Bellman-Ford (negative edges)");
    auto bf = g.bellman_ford(0);
    CHECK(bf.dist == sp.dist && !bf.negative_cycle);
    Graph neg(4, true);
    neg.add_edge(0, 1, 4);
    neg.add_edge(0, 2, 5);
    neg.add_edge(2, 1, -3);
    neg.add_edge(1, 3, 2);
    auto nb = neg.bellman_ford(0);
    CHECK(nb.dist == (std::vector<long long>{0, 2, 5, 4}) && !nb.negative_cycle);
    CHECK(nb.path_to(3) == (V{0, 2, 1, 3}));
    neg.add_edge(3, 2, -10); // 2->1->3->2 权和 -11
    CHECK(neg.bellman_ford(0).negative_cycle);

    TEST_SECTION("Floyd");
    auto d = g.floyd();
    CHECK(d[0][4] == 20 && d[2][4] == 11 && d[4][0] == Graph::INF && d[3][3] == 0);
    for (int v = 0; v < 6; ++v)
        CHECK(d[0][v] == sp.dist[v]);

    TEST_SECTION("unreachable");
    Graph iso(3, true);
    iso.add_edge(0, 1, 1);
    auto r = iso.dijkstra(0);
    CHECK(r.dist[2] == Graph::INF && r.path_to(2).empty());
}

static void test_mst()
{
    TEST_SECTION("Kruskal / Prim");
    Graph g(7, false);
    g.add_edge(0, 1, 7);
    g.add_edge(0, 3, 5);
    g.add_edge(1, 2, 8);
    g.add_edge(1, 3, 9);
    g.add_edge(1, 4, 7);
    g.add_edge(2, 4, 5);
    g.add_edge(3, 4, 15);
    g.add_edge(3, 5, 6);
    g.add_edge(4, 5, 8);
    g.add_edge(4, 6, 9);
    g.add_edge(5, 6, 11);
    auto k = g.kruskal();
    auto p = g.prim();
    CHECK(k.total == 39 && p.total == 39);
    CHECK(k.edges.size() == 6 && p.edges.size() == 6);
    CHECK(k.connected && p.connected);

    Graph forest(4, false);
    forest.add_edge(0, 1, 3);
    forest.add_edge(2, 3, 4);
    CHECK(!forest.kruskal().connected && forest.kruskal().total == 7);
    CHECK(!forest.prim().connected && forest.prim().total == 7);
}

int main()
{
    test_traversal();
    test_dag();
    test_shortest_paths();
    test_mst();
    return CHECK_REPORT();
}
