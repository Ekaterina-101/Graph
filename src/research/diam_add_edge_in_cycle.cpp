#include <iostream>

// #include "art_points_and_bridges.hpp"

// int main(int argc, char const *argv[]) {
//     Graph g(3);
//     add_edge(0, 1, g);
//     add_edge(1, 2, g);
//     add_edge(1, 3, g);
//     add_edge(3, 4, g);
//     add_edge(3, 5, g);

//     std::cout << has_articulation_points(g) << " " << has_bridges(g) << "\n";

//     return 0;
// }


#include "graph_diam.hpp"
#include <vector>


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

std::vector<std::vector<int>> Generation_mtr(int n, int to_first, int from_first, int to_second, int from_second){
    std::vector<std::vector<int>> adj_matr(n, std::vector<int>(n, 0));

    for(int i = 0; i < n - 1; i++) {
        adj_matr[i][i+1] = 1;
        adj_matr[i + 1][i] = 1;
        adj_matr[i][i] = 1;
    }
    adj_matr[n-1][0] = 1;
    adj_matr[0][n-1] = 1;


    adj_matr[to_first][from_first] = 1;
    adj_matr[from_first][to_first] = 1;


    adj_matr[to_second][from_second] = 1;
    adj_matr[from_second][to_second] = 1;


    return adj_matr;
}

int main() {
    Graph g;
    for (int n = 6; n <= 6; n++) {
        for (int i = 0; i < n; i++) {
            for (int j = 1; j < n; j++) {
                for (int k = 2; k < n; k++) {
                        std::vector<std::vector<int>> adj_matrix = Generation_mtr(n, 0, i, j, k);
                        g = matrix_to_boost_graph(adj_matrix);
                        int d = diameter(g);
                        if (d <= n/2) {
                            std::cout << "n = " << n << " edge: 0 - " << i << " edge: " << j << " - " << k <<" diam = " << d << std::endl;

                        }
                }
            }


        }

        std::cout << std::endl;
    }

    // for (int n = 4; n <= 80; n++) {
    //     for (int i = 0; i < n; i++) {
    //                     std::vector<std::vector<int>> adj_matrix = Generation_mtr(n, 0, i, 0, 0);
    //                     g = matrix_to_boost_graph(adj_matrix);
    //                     int d = diameter(g);
    //                     if (d < n/2) {
    //                         std::cout << "n = " << n << " edge: 0 - " << i <<" diam = " << d << std::endl;

    //                     }
                
            


    //     }

    //     std::cout << std::endl;
    // }


}