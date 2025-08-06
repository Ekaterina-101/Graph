#pragma once

#include <boost/graph/depth_first_search.hpp>

#include "graph_settings.hpp"

bool has_articulation_points(const Graph& g);
bool has_bridges(const Graph& g);
