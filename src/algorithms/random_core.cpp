// random_core.cpp
#include "random_generator.hpp"
#include <random>
#include <cmath>

#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"

// Методы класса RandomGenerator, которые относятся к самому алгоритму.
void RandomGenerator::run_trials() {
    std::mt19937 generator(std::random_device{}());

    for (double p = PROB_MIN; p <= PROB_MAX + 1e-8; p += PROB_STEP) {
        stats_connected.clear();
        stats_no_apb.clear();
        stats_degree.clear();
        stats_all_conditions.clear();

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
            bool degree_ok = all_vertices_degree_less(graph, m_degree); // Убедитесь, что используете правильное имя функции
            int diam = diameter(graph);
            stats_connected.update(diam);

            if (no_apb) {
                stats_no_apb.update(diam);
            }
            if (degree_ok) {
                stats_degree.update(diam);
            }
            if (no_apb && degree_ok) {
                stats_all_conditions.update(diam);
            }
        }

        // Вычисление вероятностей
        double p_connected, p_no_apb, p_degree, p_both;
        if (!COND_PROB) {
            p_connected = static_cast<double>(stats_connected.count) / NUMBER_OF_TRIALS;
            p_no_apb = static_cast<double>(stats_no_apb.count) / NUMBER_OF_TRIALS;
            p_degree = static_cast<double>(stats_degree.count) / NUMBER_OF_TRIALS;
            p_both = static_cast<double>(stats_all_conditions.count) / NUMBER_OF_TRIALS;
        } else {
            p_connected = static_cast<double>(stats_connected.count) / NUMBER_OF_TRIALS;
            p_no_apb = static_cast<double>(stats_no_apb.count) / stats_connected.count;
            p_degree = static_cast<double>(stats_degree.count) / stats_connected.count;
            p_both = static_cast<double>(stats_all_conditions.count) / stats_connected.count;
        }

        // Сохраняем результаты для текущего p
        TrialResult result;
        result.probability = p;
        result.p_connected = p_connected;
        result.p_no_apb = p_no_apb;
        result.p_degree = p_degree;
        result.p_both = p_both;
        results.push_back(result);
    }
}

// Определение статических членов
std::size_t RandomGenerator::NUMBER_OF_TRIALS = 10000;
double RandomGenerator::PROB_STEP = 0.1;
double RandomGenerator::PROB_MIN = 0.0;
double RandomGenerator::PROB_MAX = 1.0;
bool RandomGenerator::COND_PROB = false;
