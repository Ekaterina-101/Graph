cmake_minimum_required(VERSION 3.10)
project(GraphConnectivity)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -stdlib=libstdc++ -pedantic -O3 -mtune=x86-64 -march=x86-64")

find_package(Boost REQUIRED COMPONENTS graph program_options)

add_library(graph_properties
    src/graph_properties/art_points_and_bridges.cpp
    src/graph_properties/graph_diam.cpp
    src/graph_properties/is_graph_connected.cpp
    src/graph_properties/graph_settings.cpp
    src/algorithms/brute_force.cpp
    src/algorithms/brute_force_core.cpp
    src/algorithms/brute_force_output.cpp
    src/algorithms/random_core.cpp
    src/algorithms/random_generator.cpp
    src/algorithms/random_output.cpp
    src/algorithms/process_subgraphs.cpp
)

target_include_directories(graph_properties
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/graph_properties
)

add_library(graph_headers INTERFACE)
target_include_directories(graph_headers
    INTERFACE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#add_executable(random_generator src/research/random_generator.cpp)
#target_link_libraries(random_generator PRIVATE graph_properties graph_headers)

add_executable(new src/research/alg.cpp)
target_link_libraries(new PRIVATE graph_properties graph_headers)

add_executable(generator src/research/united.cpp)
target_link_libraries(generator PRIVATE graph_properties graph_headers Boost::program_options)

add_executable(test src/research/test.cpp)
target_link_libraries(test PRIVATE graph_properties graph_headers)
