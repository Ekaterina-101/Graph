// brute_force.cpp
#include "brute_force.hpp"
#include <iostream>
#include <iomanip>

void BruteForceGenerator::generate_graphs() {
    //const std::size_t V = num_vertices(m_graph);
    std::vector<std::pair<int, int>> all_edges;
    generate_all_edges(all_edges);

    const int E = all_edges.size();
    const long long total_subgraphs = 1LL << E;
    
    std::cout << "Vertices: " << num_vertices(m_graph) << "\n"
              << "Total subgraphs: " << total_subgraphs << std::endl;

    for (int i = 0; i <= E; ++i) {
        stats_connected.edge_counts[i] = 0;
        stats_no_apb.edge_counts[i] = 0;
        stats_degree.edge_counts[i] = 0;
        stats_all_conditions.edge_counts[i] = 0;
    }

    process_graphs_by_edges(all_edges);
    
    std::cout << std::fixed << std::setprecision(2);
    print_diameter_statistics({
        {"Connected", stats_connected},
        {"No AP/B", stats_no_apb},
        {"Degree <= " + std::to_string(m_degree), stats_degree},
        {"All Conditions", stats_all_conditions}
    });

    print_detailed_statistics({
        {"All Conditions", stats_all_conditions}
    });

    output_results_to_file();
    save_detailed_statistics_to_csv();
}