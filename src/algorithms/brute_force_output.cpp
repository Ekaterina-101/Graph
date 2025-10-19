// brute_force_output.cpp
#include "brute_force.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

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

void BruteForceGenerator::print_detailed_statistics(const std::vector<std::pair<std::string, DiameterStats>>& data) {
    const int width_label = 20;
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

void BruteForceGenerator::save_detailed_statistics_to_csv() {
    std::ostringstream filename_stream;
    filename_stream << "../data/results/v" 
                    << num_vertices(m_graph) 
                    << "-d" << m_max_degree  
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
        {"Degree <= " + std::to_string(m_max_degree) + " & connected", stats_degree},
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
}

void BruteForceGenerator::output_results_to_file() {
    std::ostringstream filename_stream;
    filename_stream << "../data/results/v" 
                    << num_vertices(m_graph) 
                    << "-d" << m_max_degree  
                    << "-resultsFormula"<< ".csv";
    
    std::ofstream output(filename_stream.str());

    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    output.imbue(std::locale("C"));

    const int E = stats_all_conditions.edge_counts.size() - 1;
    
    output << "p,Connected,No AP/B & connected,Degree <= " << m_max_degree << " & connected,All Conditions\n";

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
            sum_degree_le += (static_cast<double>(stats_degree.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
            sum_all_conditions += (static_cast<double>(stats_all_conditions.edge_counts[i]) 
                          * pow(p, i) * pow(1 - p, E - i));
        }

        output << std::fixed << std::setprecision(6) << p
               << "," << std::scientific << std::setprecision(6) << sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_no_apb / sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_degree_le / sum_connected
               << "," << std::scientific << std::setprecision(6) << sum_all_conditions / sum_connected
               << "\n";
    }
    
    output.close();
}
