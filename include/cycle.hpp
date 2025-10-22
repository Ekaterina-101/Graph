#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

class CycleFinder {
private:
    std::vector<bool> used;
    std::vector<bool> in_stack;
    std::vector<Vertex> parent;
    std::vector<std::vector<Vertex>> cycles;
    std::vector<Vertex> current_path;

    void dfs(const Graph& g, Vertex u);

public:
    std::vector<Vertex> find_max_cycle(const Graph& g);
    const std::vector<std::vector<Vertex>>& get_all_cycles() const;
    std::size_t len_max_cycle(const Graph& g);
};


bool is_hamilton(Graph g);