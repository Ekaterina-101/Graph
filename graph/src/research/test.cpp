#include <iostream>

#include "art_points_and_bridges.hpp"

int main(int argc, char const *argv[]) {
    Graph g(3);
    add_edge(0, 1, g);
    add_edge(1, 2, g);
    add_edge(1, 3, g);
    add_edge(3, 4, g);
    add_edge(3, 5, g);

    std::cout << has_articulation_points(g) << " " << has_bridges(g) << "\n";

    return 0;
}
