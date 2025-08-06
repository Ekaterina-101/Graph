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

void BruteForceGenerator::generate_graphs() {
    //const std::size_t V = num_vertices(m_graph);
    std::vector<std::pair<int, int>> all_edges;
    generate_all_edges(all_edges);

    const int E = all_edges.size();
    const long long total_subgraphs = 1LL << E;
    
    std::cout << "Vertices: " << num_vertices(m_graph) << "\n"
              << "Total subgraphs: " << total_subgraphs << std::endl;

    // Для больших графов используем предподсчитанные данные
    //if (V >= 7) {
    //    load_precomputed_data();
    //    //std::cout << "Using precomputed data for " << V << " vertices\n";
    //    
    //    // Выводим статистику из предподсчитанных данных
    //    std::cout << std::fixed << std::setprecision(2);
    //    print_diameter_statistics({
    //        {"All Conditions", stats_all_conditions}
    //    });
    //    
    //    save_detailed_statistics_to_csv();
    //    output_results_to_file();
    //    return;
    //}

    for (int i = 0; i <= E; ++i) {
        stats_connected.edge_counts[i] = 0;
        stats_no_apb.edge_counts[i] = 0;
        stats_degree_le3.edge_counts[i] = 0;
        stats_all_conditions.edge_counts[i] = 0;
    }

    process_graphs_by_edges(all_edges);
    
    std::cout << std::fixed << std::setprecision(2);
    print_diameter_statistics({
        {"Connected", stats_connected},
        {"No AP/B", stats_no_apb},
        {"Degree <= " + std::to_string(m_degree), stats_degree_le3},
        {"All Conditions", stats_all_conditions}
    });

    print_detailed_statistics({
        //{"Connected", stats_connected},
        //{"No AP/B", stats_no_apb},
        //{"Degree <= " + std::to_string(m_degree), stats_degree_le3},
        {"All Conditions", stats_all_conditions}
    });

    //for (size_t edges = 0; edges < stats_all_conditions.edge_counts.size(); ++edges) {
    //    std::cout << std::setw(10) << std::left << edges << " | " << stats_all_conditions.edge_counts[edges] << "\n";
    //}
    output_results_to_file();
    save_detailed_statistics_to_csv();
}

void BruteForceGenerator::generate_all_edges(std::vector<std::pair<int, int>>& all_edges) {
    const std::size_t V = num_vertices(m_graph);
    for (std::size_t u = 0; u < V; ++u)
        for (std::size_t v = u + 1; v < V; ++v)
            all_edges.emplace_back(u, v);
}

void BruteForceGenerator::process_graphs_by_edges(const std::vector<std::pair<int, int>>& all_edges) {
    const int E = all_edges.size();
    for (int e = 0; e <= E; ++e) {
        if (e == 0) continue;

        std::vector<int> indices(e);
        std::iota(indices.begin(), indices.end(), 0);

        do {
            Graph graph(num_vertices(m_graph));
            for (int i : indices)
                add_edge(all_edges[i].first, all_edges[i].second, graph);

            if (!is_graph_connected(graph)) continue;

            int current_diameter = diameter(graph);
            update_statistics(graph, current_diameter);
        } while (next_combination(indices, E));
    }
}

void BruteForceGenerator::update_statistics(const Graph& graph, int current_diameter) {
    int edges = num_edges(graph);
    stats_connected.update(current_diameter, edges);
    bool no_apb = !has_articulation_points(graph) && !has_bridges(graph);
    bool degree_ok = all_vertices_degree_le(graph, m_degree);

    if (no_apb)
        stats_no_apb.update(current_diameter, edges);
    if (degree_ok)
        stats_degree_le3.update(current_diameter, edges);
    if (no_apb && degree_ok) {
        stats_all_conditions.update(current_diameter, edges);
        //if (edges == 10)
            //print_graph_properties(graph);
    }
}

