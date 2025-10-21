#include <vector>
#include <boost/graph/breadth_first_search.hpp>

#include "graph_diam.hpp"

using namespace boost;

int diameter(const Graph& g) {
    const int V = num_vertices(g);
    if (V == 0) return 0;

    std::vector<int> distances(V);
    int max_distance = 0;

    for (int i = 0; i < V; ++i) {
        std::fill(distances.begin(), distances.end(), -1);
        distances[i] = 0;

        breadth_first_search(
            g,
            i,
            visitor(make_bfs_visitor(
                record_distances(
                    make_iterator_property_map(
                        distances.begin(),
                        get(vertex_index, g)
                    ),
                    on_tree_edge()
                )
            ))
        );

        int local_max = *std::max_element(distances.begin(), distances.end());
        if (local_max > max_distance) {
            max_distance = local_max;
        }
    }

    return max_distance;
}

bool all_vertices_degree_less(const Graph& g, std::size_t degree) {
    for (auto [v_it, v_end] = vertices(g); v_it != v_end; ++v_it) {
        if (out_degree(*v_it, g) > degree) {
            return false;
        }
    }

    return true;
}

bool all_vertices_degree_greater(const Graph& g, std::size_t degree) {
    for (auto [v_it, v_end] = vertices(g); v_it != v_end; ++v_it) {
        if (out_degree(*v_it, g) < degree) {
            return false;
        }
    }

    return true;
}
