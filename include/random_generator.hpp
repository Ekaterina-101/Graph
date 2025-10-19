#pragma once  

#include "abstract_generator.hpp"

struct TrialResult {
    double probability;
    double p_connected;
    double p_no_apb;
    double p_degree;
    double p_both;
};

class RandomGenerator : public AbstractGenerator {
public:
    static std::size_t NUMBER_OF_TRIALS;
    static double PROB_STEP;
    static double PROB_MIN;
    static double PROB_MAX;
    static bool COND_PROB;

    explicit RandomGenerator(Graph graph)
        : AbstractGenerator(std::move(graph)) {}

    void generate_graphs() override;
    void run_trials();
    void output_results_to_file(const std::string& filename) const;
    void print_results_to_console() const;
    std::vector<TrialResult> results;
};