void BruteForceGenerator::load_precomputed_data() {
    const std::size_t V = num_vertices(m_graph);
    const std::string base_path = "../data/precomputed/";

    std::ostringstream edge_filename_stream;
    edge_filename_stream << base_path
                        << "v" << V
                        << "-d" << m_degree
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

    // Загружаем детальную статистику (диаметр + рёбра)
    std::ostringstream detail_filename_stream;
    detail_filename_stream << base_path
                          << "v" << V
                          << "-d" << m_degree
                          << "-detailed.txt";

    std::ifstream detail_file(detail_filename_stream.str());
    if (detail_file) {
        std::string type;
        int diameter, edges, count;
        while (detail_file >> type >> diameter >> edges >> count) {
            // Заполняем статистику для соответствующего типа
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

void BruteForceGenerator::save_detailed_statistics_to_csv() {
    std::ostringstream filename_stream;
    filename_stream << "../data/results/v" 
                    << num_vertices(m_graph) 
                    << "-d" << m_degree  
                    << "-detailedStats"<< ".csv";
    
    std::ofstream csv_file(filename_stream.str());
    if (!csv_file) {
        std::cerr << "Failed to open detailed stats CSV file: " << filename_stream.str() << "\n";
        return;
    }

    csv_file << "Type,Diameter,Edges,Count\n";

    const std::vector<std::pair<std::string, DiameterStats>> stats_list = {
        {"Connected", stats_connected},
        {"No AP/B & connected", stats_no_apb},
        {"Degree <= " + std::to_string(m_degree) + " & connected", stats_degree_le3},
        {"All Conditions", stats_all_conditions}
    };

    for (const auto& [label, stats] : stats_list) {
        for (const auto& [diameter, edge_map] : stats.diameter_edge_counts) {
            for (const auto& [edges, count] : edge_map) {
                csv_file << "\"" << label << "\"," 
                        << diameter << "," 
                        << edges << "," 
                        << count << "\n";
            }
        }
    }

    csv_file.close();
    //std::cout << "Detailed statistics saved to " << filename_stream.str() << "\n";
}

void BruteForceGenerator::print_detailed_statistics(const std::vector<std::pair<std::string, DiameterStats>>& data) {
    const int width_label = 20;
    //const int width_diam = 10;
    const int width_edges = 10;
    const int width_count = 15;

    for (const auto& [label, stats] : data) {
        std::cout << "\nDetailed stats for: " << label << "\n";
        std::cout << std::setw(width_label) << std::left << "Diameter"
                  << " | " << std::setw(width_edges) << std::left << "Edges"
                  << " | " << std::setw(width_count) << std::left << "Count"
                  << "\n";
        std::cout << std::string(50, '-') << "\n";

        for (const auto& [diameter, edge_map] : stats.diameter_edge_counts) {
            for (const auto& [edges, count] : edge_map) {
                std::cout << std::setw(width_label) << std::left << diameter
                          << " | " << std::setw(width_edges) << std::left << edges
                          << " | " << std::setw(width_count) << std::left << count
                          << "\n";
            }
        }
    }
}

void BruteForceGenerator::print_diameter_statistics(const std::vector<std::pair<std::string, DiameterStats>>& data) {
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

void BruteForceGenerator::output_results_to_file() {
    std::ostringstream filename_stream;
    filename_stream << "../data/results/v" 
                    << num_vertices(m_graph) 
                    << "-d" << m_degree  
                    << "-resultsFormula"<< ".csv";
    
    std::ofstream output(filename_stream.str());

    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    output.imbue(std::locale("C"));

    const int E = stats_all_conditions.edge_counts.size() - 1;
    
    output << "p,Connected,No AP/B & connected,Degree <= " << m_degree << " & connected,All Conditions\n";

    for (double p = 0.0; p <= 1.0 + 1e-8; p += 0.01) {
        double sum_connected = 0;
        double sum_no_apb = 0;
        double sum_degree_le = 0;
        double sum_all_conditions = 0;

        for (int i = 1; i <= E; ++i) {
            sum_connected += (static_cast<double>(stats_connected.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
            sum_no_apb += (static_cast<double>(stats_no_apb.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
            sum_degree_le += (static_cast<double>(stats_degree_le3.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
            sum_all_conditions += (static_cast<double>(stats_all_conditions.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
        }

        // Форматируем вывод в CSV стиле (разделитель запятая, точка как десятичный разделитель)
        output << std::fixed << std::setprecision(6) << p
               << "," << std::scientific << std::setprecision(6) << sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_no_apb / sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_degree_le / sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_all_conditions / sum_connected
               << "\n";
    }
    
    output.close();
}

void BruteForceGenerator::load_graphs_for_specific_cases() {
    const std::size_t V = num_vertices(m_graph);
    const std::string base_path = "../data/precomputed/";

    std::ostringstream filename_stream;
    filename_stream << base_path
                    << "v" << V
                    << "-d" << m_degree
                    << "-allConditions-byEdges.txt";

    const std::string filename = filename_stream.str();

    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: Unable to open precomputed file: " << filename << std::endl;
        return;
    }

    std::size_t edge_count, graph_count;
    while (in >> edge_count >> graph_count) {
        //if (edge_count < stats_all_conditions.edge_counts.size()) {
        stats_all_conditions.edge_counts[edge_count] = graph_count;
    }
}

double BruteForceGenerator::get_prob() {
    std::vector<std::pair<int, int>> all_edges;
    generate_all_edges(all_edges);

    const int E = all_edges.size();

    if (num_vertices(m_graph) < 8) {
        process_graphs_by_edges(all_edges);
    }
    else {
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
