#include <iostream>

#include "is_graph_connected.hpp"

using namespace boost;

using ColorVec = std::vector<default_color_type>;

static unsigned int discovered_vertices_counter = 0;

void VisitCounter::discover_vertex([[maybe_unused]] Vertex u, [[maybe_unused]] const Graph& g) { 
    //std::cout << u << std::endl;
    ++discovered_vertices_counter; 
}


bool is_graph_connected(const Graph& g) {
    if (num_vertices(g) <= 1) { return true; }

    ColorVec color(num_vertices(g));
    auto color_map = make_iterator_property_map(color.begin(), get(vertex_index, g));

    VisitCounter visitor;
    Vertex start = vertex(0, g);
    discovered_vertices_counter = 0;

    depth_first_visit(g, start, visitor, color_map);

    return discovered_vertices_counter == num_vertices(g);
}
