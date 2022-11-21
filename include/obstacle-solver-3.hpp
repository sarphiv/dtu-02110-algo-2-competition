#pragma once
#include <vector>

#include "common.hpp"
#include "graph-builder.hpp"


using obstacle_solver_3_line_val_t = float;


class ObstacleSolver3
{
private:
    GraphBuilder& graph_builder;

    const std::vector<ZoneInfo>& zones_sorted;



public:
    ObstacleSolver3() = delete;
    ObstacleSolver3
    (
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones_sorted
    )
        : graph_builder(graph_bulider), zones_sorted(zones_sorted)
    {
    }

    void solve();
};