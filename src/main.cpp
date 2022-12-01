#include <vector>
#include <iostream>
#include <fstream>


#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"
#include "obstacle-solver-2.hpp"
#include "obstacle-solver-3.hpp"
#include "obstacle-solver-4.hpp"
#include "flow-graph.hpp"


ZoneInfos
load_zones(std::istream& stream)
{
    // Load zone amount
    zone_idx_t n;
    stream >> n;

    // Load zones
    ZoneInfos zones(n);

    for (zone_idx_t i = 0; i < n; ++i)
    {
        stream 
            >> zones.x[i] >> zones.y[i] 
            >> zones.capacity[0][i] >> zones.capacity[1][i] 
            >> zones.capacity[2][i] >> zones.capacity[3][i];

        zones.idx[i] = i;
    }


    // Return zones keyed by member
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
    const zone_idx_t terminal_idx = zones.size - 1;
    {
        for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
            zones.capacity[o][terminal_idx] = 0;
    }


    // Sort zones by x then y
    const auto zones_idx = argsort<zone_idx_t, zone_idx_t>
    (
        zones.idx,
        [&](const zone_idx_t &a, const zone_idx_t &b)
        {
            return (zones.x[a] < zones.x[b]) | ((zones.x[a] == zones.x[b]) & (zones.y[a] < zones.y[b]));
        }
    );

    argsort(zones.x, zones_idx);
    argsort(zones.y, zones_idx);
    for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
        argsort(zones.capacity[o], zones_idx);



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
