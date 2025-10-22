#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <numeric> // for std::accumulate

#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"
#include "process_subgraphs.hpp"

using namespace boost;  

// Структура для хранения результатов одного потока
struct SubgraphResults {
    DiameterStats stats_conn;
    DiameterStats stats_no_apb;
    DiameterStats stats_deg;
    DiameterStats stats_all;
    std::vector<int> valid_diameters;
    bool has_valid_subgraph = false;

    SubgraphResults() = default;

    void reset() {
        stats_conn.clear();
        stats_no_apb.clear();
        stats_deg.clear();
        stats_all.clear();
        valid_diameters.clear();
        has_valid_subgraph = false;
    }
};

std::mutex stats_mutex;
std::mutex valid_diameters_mutex;

void process_subgraph_range(const Graph& original_graph,
                            size_t start_mask,
                            size_t end_mask,
                            const int max_degree,
                            const int min_degree,
                            SubgraphResults& results) {

    std::vector<Edge> edges_list;

    for (auto e : make_iterator_range(edges(original_graph))) {
        edges_list.push_back(e);
    }

    size_t num_edges = edges_list.size();

    for (size_t e_mask = start_mask; e_mask < end_mask; ++e_mask) {
        Graph subgraph(num_vertices(original_graph));

        for (size_t i = 0; i < num_edges; ++i) {
            if (e_mask & (1ULL << i)) {
                Edge e = edges_list[i];
                Vertex src = source(e, original_graph);
                Vertex tgt = target(e, original_graph);
                add_edge(src, tgt, subgraph);
            }
        }

        if (!is_graph_connected(subgraph)) continue;

        int sub_diam = diameter(subgraph);

        {
            std::lock_guard<std::mutex> lock(stats_mutex);
            results.stats_conn.update(sub_diam);
        }

        bool no_apb = !has_articulation_points(subgraph) && !has_bridges(subgraph);
        bool degree_ok = all_vertices_degree_less(subgraph, max_degree) &&
                         all_vertices_degree_greater(subgraph, min_degree);

        if (no_apb) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            results.stats_no_apb.update(sub_diam);
        }

        if (degree_ok) {
            std::lock_guard<std::mutex> lock(stats_mutex);
            results.stats_deg.update(sub_diam);
        }

        if (no_apb && degree_ok) {
            std::lock_guard<std::mutex> lock(stats_mutex);

            results.stats_all.update(sub_diam);
            results.has_valid_subgraph = true;
            results.valid_diameters.push_back(sub_diam);
        }
    }
}

void process_subgraphs(const Graph& original_graph,
                      DiameterStats& stats_blue_conn,
                      DiameterStats& stats_blue_no_apb,
                      DiameterStats& stats_blue_deg,
                      DiameterStats& stats_blue_all,
                      double& sum_d_avgMin,
                      double& sum_d_avgAvg,
                      int& count_d_avg,
                      int& count_has_blue_subgraph,
                      const int max_degree,
                      const int min_degree,
                      size_t num_threads) {

    std::vector<Edge> edges_list;
    for (auto e : make_iterator_range(edges(original_graph))) {
        edges_list.push_back(e);
    }
    size_t num_edges = edges_list.size();
    size_t total_masks = (1ULL << num_edges);

    if (num_threads <= 1 || total_masks < 2) {
        SubgraphResults single_results;
        process_subgraph_range(original_graph, 0, total_masks, max_degree, min_degree, single_results);

        stats_blue_conn.merge(single_results.stats_conn);
        stats_blue_no_apb.merge(single_results.stats_no_apb);
        stats_blue_deg.merge(single_results.stats_deg);
        stats_blue_all.merge(single_results.stats_all);

        if (!single_results.valid_diameters.empty()) {
            int min_diam = *std::min_element(single_results.valid_diameters.begin(), single_results.valid_diameters.end());
            sum_d_avgMin += min_diam;
            double avg_diam = std::accumulate(single_results.valid_diameters.begin(), single_results.valid_diameters.end(), 0.0) / single_results.valid_diameters.size();
            sum_d_avgAvg += avg_diam;
            count_d_avg++;
        }

        if (single_results.has_valid_subgraph) {
            count_has_blue_subgraph++;
        }
        return;
    }

    std::vector<std::thread> threads;
    std::vector<SubgraphResults> thread_results(num_threads);

    size_t masks_per_thread = total_masks / num_threads;
    size_t remainder = total_masks % num_threads;

    size_t start_mask = 0;
    for (size_t t = 0; t < num_threads; ++t) {
        size_t end_mask = start_mask + masks_per_thread;
        if (t < remainder) {
            end_mask++; // Распределяем остаток
        }

        threads.emplace_back(process_subgraph_range,
                             std::cref(original_graph),
                             start_mask,
                             end_mask,
                             max_degree,
                             min_degree,
                             std::ref(thread_results[t]));

        start_mask = end_mask;
    }

    for (auto& th : threads) {
        th.join();
    }

    std::vector<int> combined_valid_diameters;
    bool any_has_valid = false;

    for (const auto& res : thread_results) {
        stats_blue_conn.merge(res.stats_conn);
        stats_blue_no_apb.merge(res.stats_no_apb);
        stats_blue_deg.merge(res.stats_deg);
        stats_blue_all.merge(res.stats_all);

        combined_valid_diameters.insert(combined_valid_diameters.end(),
                                        res.valid_diameters.begin(),
                                        res.valid_diameters.end());

        if (res.has_valid_subgraph) {
            any_has_valid = true;
        }
    }

    if (!combined_valid_diameters.empty()) {
        int min_diam = *std::min_element(combined_valid_diameters.begin(), combined_valid_diameters.end());
        sum_d_avgMin += min_diam;
        double avg_diam = std::accumulate(combined_valid_diameters.begin(), combined_valid_diameters.end(), 0.0) / combined_valid_diameters.size();
        sum_d_avgAvg += avg_diam;
        count_d_avg++;
    }

    if (any_has_valid) {
        count_has_blue_subgraph++;
    }
}
