#include "cycle.hpp"
// #include "alg.hpp"




#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "graph_diam.hpp"
#include "is_graph_connected.hpp"
#include "art_points_and_bridges.hpp"

// #include "alg.hpp"

using namespace boost;


Graph matrix_to_boost_graph(const std::vector<std::vector<int>>& adj_matrix) {
    int n = adj_matrix.size();
    
    Graph g(n);
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (adj_matrix[i][j] == 1) {
                boost::add_edge(i, j, g);
            }
        }
    }
    
    return g;
}

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

void CycleFinder::dfs(const Graph& g, Vertex u) {
    used[u] = true;
    in_stack[u] = true;
    current_path.push_back(u);

    for (auto e : boost::make_iterator_range(boost::out_edges(u, g))) {
        Vertex v = boost::target(e, g);
        if (v == u) v = boost::source(e, g);

        if (!used[v]) {
            parent[v] = u;
            dfs(g, v);
        } else if (in_stack[v] && v != parent[u]) {
            std::vector<Vertex> cycle;
            for (int i = current_path.size() - 1; i >= 0; --i) {
                cycle.push_back(current_path[i]);
                if (current_path[i] == v) break;
            }
            std::reverse(cycle.begin(), cycle.end());
            cycle.push_back(v);
            cycles.push_back(cycle);
        }
    }

    in_stack[u] = false;
    current_path.pop_back();
}


std::vector<Vertex> CycleFinder::find_max_cycle(const Graph& g) {
    size_t num_vertices = boost::num_vertices(g);
    used.assign(num_vertices, false);
    in_stack.assign(num_vertices, false);
    parent.assign(num_vertices, Vertex());
    current_path.clear();
    cycles.clear();

    for (auto v : boost::make_iterator_range(boost::vertices(g))) {
        if (!used[v]) {
            dfs(g, v);
        }
    }

    if (cycles.empty()) {
        return {};
    }

    auto longest_cycle = std::max_element(
        cycles.begin(), 
        cycles.end(),
        [](const std::vector<Vertex>& a, const std::vector<Vertex>& b) {
            return a.size() < b.size();
        }
    );

    return *longest_cycle;
}

const std::vector<std::vector<Vertex>>& CycleFinder::get_all_cycles() const {
    return cycles;
}


std::size_t CycleFinder::len_max_cycle(const Graph& g) {
    std::vector<Vertex> longest_cycle = find_max_cycle(g);
    return longest_cycle.size();
}


void analysis(int n, double p) {
    const int NUMBER_OF_TRIALS = 100;

    CycleFinder finder;
    std::vector<int> count_of_len_max_c(n + 1, 0);


    std::ofstream output("../results_regular_graph_cycle.txt");
    
    if (!output) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    output << "len_max_cycle, percent\n";


    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < NUMBER_OF_TRIALS; i++) {
        auto adj_matrix = generate_single_graph(n, p, gen);
        Graph g = matrix_to_boost_graph(adj_matrix);
        count_of_len_max_c[finder.len_max_cycle(g)]++;
    }

    for (int i = 0; i < n+1; i++) {
        output << std::fixed << std::setprecision(2) << i
               << "," << std::scientific << double(count_of_len_max_c[i])/NUMBER_OF_TRIALS << "\n";
    }    
}


bool is_hamilton(Graph g) {
    CycleFinder finder;
    auto longest_cycle = finder.find_max_cycle(g);



    if (finder.len_max_cycle(g) == num_vertices(g)) {
        return true;
    }

    return false;
}



// int main() {
//     Graph g;
//     boost::add_edge(0, 1, g);
//     boost::add_edge(0, 2, g);
//     boost::add_edge(0, 3, g);
//     boost::add_edge(1, 2, g);
//     boost::add_edge(1, 4, g);
//     boost::add_edge(1, 6, g);
//     boost::add_edge(3, 4, g);
//     boost::add_edge(4, 5, g);
//     boost::add_edge(5, 6, g);
//     boost::add_edge(2, 3, g);
//     boost::add_edge(2, 5, g);

//     CycleFinder finder;
//     auto longest_cycle = finder.find_max_cycle(g);

//     std::cout << "Самый длинный цикл: " << finder.len_max_cycle(g) << " : ";
//     for (Vertex v : longest_cycle) {
//         std::cout << v << " ";
//     }
//     std::cout << "\n";

//     std::cout << "Найдено циклов: " << finder.get_all_cycles().size() << "\n";
//     for (const auto& cycle : finder.get_all_cycles()) {
//         std::cout << "Цикл: ";
//         for (Vertex v : cycle) {
//             std::cout << v << " ";
//         }
//         std::cout << "\n";
//     }

//     analysis(8, 0.2);

//     return 0;
// }