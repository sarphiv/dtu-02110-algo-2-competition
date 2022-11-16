#include <iostream>
#include <vector>
#include <tuple>

#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"
#include "obstacle-solver-2.hpp"
#include "obstacle-solver-3.hpp"
#include "obstacle-solver-4.hpp"
#include "graph-builder.hpp"
#include "flow-graph.hpp"


std::tuple<std::vector<ZoneInfo>, std::vector<zone_coord_t>, std::vector<zone_coord_t>> 
load_zones()
{
    // Load zone amount
    node_idx_t n;
    std::cin >> n;

    // Load zones and separate coordinates
    std::vector<ZoneInfo> zones(n);
    std::vector<zone_coord_t> x(n);
    std::vector<zone_coord_t> y(n);

    for (zone_idx_t i = 0; i < n; ++i)
    {
        ZoneInfo zone;
        
        std::cin 
            >> zone.x >> zone.y 
            >> zone.capacity[0] >> zone.capacity[1] 
            >> zone.capacity[2] >> zone.capacity[3];

        zones[i] = zone;
        x[i] = zone.x;
        y[i] = zone.y;
    }


    // Return vector of zones
    return {zones, x, y};
}


int main()
{
    // Load zones and sort by x and y
    auto [zones, x, y] = load_zones();
    auto [x_idx, x_idx_inv] = argsort<zone_coord_t, zone_idx_t>(x, true);
    auto [y_idx, y_idx_inv] = argsort<zone_coord_t, zone_idx_t>(y, true);


    // Build graph via solvers
    GraphBuilder graph_builder;

    ObstacleSolver4 solver_4(graph_builder, zones);
    solver_4.solve();

    ObstacleSolver1 solver_1(graph_builder, zones, x, y, x_idx, y_idx);
    solver_1.solve();




    // TODO: Construct zones sorted by x, y in O(n log n) (low constant) 
    //  via looping through x, if sequence of same, add to heap sorted by y,
    //  once new stuff or end, dequeue from heap

    // TODO: Solve convex hull

    // TODO: Solve obstacle 3


    // TODO: Make graph builder build method


    // Construct regular graph edges
    auto flow_edges = graph_builder.build();

    // Construct flow graph
    FlowGraph graph(0, zones.size(), flow_edges);

    // Find max flow and print
    std::cout << graph.maximize_flow() << std::endl;


    // Return success
    return 0;
}
