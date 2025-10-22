#include <iostream>

#include "process_subgraphs.hpp"
#include "graph_diam.hpp"

int main() {
    // Создаем простой граф для теста
    Graph g;
    Vertex v0 = add_vertex(g);
    Vertex v1 = add_vertex(g);
    Vertex v2 = add_vertex(g);
    Vertex v3 = add_vertex(g);
    add_edge(v0, v1, g);
    add_edge(v1, v2, g);
    add_edge(v2, v3, g);
    add_edge(v0, v3, g);

    // Параметры
    int max_degree = 3;
    int min_degree = 2;
    size_t num_threads = 1;  // ПОКА ОБЯЗАТЕЛЬНО 1

    // Статистика
    DiameterStats stats_blue_conn, 
                  stats_blue_no_apb, 
                  stats_blue_deg, 
                  stats_blue_all;
                  
    double sum_d_avgMin = 0.0, sum_d_avgAvg = 0.0;
    int count_d_avg = 0, count_has_blue_subgraph = 0;

    // Запускаем обработку графа
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
                      min_degree,
                      num_threads);

    // Выводим результаты
    std::cout << "Statistics for blue subgraphs:\n";
    std::cout << "  - Connected subgraphs count: " << stats_blue_conn.count << "\n";
    std::cout << "  - Subgraphs without APB count: " << stats_blue_no_apb.count << "\n";
    std::cout << "  - Subgraphs with degree condition: " << stats_blue_deg.count << "\n";
    std::cout << "  - All valid subgraphs: " << stats_blue_all.count << "\n";
    std::cout << "  - Average diameter: " << sum_d_avgAvg / count_d_avg << "\n";
    std::cout << "  - Minimum diameter: " << sum_d_avgMin / count_d_avg << "\n";
    std::cout << "  - Subgraphs with blue properties: " << count_has_blue_subgraph << "\n";

    return 0;
}
