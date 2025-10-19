// random_generator.cpp (или random_main.cpp)
#include "random_generator.hpp"
#include <iostream>

void RandomGenerator::generate_graphs() {
    const std::size_t V = num_vertices(m_graph);
    std::cout << "Vertices: " << V << std::endl;

    run_trials();

    print_results_to_console();
    output_results_to_file("../results.txt");
}
