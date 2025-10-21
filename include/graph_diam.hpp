#pragma once

#include "graph_settings.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

struct DiameterStats {
    int min = INT32_MAX;
    int max = INT32_MIN;
    long long sum = 0;
    long long count = 0;
    long long count_min = 0;
    long long count_max = 0;

    std::map<int, long long> diameter_counts;
    std::map<int, long long> edge_counts;
    std::map<int, std::map<int, long long>> diameter_edge_counts;

    void update(int d, int edges = -1) { 
        sum += d;
        count++;

        if (d < min) {
            min = d;
            count_min = 1;
        } else if (d == min) {
            count_min++;
        }

        if (d > max) {
            max = d;
            count_max = 1;
        } else if (d == max) {
            count_max++;
        }

        diameter_counts[d]++;
        
        if (edges >= 0) {
            edge_counts[edges]++;
            diameter_edge_counts[d][edges]++;
        }
    }

    void merge(const DiameterStats& other) {
        if (other.count == 0) return; // Nothing to merge
        
        // Update min and count_min
        if (other.min < min) {
            min = other.min;
            count_min = other.count_min;
        } else if (other.min == min) {
            count_min += other.count_min;
        }
    
        // Update max and count_max
        if (other.max > max) {
            max = other.max;
            count_max = other.count_max;
        } else if (other.max == max) {
            count_max += other.count_max;
        }
    
        // Update sum and count
        sum += other.sum;
        count += other.count;
    
        // Merge diameter_counts map
        for (const auto& [diam, cnt] : other.diameter_counts) {
            diameter_counts[diam] += cnt;
        }
    
        // Merge edge_counts map
        for (const auto& [edge_val, cnt] : other.edge_counts) {
            edge_counts[edge_val] += cnt;
        }
    
        // Merge diameter_edge_counts nested map
        for (const auto& [diam, edge_map] : other.diameter_edge_counts) {
            for (const auto& [edge_val, cnt] : edge_map) {
                diameter_edge_counts[diam][edge_val] += cnt;
            }
        }
    }

    double average() const { return count > 0 ? static_cast<double>(sum) / count : 0.0; }

    void clear() {
        min = INT32_MAX; max = INT32_MIN;
        sum = 0; count = 0;
        count_min = 0; count_max = 0;
        diameter_counts.clear();
        edge_counts.clear();
        diameter_edge_counts.clear();
    }
};

int diameter(const Graph& g);
bool all_vertices_degree_less(const Graph& g, std::size_t degree);
bool all_vertices_degree_greater(const Graph& g, std::size_t degree);
