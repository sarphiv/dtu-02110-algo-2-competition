#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver1
{
private:
    FlowGraph& graph;
    const ZoneInfos& zones_sorted;


    void connect_range
    (
        const std::vector<zone_idx_t>& zone_idx,
        const std::vector<zone_capacity_t>& zone_cap,
        const zone_idx_t start,
        const zone_idx_t end
    );


public:
    ObstacleSolver1() = delete;
    ObstacleSolver1
    (
        FlowGraph& graph, 
        const ZoneInfos& zones_sorted
    )
        : graph(graph), zones_sorted(zones_sorted)
    {
    }

    void solve();
};