#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver1
{
private:
    FlowGraph& graph;
    const std::vector<ZoneInfo>& zones_sorted;


    void connect_range
    (
        const std::vector<ZoneInfo>& zones,
        const zone_idx_t start, 
        const zone_idx_t end
    );


public:
    ObstacleSolver1() = delete;
    ObstacleSolver1
    (
        FlowGraph& graph, 
        const std::vector<ZoneInfo>& zones_sorted
    )
        : graph(graph), zones_sorted(zones_sorted)
    {
    }

    void solve();
};