#include "cycle.hpp"

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



int main() {
    Graph g;
    boost::add_edge(0, 1, g);
    boost::add_edge(0, 2, g);
    boost::add_edge(0, 3, g);
    boost::add_edge(1, 2, g);
    boost::add_edge(1, 4, g);
    boost::add_edge(1, 6, g);
    boost::add_edge(3, 4, g);
    boost::add_edge(4, 5, g);
    boost::add_edge(5, 6, g);
    boost::add_edge(2, 3, g);
    boost::add_edge(2, 5, g);

    CycleFinder finder;
    auto longest_cycle = finder.find_max_cycle(g);

    std::cout << "Самый длинный цикл: ";
    for (Vertex v : longest_cycle) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    std::cout << "Найдено циклов: " << finder.get_all_cycles().size() << "\n";
    for (const auto& cycle : finder.get_all_cycles()) {
        std::cout << "Цикл: ";
        for (Vertex v : cycle) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }

    return 0;
}