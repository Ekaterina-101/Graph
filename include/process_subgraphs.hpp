#pragma once

#include <thread>

#include <graph_diam.hpp>

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
                      size_t num_threads = std::thread::hardware_concurrency());
