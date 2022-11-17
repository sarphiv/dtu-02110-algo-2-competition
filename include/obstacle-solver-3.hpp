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

    const std::vector<zone_idx_t>& zones_idx;



public:
    ObstacleSolver3() = delete;
    ObstacleSolver3
    (
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones_sorted, 
        const std::vector<zone_idx_t>& zones_idx
    )
        : graph_builder(graph_bulider), zones_sorted(zones_sorted), zones_idx(zones_idx)
    {
    }

    void solve_naive();
    void solve();
};