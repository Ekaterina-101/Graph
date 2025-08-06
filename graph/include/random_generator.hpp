#pragma once  

#include "abstract_generator.hpp"

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
};
