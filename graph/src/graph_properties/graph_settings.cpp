#include <iostream>
#include <vector>

#include "graph_settings.hpp"
#include "graph_diam.hpp"

void print_graph_properties(const Graph& g) {
    std::cout << "Vertices: " << boost::num_vertices(g) << std::endl;
    std::cout << "Edges: " << boost::num_edges(g) << std::endl;
    std::cout << "Diameter: " << diameter(g) << std::endl;

    std::cout << "Edges in the graph:\n";
    Graph::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
        std::cout << source(*ei, g) << " -- " << target(*ei, g) << std::endl;
    }
}

Graph matrix_to_boost_graph(const std::vector<std::vector<int>>& adj_matrix) {
    int n = adj_matrix.size();
    
    Graph g(n);
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (adj_matrix[i][j] == 1) {
                boost::add_edge(i, j, g);
            }
        }
    }
    
    return g;
}
