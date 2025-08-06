#include <random>
#include <cmath>
#include <iomanip>
#include <fstream>

#include "random_generator.hpp"
#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"

void RandomGenerator::generate_graphs() {
    std::ofstream output("../results.txt");

    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    const std::size_t V = num_vertices(m_graph);
    output << "# number_of_vertices = " << V << std::endl;
    std::cout << "Vertices: " << V << std::endl;
    //print_graph_properties(m_graph);
    std::mt19937 generator(std::random_device{}());

    for (double p = PROB_MIN; p <= PROB_MAX + 1e-8; p += PROB_STEP) {

        stats_connected.clear(); stats_no_apb.clear();
        stats_degree_le3.clear(); stats_all_conditions.clear();

        for (std::size_t trial = 0; trial < NUMBER_OF_TRIALS; ++trial) {
            Graph graph = m_graph;
            auto edge_range = edges(graph);
            std::bernoulli_distribution dist(p);
            
            std::vector<Graph::edge_descriptor> to_remove;
            for (auto it = edge_range.first; it != edge_range.second; ++it) {
                if (!dist(generator)) {
                    to_remove.push_back(*it);
                }
            }

            for (const auto& e : to_remove) {
                remove_edge(e, graph);
            }

            if (!is_graph_connected(graph)) continue;

            bool no_apb = !has_articulation_points(graph) && !has_bridges(graph);
            bool degree_ok = all_vertices_degree_le(graph, m_degree);
            int diam = diameter(graph);
            stats_connected.update(diam);

            if (no_apb) {
                stats_no_apb.update(diam);
            }
            if (degree_ok) {
                stats_degree_le3.update(diam);
            }
            if (no_apb && degree_ok) {
                stats_all_conditions.update(diam);
            }
        }

        double p_connected, p_no_apb, p_degree, p_both;
        //std::cout << stats_connected.count << "\n";
        if (!COND_PROB) {
            p_connected = static_cast<double>(stats_connected.count) / NUMBER_OF_TRIALS;
            p_no_apb = static_cast<double>(stats_no_apb.count) / NUMBER_OF_TRIALS;
            p_degree = static_cast<double>(stats_degree_le3.count) / NUMBER_OF_TRIALS;
            p_both = static_cast<double>(stats_all_conditions.count) / NUMBER_OF_TRIALS;
        }
        else {
            p_connected = static_cast<double>(stats_connected.count) / NUMBER_OF_TRIALS;
            p_no_apb = static_cast<double>(stats_no_apb.count) / stats_connected.count;
            p_degree = static_cast<double>(stats_degree_le3.count) / stats_connected.count;
            p_both = static_cast<double>(stats_all_conditions.count) / stats_connected.count;
        }
        
        std::cout << std::fixed << std::setprecision(2)
                  << "p = " << std::setw(4) << p
                  << " | P(all conditions) = " << std::scientific << p_both << "\n";
        output << std::scientific
               << "p = " << p
               << " | P(connected) = " << p_connected
               << " | P(no AP/B & connected) = " << p_no_apb
               << " | P(degree <= " << m_degree << " & connected) = " << p_degree
               << " | P(all conditions) = " << p_both
               << std::endl;
    }

    output.close();
}

std::size_t RandomGenerator::NUMBER_OF_TRIALS = 10000;
double RandomGenerator::PROB_STEP = 0.1;
double RandomGenerator::PROB_MIN = 0.0;
double RandomGenerator::PROB_MAX = 1.0;
bool RandomGenerator::COND_PROB = false;

//int main() {
//    Graph g(8);
//    RandomGenerator generator(g);
//    generator.generate_graphs();
//}
