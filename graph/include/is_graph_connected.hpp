#pragma once

#include <boost/graph/depth_first_search.hpp>

#include "graph_settings.hpp"

class VisitCounter : public boost::default_dfs_visitor {
public:
    void discover_vertex(Vertex u, const Graph& g);
};

bool is_graph_connected(const Graph& g);
