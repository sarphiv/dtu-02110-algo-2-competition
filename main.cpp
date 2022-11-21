#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>


#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"
#include "obstacle-solver-2.hpp"
#include "obstacle-solver-3.hpp"
#include "obstacle-solver-4.hpp"
#include "graph-builder.hpp"
#include "flow-graph.hpp"


std::vector<ZoneInfo>
load_zones(std::istream& stream)
{
    // Load zone amount
    node_idx_t n;
    stream >> n;

    // Load zones and separate coordinates
    std::vector<ZoneInfo> zones(n);

    for (zone_idx_t i = 0; i < n; ++i)
    {
        ZoneInfo zone;

        stream 
            >> zone.x >> zone.y 
            >> zone.capacity[0] >> zone.capacity[1] 
            >> zone.capacity[2] >> zone.capacity[3];
        zone.idx = i;


        zones[i] = zone;
    }


    // Return vector of zones
    return zones;
}


int main(int argc, char *argv[])
{
    // Load zones and separate out coordinates
    std::ifstream fstream;
    if (argc > 1) fstream.open(argv[1]);
    auto zones = load_zones(argc > 1 ? fstream : std::cin);
    if (argc > 1) fstream.close();


    // Sort zones by x then y
    argsort<ZoneInfo, zone_idx_t>
    (
        zones,
        [&](const zone_idx_t &a, const zone_idx_t &b)
        {
            return (zones[a].x < zones[b].x) | ((zones[a].x == zones[b].x) & (zones[a].y < zones[b].y));
        }
    );


    // Build graph via solvers
    GraphBuilder graph_builder(zones);

    ObstacleSolver4 solver_4(graph_builder, zones);
    std::cout << "Solving 4" << std::endl;
    solver_4.solve();

    ObstacleSolver1 solver_1(graph_builder, zones);
    std::cout << "Solving 1" << std::endl;
    solver_1.solve();

    ObstacleSolver2 solver_2(graph_builder, zones);
    std::cout << "Solving 2" << std::endl;
    solver_2.solve();

    ObstacleSolver3 solver_3(graph_builder, zones);
    std::cout << "Solving 3" << std::endl;
    solver_3.solve();


    // Construct regular graph edges
    std::cout << "Building" << std::endl;
    auto flow_edges = graph_builder.build();

    // Construct flow graph
    std::cout << "Constructing" << std::endl;
    FlowGraph graph(0, graph_builder.get_zone_input(zones.size()-1), flow_edges);
    

    // Find max flow and print
    std::cout << "Flowing" << std::endl;
    std::cout << graph.maximize_flow() << std::endl;


    // Return success
    return 0;
}
