#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"
#include "process_subgraphs.hpp"

using namespace boost;

const double EPS = 1e-8;

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

        if (to_add < needed) {
            int remaining = needed - to_add;
            std::vector<int> left_part;
            for (int j = 0; j < i; ++j) {
                if (adj_matrix[i][j] == 0) {
                    left_part.push_back(j);
                }
            }
            
            std::shuffle(left_part.begin(), left_part.end(), gen);
            int to_add_left = std::min(remaining, static_cast<int>(left_part.size()));
            //int to_add_left = remaining;
            for (int k = 0; k < to_add_left; ++k) {
                int neighbor = left_part[k];
                adj_matrix[i][neighbor] = 1;
                adj_matrix[neighbor][i] = 1;
            }
        }
    }
    
    return adj_matrix;
}

void generate_graphs_with_steps(int n) {
    const int NUMBER_OF_TRIALS = 100;
    const double PROB_MIN = 0.3;
    const double PROB_MAX = 1.0;
    const double PROB_STEP = 0.1;
    const int max_degree = 3;
    const int min_degree = 2; 
    
    std::ofstream output("../results_regular_graph_8v02.txt");
    std::ofstream gr("../gr_8v02.txt");
    
    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }
    
    output << "probability,connected,no_articulation_points,degree_limited,all_conditions,has_valid_subgraph,"
           << "avr_blue_conn,avr_blue_apb,avr_blue_deg,avr_blue_all,"
           << "d_visAvg,d_avgMin,d_avgAvg\n";
    std::cout << "Vertices: " << n << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    //bool flag = false;
    //bool red_printed = false;

    for (double p = PROB_MIN; p <= PROB_MAX + EPS; p += PROB_STEP) {
        DiameterStats stats_connected_red, stats_no_apb_red, 
                      stats_degree_red, stats_red_all;
        DiameterStats stats_blue_conn, stats_blue_no_apb, 
                      stats_blue_deg, stats_blue_all;
        //int count_with_valid_subgraph = 0;
        //int total_subgraphs = 0;
        //int total_valid_subgraphs = 0;
        //double sum_ratios = 0.0;
        int count_has_blue_subgraph = 0;

        //if (std::abs(p - 0.3) < EPS) {
        //    break;
        //}

        //std::vector<std::vector<int>> weights_per_row(n, std::vector<int>(0)); 
        //std::vector<double> all_avg_weights;

        double sum_d_vis = 0.0;
        int count_d_vis = 0;
        double sum_d_avgMin = 0.0;
        double sum_d_avgAvg = 0.0;
        int count_d_avg = 0;

        for (int trial = 0; trial < NUMBER_OF_TRIALS; ++trial) {
            auto adj_matrix = generate_single_graph(n, p, gen);
            
            Graph g = matrix_to_boost_graph(adj_matrix);
            int diam = diameter(g);

            if (is_graph_connected(g)) {
                //if (std::abs(p - 0.2) < EPS) {
                //    gr << "red conn\n";
                //    print_graph_properties(g, gr);
                //}
                
                bool no_apb = !has_articulation_points(g) && !has_bridges(g);
                bool degree_ok = all_vertices_degree_less(g, max_degree) &&
                                 all_vertices_degree_greater(g, min_degree);
                

                stats_connected_red.update(diam);
                if (no_apb) { stats_no_apb_red.update(diam); }
                if (degree_ok) { stats_degree_red.update(diam); }
                if (no_apb && degree_ok) { stats_red_all.update(diam); }
            }
            else {
                //if (std::abs(p - 0.2) < EPS) {
                //    gr << "red not conn\n";
                //    print_graph_properties(g, gr);
                //}
            }

            //if (p == 0.4 /*&& !red_printed*/) {
            //        std::cout << "red\n";
            //        print_graph_properties(g);
            //        //red_printed = true;
            //}

            //Подсчитываем вес каждой строки (количество единиц)
            //std::vector<int> row_weights(n, 0);
            //for (int i = 0; i < n; ++i) {
            //    for (int j = 0; j < n; ++j) {
            //        row_weights[i] += adj_matrix[i][j];
            //    }
            //}
            //// Сохраняем веса для каждой строки
            //for (int i = 0; i < n; ++i) {
            //    weights_per_row[i].push_back(row_weights[i]);
            //}
//
            //// Считаем средний вес строк в этом графе
            //double avg_weight = 0.0;
            //for (int w : row_weights) avg_weight += w;
            //avg_weight /= n;
            //all_avg_weights.push_back(avg_weight);

            sum_d_vis += diam;
            count_d_vis++;

            process_subgraphs(g,
                              stats_blue_conn,
                              stats_blue_no_apb,
                              stats_blue_deg,
                              stats_blue_all,
                              sum_d_avgMin,
                              sum_d_avgAvg,
                              count_d_avg,
                              count_has_blue_subgraph,
                              max_degree,
                              min_degree);

            //double ratio = (total_count > 0) 
            //    ? static_cast<double>(stats_all_blue.count) / total_count : 0.0;
            //sum_ratios += ratio;
            //if (has_blue_subgraph) { count_has_blue_subgraph++; }
        }

        std::cout << p << " " << stats_connected_red.count << "\n";

        double p_connected_red = static_cast<double>(stats_connected_red.count) / NUMBER_OF_TRIALS;
        double p_no_apb_red = static_cast<double>(stats_no_apb_red.count) / stats_connected_red.count;
        double p_degree_red = static_cast<double>(stats_degree_red.count) / stats_connected_red.count;
        double p_all_red = static_cast<double>(stats_red_all.count) / stats_connected_red.count; 
        //double p_blue = static_cast<double>(count_with_valid_subgraph) / static_cast<double>(stats_connected_red.count);
        //double p_blue_avg = (NUMBER_OF_TRIALS > 0)
        //    ? sum_ratios / NUMBER_OF_TRIALS
        //    : 0.0;
        double p_exists_blue = static_cast<double>(count_has_blue_subgraph) / stats_connected_red.count;
        
        double avr_blue_conn = static_cast<double>(stats_blue_conn.count) / stats_connected_red.count;
        double avr_blue_apb = static_cast<double>(stats_blue_no_apb.count) / stats_connected_red.count;
        double avr_blue_deg = static_cast<double>(stats_blue_deg.count) / stats_connected_red.count;
        double avr_blue_all = static_cast<double>(stats_blue_all.count) / stats_connected_red.count;

        //std::cout << avr_blue_all << "\n";

        double d_visAvg = (count_d_vis > 0) ? sum_d_vis / count_d_vis : 0.0;
        double d_avgMin = (count_d_avg > 0) ? sum_d_avgMin / count_d_avg : 0.0;
        double d_avgAvg = (count_d_avg > 0) ? sum_d_avgAvg / count_d_avg : 0.0;

        output << std::fixed << std::setprecision(2) << p
               << "," << std::scientific << p_connected_red
               << "," << p_no_apb_red 
               << "," << p_degree_red
               << "," << p_all_red
               << "," << p_exists_blue
               << "," << avr_blue_conn
               << "," << avr_blue_apb
               << "," << avr_blue_deg
               << "," << avr_blue_all
               << "," << d_visAvg
               << "," << d_avgMin
               << "," << d_avgAvg
               << "\n";

        // Выводим в консоль вес каждой строки и средний вес
        //std::cout << "Probability p = " << std::fixed << std::setprecision(2) << p << ":\n";
        //double overall_avg = 0.0;
        //for (int i = 0; i < n; ++i) {
        //    double avg = 0.0;
        //    if (!weights_per_row[i].empty()) {
        //        for (int val : weights_per_row[i]) {
        //            avg += val;
        //        }
        //        avg /= weights_per_row[i].size();
        //    }
        //    std::cout << "  Weight of row " << i << ": " << std::fixed << std::setprecision(2) << avg << "\n";
        //    overall_avg += avg;
        //}
        //overall_avg /= n;
        //std::cout << "  Average weight: " << std::fixed << std::setprecision(2) << overall_avg << "\n\n";
    }
    
    gr.close();
    output.close();
}

int main() {
    int n = 6; 
    generate_graphs_with_steps(n);
    
    return 0;
}
