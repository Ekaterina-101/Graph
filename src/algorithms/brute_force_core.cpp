// brute_force_core.cpp
#include "brute_force.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

#include "art_points_and_bridges.hpp"
#include "is_graph_connected.hpp"
#include "graph_diam.hpp"


void BruteForceGenerator::generate_all_edges(
    std::vector<std::pair<int, int>>& all_edges) {

    const std::size_t V = num_vertices(m_graph);

    for (std::size_t u = 0; u < V; ++u) {
        for (std::size_t v = u + 1; v < V; ++v) {
            all_edges.emplace_back(u, v);
        }
    }
}

bool BruteForceGenerator::next_combination(std::vector<int>& comb, int n) {
    int k = comb.size();
    for (int i = k - 1; i >= 0; i--) {
        if (comb[i] != n - k + i) {
            comb[i]++;
            for (int j = i + 1; j < k; j++) {
                comb[j] = comb[j - 1] + 1;
            }

            return true;
        }
    }

    return false;
}

void BruteForceGenerator::process_graphs_by_edges(
    const std::vector<std::pair<int, int>>& all_edges) {

    const int E = all_edges.size();
    for (int e = 0; e <= E; ++e) {
        if (e == 0) continue;

        std::vector<int> indices(e);
        std::iota(indices.begin(), indices.end(), 0);

        do {
            Graph graph(num_vertices(m_graph));

            for (int i : indices) {
                add_edge(all_edges[i].first, all_edges[i].second, graph);
            }

            if (!is_graph_connected(graph)) continue;

            int current_diameter = diameter(graph);

            update_statistics(graph, current_diameter);
        } while (next_combination(indices, E));
    }
}

void BruteForceGenerator::update_statistics(
    const Graph& graph, int current_diameter) {

    int edges = num_edges(graph);
    stats_connected.update(current_diameter, edges);

    bool no_apb = !has_articulation_points(graph) && !has_bridges(graph);
    bool degree_ok = all_vertices_degree_less(graph, m_max_degree) &&
        all_vertices_degree_greater(graph, m_min_degree);

    if (no_apb) {
        stats_no_apb.update(current_diameter, edges);
    }

    if (degree_ok) {
        stats_degree.update(current_diameter, edges);
    }

    if (no_apb && degree_ok) {
        stats_all_conditions.update(current_diameter, edges);
    }
}

void BruteForceGenerator::load_precomputed_data() {
    const std::size_t V = num_vertices(m_graph);
    const std::string base_path = "../data/precomputed/";

    std::ostringstream edge_filename_stream;
    edge_filename_stream << base_path
                        << "v" << V
                        << "-d" << m_max_degree
                        << "-allConditions-byEdges.txt";

    std::ifstream edge_file(edge_filename_stream.str());
    if (edge_file) {
        std::size_t edge_count, graph_count;

        while (edge_file >> edge_count >> graph_count) {
            stats_all_conditions.edge_counts[edge_count] = graph_count;
        }

        edge_file.close();
    } else {
        std::cerr << "Warning: Edge count file not found: " << edge_filename_stream.str() << std::endl;
    }

    std::ostringstream detail_filename_stream;
    detail_filename_stream << base_path
                          << "v" << V
                          << "-d" << m_max_degree
                          << "-detailed.txt";

    std::ifstream detail_file(detail_filename_stream.str());
    if (detail_file) {
        std::string type;

        int diameter, edges, count;
        while (detail_file >> type >> diameter >> edges >> count) {
            if (type == "AllConditions") {
                stats_all_conditions.diameter_edge_counts[diameter][edges] = count;
                stats_all_conditions.diameter_counts[diameter] += count;
                stats_all_conditions.sum += static_cast<long long>(diameter) * count;
                stats_all_conditions.count += count;
                
                if (diameter < stats_all_conditions.min) {
                    stats_all_conditions.min = diameter;
                }

                if (diameter > stats_all_conditions.max) {
                    stats_all_conditions.max = diameter;
                }
            }
        }
        detail_file.close();
    } else {
        std::cerr << "Warning: Detailed stats file not found: " << detail_filename_stream.str() << std::endl;
    }
}

void BruteForceGenerator::load_graphs_for_specific_cases() {
    const std::size_t V = num_vertices(m_graph);
    const std::string base_path = "../data/precomputed/";

    std::ostringstream filename_stream;
    filename_stream << base_path
                    << "v" << V
                    << "-d" << m_max_degree
                    << "-allConditions-byEdges.txt";

    const std::string filename = filename_stream.str();

    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: Unable to open precomputed file: " << filename << std::endl;
        return;
    }

    std::size_t edge_count, graph_count;
    while (in >> edge_count >> graph_count) {
        stats_all_conditions.edge_counts[edge_count] = graph_count;
    }
}

double BruteForceGenerator::get_prob() {
    std::vector<std::pair<int, int>> all_edges;
    generate_all_edges(all_edges);

    const int E = all_edges.size();

    if (num_vertices(m_graph) < 8) {
        process_graphs_by_edges(all_edges);
    } else {
        load_graphs_for_specific_cases();
    }

    double max_sum = -1;
    double max_p;

    for (double p = 0.0; p <= 1.0 + 1e-8; p += 0.1)  {
        double sum = 0;
        for (int i = 1; i <= E; ++i) {
            sum += (static_cast<double>(stats_all_conditions.edge_counts[i]) * pow(p, i) * pow(1 - p, E - i));
        }

        if (sum > max_sum) {
            max_sum = sum;
            max_p = p;
        }
    }

    return max_p;
}
