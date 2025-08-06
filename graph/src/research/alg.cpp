#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"

using namespace boost;

std::vector<std::vector<int>> generate_single_graph(int n, double p, std::mt19937& gen) {
    int x = static_cast<int>(std::ceil(n * p));
    
    std::vector<std::vector<int>> adj_matrix(n, std::vector<int>(n, 0));
    
    for (int i = 0; i < n; ++i) {
        std::vector<int> right_part;
        for (int j = i + 1; j < n; ++j) {
            right_part.push_back(j);
        }

        std::shuffle(right_part.begin(), right_part.end(), gen);
        
        int sum_left = 0;
        for (int j = 0; j < i; ++j) {
            sum_left += adj_matrix[i][j]; 
        }

        int needed = x - sum_left;
        
        int to_add = std::min(needed, static_cast<int>(right_part.size()));
        for (int k = 0; k < to_add; ++k) {
            int neighbor = right_part[k];
            adj_matrix[i][neighbor] = 1;
            adj_matrix[neighbor][i] = 1;
        }
    }
    
    //for (int i = 0; i < n; ++i) {
    //    int count = 0;
    //    for (int j = 0; j < n; ++j) {
    //        if (adj_matrix[i][j] == 1) {
    //            count++;
    //        }
    //    }
    //    if (count < x) {
    //        return true; 
    //    }
    //}
    
    return adj_matrix;
}

void generate_graphs_with_steps(int n) {
    const int NUMBER_OF_TRIALS = 100;
    const double PROB_MIN = 0.0;
    const double PROB_MAX = 1.0;
    const double PROB_STEP = 0.1;
    const int degree = 3; 
    
    std::ofstream output("../results_regular_graph.txt");
    
    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }
    
    output << "probability,P_red,P_blue\n";
    std::cout << "Vertices: " << n << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    bool flag = false;

    for (double p = PROB_MIN; p <= PROB_MAX + 1e-8; p += PROB_STEP) {
        DiameterStats stats_red_all;
        int count_with_valid_subgraph = 0;

        std::vector<std::vector<int>> weights_per_row(n, std::vector<int>(0)); // Храним веса строк
        std::vector<double> all_avg_weights; // Для подсчёта общего среднего

        for (int trial = 0; trial < NUMBER_OF_TRIALS; ++trial) {
            auto adj_matrix = generate_single_graph(n, p, gen);
            Graph g = matrix_to_boost_graph(adj_matrix);

            if (!is_graph_connected(g)) continue;

            bool no_apb = !has_articulation_points(g) && !has_bridges(g);
            bool degree_ok = all_vertices_degree_le(g, degree);
            int diam = diameter(g);

            if (no_apb && degree_ok) { 
                stats_red_all.update(diam); 
                if (!flag && p == 0.2) {
                    print_graph_properties(g);
                    flag = true;
                }
            }

            // Подсчитываем вес каждой строки (количество единиц)
            std::vector<int> row_weights(n, 0);
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    row_weights[i] += adj_matrix[i][j];
                }
            }
            // Сохраняем веса для каждой строки
            for (int i = 0; i < n; ++i) {
                weights_per_row[i].push_back(row_weights[i]);
            }
            // Считаем средний вес строк в этом графе
            double avg_weight = 0.0;
            for (int w : row_weights) avg_weight += w;
            avg_weight /= n;
            all_avg_weights.push_back(avg_weight);

            std::vector<Edge> edges_list;
            for (auto e : make_iterator_range(edges(g))) {
                edges_list.push_back(e);
            }
            size_t num_edges = edges_list.size();

            bool found_valid_subgraph = false;
            for (size_t e_mask = 0; e_mask < (1ULL << num_edges) && !found_valid_subgraph; ++e_mask) {
                Graph subgraph(num_vertices(g));

                for (size_t i = 0; i < num_edges; ++i) {
                    if (e_mask & (1ULL << i)) {
                        Edge e = edges_list[i];
                        Vertex src = source(e, g);
                        Vertex tgt = target(e, g);
                        add_edge(src, tgt, subgraph);
                    }
                }

                if (!is_graph_connected(subgraph)) continue;

                bool no_apb = !has_articulation_points(subgraph) && !has_bridges(subgraph);
                bool degree_ok = all_vertices_degree_le(subgraph, degree);
            
                if (no_apb && degree_ok) {
                    found_valid_subgraph = true; 
                }
            }
            
            if (found_valid_subgraph) {
                count_with_valid_subgraph++;
            }
        }

        double p_red = static_cast<double>(stats_red_all.count) / NUMBER_OF_TRIALS;
        double p_blue = static_cast<double>(count_with_valid_subgraph) / NUMBER_OF_TRIALS;

        output << std::fixed << std::setprecision(2) << p
               << "," << std::scientific << p_red
               << "," << p_blue << "\n";

        // Выводим в консоль вес каждой строки и средний вес
        std::cout << "Probability p = " << std::fixed << std::setprecision(2) << p << ":\n";
        double overall_avg = 0.0;
        for (int i = 0; i < n; ++i) {
            double avg = 0.0;
            if (!weights_per_row[i].empty()) {
                for (int val : weights_per_row[i]) {
                    avg += val;
                }
                avg /= weights_per_row[i].size();
            }
            std::cout << "  Weight of row " << i << ": " << std::fixed << std::setprecision(2) << avg << "\n";
            overall_avg += avg;
        }
        overall_avg /= n;
        std::cout << "  Average weight: " << std::fixed << std::setprecision(2) << overall_avg << "\n\n";
    }
    
    output.close();
}

int main() {
    int n = 8; 
    generate_graphs_with_steps(n);
    
    return 0;
}