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


int main(int argc, char *argv[])
{
    // Load zones and separate out coordinates
    auto [zones, x, y] = load_zones();


    // Sort y, and then sort x by x then y
    auto [y_idx, y_idx_inv] = argsort<zone_coord_t, zone_idx_t>(y, true);
    auto [x_idx, x_idx_inv] = argsort<zone_coord_t, zone_idx_t>
    (
        x,
        [&](const zone_idx_t &a, const zone_idx_t &b)
        {
            return x[a] < x[b] || (x[a] == x[b] && y_idx_inv[a] < y_idx_inv[b]);
        },
        true
    );

    // Make zones inherit order of x
    const auto& zones_idx = x_idx;
    const auto& zones_idx_inv = x_idx_inv;
    {
        std::vector<ZoneInfo> zones_sorted(zones.size());
        for (zone_idx_t i = 0; i < zones_idx.size(); ++i)
            zones_sorted[i] = zones[zones_idx[i]];
        zones = zones_sorted;
    }


    // Build graph via solvers
    GraphBuilder graph_builder(zones, zones_idx);

    ObstacleSolver4 solver_4(graph_builder, zones, zones_idx);
    solver_4.solve();

    ObstacleSolver1 solver_1(graph_builder, zones, zones_idx_inv, x, y, x_idx, y_idx);
    solver_1.solve();

    ObstacleSolver2 solver_2(graph_builder, zones, zones_idx);
    solver_2.solve();

    ObstacleSolver3 solver_3(graph_builder, zones, zones_idx);
    solver_3.solve();


    // Construct regular graph edges
    auto flow_edges = graph_builder.build();

    // Construct flow graph
    FlowGraph graph(0, graph_builder.get_zone_input(zones.size()-1), flow_edges);
    

    // Find max flow and print
    std::cout << graph.maximize_flow() << std::endl;


    // Return success
    return 0;
}
