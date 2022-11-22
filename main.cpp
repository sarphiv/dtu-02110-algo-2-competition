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
#include "flow-graph.hpp"


std::vector<ZoneInfo>
load_zones(std::istream& stream)
{
    // Load zone amount
    zone_idx_t n;
    stream >> n;

    // Load zones
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
    // Load zones from either stdin or file (argument)
    auto zones = [&]()
    {
        if (argc > 1)
        {
            std::ifstream fstream(argv[1]);
            auto ret = load_zones(fstream);
            fstream.close();

            return ret;
        }
        else
            return load_zones(std::cin);
    }();

    // Disallow terminal zone from connecting out
    const zone_idx_t terminal_idx = zones.size() - 1;
    {
        auto& terminal = zones[terminal_idx];
        terminal.capacity[O1] = 0; terminal.capacity[O2] = 0; terminal.capacity[O3] = 0; terminal.capacity[O4] = 0;
    }

    // Sort zones by x then y
    argsort<ZoneInfo, zone_idx_t>
    (
        zones,
        [&](const zone_idx_t &a, const zone_idx_t &b)
        {
            return (zones[a].x < zones[b].x) | ((zones[a].x == zones[b].x) & (zones[a].y < zones[b].y));
        }
    );


    // Create flow graph
    FlowGraph graph
    (
        zones, 
        0, 
        terminal_idx
    );


    // Build graph via solvers
    ObstacleSolver4 solver_4(graph, zones, terminal_idx);
    // std::cout << "Solving 4" << std::endl;
    solver_4.solve();

    // NOTE: Solver 4 must be run before solver 2,
    //  because reliant on terminal node edge being at index 0 in inner adjacency lists
    ObstacleSolver2 solver_2(graph, zones, terminal_idx);
    // std::cout << "Solving 2" << std::endl;
    solver_2.solve();

    ObstacleSolver1 solver_1(graph, zones);
    // std::cout << "Solving 1" << std::endl;
    solver_1.solve();

    ObstacleSolver3 solver_3(graph, zones);
    // std::cout << "Solving 3" << std::endl;
    solver_3.solve();


    // Find max flow and print
    // std::cout << "Flowing" << std::endl;
    std::cout << graph.calculate_maximum_flow() << std::endl;


    // Return success
    return 0;
}
