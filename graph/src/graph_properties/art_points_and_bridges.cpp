#include <iostream>
#include <vector>
#include <set>
#include <map>

#include "art_points_and_bridges.hpp"

using namespace boost;

class APAndBDFS {
public:
    explicit APAndBDFS(const Graph& g) : g(g), timer(0) {
        int n = num_vertices(g);
        tin.resize(n, -1);
        low.resize(n, -1);
        used.resize(n, false);
        parent.resize(n, graph_traits<Graph>::null_vertex());
    }

    void run() {
        for (auto u : boost::make_iterator_range(vertices(g))) {
            if (!used[u]) {
                root = u;
                root_children = 0;
                dfs(u);
                if (root_children > 1) {
                    articulation_points.insert(root);
                }
            }
        }
    }

    const std::set<Vertex>& get_articulation_points() const {
        return articulation_points;
    }

    const std::set<Edge>& get_bridges() const {
        return bridges;
    }

private:
    void dfs(Vertex u) {
        used[u] = true;
        tin[u] = low[u] = timer++;
        for (auto e : boost::make_iterator_range(out_edges(u, g))) {
            Vertex v = target(e, g);
            if (v == u) v = source(e, g); 

            if (v == parent[u]) continue;

            if (used[v]) {
                low[u] = std::min(low[u], tin[v]);
            } else {
                parent[v] = u;
                if (u == root) root_children++;

                dfs(v);
                low[u] = std::min(low[u], low[v]);

                if (low[v] > tin[u]) {
                    bridges.insert(e);
                }

                if (u != root && low[v] >= tin[u]) {
                    articulation_points.insert(u);
                }
            }
        }
    }

    const Graph& g;
    std::vector<int> tin, low;
    std::vector<bool> used;
    std::vector<Vertex> parent;
    std::set<Vertex> articulation_points;
    std::set<Edge> bridges;
    Vertex root;
    int root_children;
    int timer;
};

bool has_articulation_points(const Graph& g) {
    APAndBDFS dfs(g);
    dfs.run();
    return !dfs.get_articulation_points().empty();
}

bool has_bridges(const Graph& g) {
    APAndBDFS dfs(g);
    dfs.run();
    return !dfs.get_bridges().empty();
}
