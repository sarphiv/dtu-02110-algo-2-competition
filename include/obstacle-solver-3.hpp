#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


using obstacle_solver_3_line_val_t = float;


class ObstacleSolver3
{
private:
    FlowGraph& graph;

    const std::vector<ZoneInfo>& zones_sorted;



public:
    ObstacleSolver3() = delete;
    ObstacleSolver3
    (
        FlowGraph& graph, 
        const std::vector<ZoneInfo>& zones_sorted
    )
        : graph(graph), zones_sorted(zones_sorted)
    {
    }

    void solve();
};