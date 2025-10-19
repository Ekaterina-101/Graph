#pragma once

#include "graph_diam.hpp"

class AbstractGenerator {
protected:
    Graph m_graph;
    DiameterStats stats_connected;
    DiameterStats stats_no_apb;
    DiameterStats stats_degree;
    DiameterStats stats_all_conditions;
    
public:
    std::size_t m_degree;
    explicit AbstractGenerator(Graph graph) : m_graph(std::move(graph)) {}
    virtual void generate_graphs() = 0;
};