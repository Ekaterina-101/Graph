#include "brute_force.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <numeric>

#include "art_points_and_bridges.hpp"
#include "is_graph_connected.hpp"
#include "graph_diam.hpp"

void BruteForceGenerator::generate_graphs() {
    std::vector<std::pair<int, int>> all_edges;
    const std::size_t V = num_vertices(m_graph);
    for (std::size_t u = 0; u < V; ++u)
        for (std::size_t v = u + 1; v < V; ++v)
            all_edges.emplace_back(u, v);

    const int E = all_edges.size();
    const long long total_subgraphs = 1LL << E;
    std::vector<int> graphs_by_edges(E + 1);

    std::cout << "Vertices: " << V << std::endl;

    for (int e = 0; e <= E; ++e) {
        if (e == 0) continue;
        std::vector<int> indices(e);
        std::iota(indices.begin(), indices.end(), 0);

        do {
            Graph graph(V);
            for (int i : indices)
                add_edge(all_edges[i].first, all_edges[i].second, graph);

            if (!is_graph_connected(graph)) continue;

            int current_diameter = diameter(graph);
            stats_connected.update(current_diameter);
            bool no_apb = !has_articulation_points(graph) && !has_bridges(graph);
            bool degree_ok = all_vertices_degree_le(graph, m_degree);

            if (no_apb)
                stats_no_apb.update(current_diameter);
            if (degree_ok)
                stats_degree_le3.update(current_diameter);
            if (no_apb && degree_ok) {
                stats_all_conditions.update(current_diameter);
                graphs_by_edges[num_edges(graph)]++;
                //print_graph_properties(graph);
            }
        } while (next_combination(indices, E));
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::setw(30) << std::left << "Total subgraphs:" 
              << total_subgraphs << "\n";
    std::cout << std::setw(30) << std::left << "Connected graphs:" 
              << stats_connected.count 
              << " (" 
              << static_cast<double>(stats_connected.count) / total_subgraphs * 100.0 
              << "%)\n";
    std::cout << std::setw(30) << std::left << "Graphs without AP/B:" 
              << stats_no_apb.count
              << " (" 
              << static_cast<double>(stats_no_apb.count) / total_subgraphs * 100.0 
              << "%)\n";
    std::cout << std::setw(30) << std::left << "Graphs with degree <= " + std::to_string(m_degree) + ":" 
              << stats_degree_le3.count
              << " (" 
              << static_cast<double>(stats_degree_le3.count) / total_subgraphs * 100.0 
              << "%)\n";
    std::cout << std::setw(30) << std::left << "All conditions:" 
              << stats_all_conditions.count 
              << " (" 
              << static_cast<double>(stats_all_conditions.count) / total_subgraphs * 100.0 
              << "%)\n";

    std::cout << "\nDiameter Statistics:\n";
    print_stats({
        {"All Conditions", stats_all_conditions}
    });

    //std::cout << "\nNumber of valid graphs by edge count:\n";
    //std::cout << std::setw(10) << std::left << "Edges" << " | " << "Count\n";
    //std::cout << std::string(20, '-') << "\n";
    //for (size_t edges = 0; edges < graphs_by_edges.size(); ++edges) {
    //    std::cout << std::setw(10) << std::left << edges << " | " << graphs_by_edges[edges] << "\n";
    //}
    
    std::ofstream output("../results_2.txt");

    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    for (double p = 0.0; p <= 1.0 + 1e-8; p += 0.1)  {
        [[maybe_unused]]double sum = 0;
        for (int i = 1; i <= E; ++i) {
            sum += (static_cast<double>(graphs_by_edges[i]) * pow(p, i) * pow(1 - p, E - i));
        }
        output << std::fixed << std::setprecision(6) << p
                << "\t"
                << std::scientific << std::setprecision(15) << sum
                << "\n";
    }
}

double BruteForceGenerator::get_prob() {
    std::vector<std::pair<int, int>> all_edges;
    const std::size_t V = num_vertices(m_graph);
    for (std::size_t u = 0; u < V; ++u)
        for (std::size_t v = u + 1; v < V; ++v)
            all_edges.emplace_back(u, v);

    const int E = all_edges.size();
    std::vector<int> graphs_by_edges(E + 1);

    if (V < 8) {
        for (int e = 0; e <= E; ++e) {
            if (e == 0) continue;
            std::vector<int> indices(e);
            std::iota(indices.begin(), indices.end(), 0);

            do {
                Graph graph(V);
                for (int i : indices)
                    add_edge(all_edges[i].first, all_edges[i].second, graph);

                if (!is_graph_connected(graph)) continue;

                int current_diameter = diameter(graph);
                bool no_apb = !has_articulation_points(graph) && !has_bridges(graph);
                bool degree_ok = all_vertices_degree_le(graph, m_degree);

                if (no_apb && degree_ok) {
                    stats_all_conditions.update(current_diameter);
                    graphs_by_edges[num_edges(graph)]++;
                }
            } while (next_combination(indices, E));
        }   
    }
    else if (V == 8) {
        if (m_degree == 3) {
            std::size_t i_zag;
            std::ifstream in("../8edges_3.txt");
            for (std::size_t i = 0; i <= 28; ++i) {
                in >> i_zag >> graphs_by_edges[i];
            }
        }
        else if (m_degree == 4) {
            std::size_t i_zag;
            std::ifstream in("../8edges_4.txt");
            for (std::size_t i = 0; i <= 28; ++i) {
                in >> i_zag >> graphs_by_edges[i];
            }
        }
        else if (m_degree == 5) {
            std::size_t i_zag;
            std::ifstream in("../8edges_5.txt");
            for (std::size_t i = 0; i <= 28; ++i) {
                in >> i_zag >> graphs_by_edges[i];
                //std::cout << i_zag << " " << graphs_by_edges[i] << "\n";
            }
        }
    }
    
    double max_sum = -1;
    double max_p;

    for (double p = 0.0; p <= 1.0 + 1e-8; p += 0.1)  {
        [[maybe_unused]]double sum = 0;
        for (int i = 1; i <= E; ++i) {
            sum += (static_cast<double>(graphs_by_edges[i]) * pow(p, i) * pow(1 - p, E - i));
        }
        if (sum > max_sum) {
            max_sum = sum;
            max_p = p;
        }
    }

    return max_p;
}

bool BruteForceGenerator::next_combination(std::vector<int>& comb, int n) {
    int k = comb.size();
    for (int i = k - 1; i >= 0; i--) {
        if (comb[i] != n - k + i) {
            comb[i]++;
            for (int j = i + 1; j < k; j++)
                comb[j] = comb[j - 1] + 1;
            return true;
        }
    }
    return false;
}

void BruteForceGenerator::print_stats(const std::vector<std::pair<std::string, DiameterStats>>& data) {
    const int width_label = 25;  
    const int width_value = 15;   
    std::cout << std::setw(width_label) << std::left << "Type"
              << " | " << std::setw(width_value) << std::left << "Min    | count"
              << " | " << std::setw(width_value) << std::left << "Max    | count"
              << " | " << std::setw(width_value) << std::left << "Avg"
              << " | " << std::setw(width_value) << std::left << "Total"
              << "\n";
    std::cout << std::string(90, '-') << "\n";
    for (const auto& [label, stats] : data) {
        std::cout << std::setw(width_label) << std::left << label
                  << " | (" << std::setw(5) << std::left << stats.min 
                  << " | " << std::setw(5) << std::left << stats.count_min << ")"
                  << " | (" << std::setw(5) << std::left << stats.max 
                  << " | " << std::setw(5) << std::left << stats.count_max << ")"
                  << " | " << std::setw(width_value) << std::left 
                  << stats.average()
                  << " | " << std::setw(width_value) << std::left 
                  << stats.count
                  << "\n";
    }

    std::cout << "\n" << std::setw(width_label) << std::left << "Type"
              << " | " << std::setw(width_value) << std::left << "Diameter"
              << " | " << std::setw(width_value) << std::left << "Count"
              << "\n";
    std::cout << std::string(90, '-') << "\n";

    for (const auto& [label, stats] : data) {
        for (const auto& [diameter, count] : stats.diameter_counts) {
            std::cout << std::setw(width_label) << std::left << label
                      << " | " << std::setw(width_value) << diameter
                      << " | " << std::setw(width_value) << count
                      << "\n";
        }
    }
}

//int main() {
//    Graph g(7);
//    BruteForceGenerator generator(g);
//    generator.generate_graphs();
//}