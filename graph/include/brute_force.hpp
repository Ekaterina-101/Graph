#pragma once
#include "abstract_generator.hpp"

#include <vector>

class BruteForceGenerator : public AbstractGenerator {
public:
    BruteForceGenerator(Graph graph) : AbstractGenerator(std::move(graph)) {}
    void generate_graphs() override;
    double get_prob();

private:
    bool next_combination(std::vector<int>& comb, int n);
    void print_stats(const std::vector<std::pair<std::string, DiameterStats>>& data);
    void generate_all_edges(std::vector<std::pair<int, int>>& all_edges);
    bool is_valid_graph(const Graph& graph);
    void update_statistics(const Graph& graph, int current_diameter);
    void print_detailed_statistics(const std::vector<std::pair<std::string, DiameterStats>>& data);
    void print_diameter_statistics(const std::vector<std::pair<std::string, DiameterStats>>& data);
    void output_results_to_file();
    void load_precomputed_data();
    void save_detailed_statistics_to_csv();
    void process_graphs_by_edges(const std::vector<std::pair<int, int>>& all_edges);
    void load_graphs_for_specific_cases();
};
