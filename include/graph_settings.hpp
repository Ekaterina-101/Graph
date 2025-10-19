#pragma once

#include <fstream>

#include <boost/graph/adjacency_list.hpp>

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using Edge = boost::graph_traits<Graph>::edge_descriptor;

void print_graph_properties(const Graph& g, std::ofstream& os);
Graph matrix_to_boost_graph(const std::vector<std::vector<int>>& adj_matrix);

