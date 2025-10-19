#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "brute_force.hpp"
#include "random_generator.hpp"

using namespace boost;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    std::size_t number_of_vertices = 7;
    std::size_t number_of_trials = 10000;
    std::size_t max_graph_degree = 3;
    std::size_t min_graph_degree = 2;
    double prob_min = 0.0;
    double prob_max = 1.0;
    double prob_step = 0.1;
    bool cond_prob = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("vertices,v", po::value<std::size_t>(&number_of_vertices)->default_value(7), "Number of vertices")
        ("trials,t", po::value<std::size_t>(&number_of_trials)->default_value(10000), "Number of trials for Random Generator")
        ("degree,d", po::value<std::size_t>(&max_graph_degree)->default_value(3), "Maximum degree of graph")
        ("cond_prob,cp", po::value<bool>(&cond_prob)->default_value(false), "Conditional or Unconditional probability for Random Generator")
        ("prob_min,pmin", po::value<double>(&prob_min)->default_value(0.0), "Minimum probability for Random Generator")
        ("prob_max,pmax", po::value<double>(&prob_max)->default_value(1.0), "Maximum probability for Random Generator")
        ("prob_step,ps", po::value<double>(&prob_step)->default_value(0.1), "Step for probability in Random Generator");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::cout << "Number of vertices: " << number_of_vertices << std::endl;
    std::cout << "Number of trials: " << number_of_trials << std::endl;
    std::cout << "Maximum graph degree: " << max_graph_degree << std::endl;
    std::cout << "Minimum graph degree: " << min_graph_degree << std::endl;
    std::cout << "Probability range: [" << prob_min << ", " << prob_max << "] with step: " << prob_step << std::endl;

    Graph g(number_of_vertices);
    for (std::size_t i = 0; i < number_of_vertices; ++i) {
        for (std::size_t j = i + 1; j < number_of_vertices; ++j) {
            add_edge(i, j, g);
        }
    }

    //std::cout << "\033[31mRANDOM GENERATOR:\033[0m\n";
    //RandomGenerator randomGenerator(g);
    //randomGenerator.m_degree = graph_degree;
    //randomGenerator.NUMBER_OF_TRIALS = number_of_trials;
    //randomGenerator.PROB_MIN = prob_min;
    //randomGenerator.PROB_MAX = prob_max;
    //randomGenerator.PROB_STEP = prob_step;
    //randomGenerator.COND_PROB = cond_prob;
    //randomGenerator.generate_graphs();

    std::cout << "\n" << std::string(90, '-');

    std::cout << "\n\n\033[31mBRUTE-FORCE GENERATOR:\033[0m\n";
    BruteForceGenerator bruteForceGenerator(g);
    bruteForceGenerator.m_max_degree = max_graph_degree;
    bruteForceGenerator.m_min_degree = min_graph_degree;
    bruteForceGenerator.generate_graphs();

    //std::ofstream out("../graphic.txt");
    //for (std::size_t i = 0; i <= 8; ++i) {
    //    Graph g(i);  
    //    BruteForceGenerator BFG(g);  
//
    //    out << i; 
//
    //    for (std::size_t j = 3; j <= 5; ++j) {
    //        BFG.m_max_degree = j;
    //        double result = BFG.get_prob();
    //        out << "," <<std::scientific << result;
    //    }
    //    out << "\n";
    //}
    
    return 0;
}
