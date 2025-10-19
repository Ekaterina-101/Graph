// random_output.cpp
#include "random_generator.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

void RandomGenerator::output_results_to_file(const std::string& filename) const {
    std::ofstream output(filename);

    if (!output) {
        std::cerr << "Failed to open output file: " << filename << std::endl;
        return;
    }

    const std::size_t V = num_vertices(m_graph);
    output << "# number_of_vertices = " << V << std::endl;

    for (const auto& res : results) {
        output << std::scientific
               << "p = " << res.probability
               << " | P(connected) = " << res.p_connected
               << " | P(no AP/B & connected) = " << res.p_no_apb
               << " | P(degree <= " << m_degree << " & connected) = " << res.p_degree
               << " | P(all conditions) = " << res.p_both
               << std::endl;
    }

    output.close();
}

void RandomGenerator::print_results_to_console() const {
    for (const auto& res : results) {
        std::cout << std::fixed << std::setprecision(2)
                  << "p = " << std::setw(4) << res.probability
                  << " | P(all conditions) = " << std::scientific << res.p_both << "\n";
    }
}
