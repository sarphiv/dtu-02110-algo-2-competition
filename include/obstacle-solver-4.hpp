#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver4
{
private:
    FlowGraph& graph;
    const std::vector<ZoneInfo>& zones;
    const zone_idx_t terminal_idx;

public:
    ObstacleSolver4() = delete;
    ObstacleSolver4(FlowGraph& graph, const std::vector<ZoneInfo>& zones, const zone_idx_t terminal_idx)
        : graph(graph), zones(zones), terminal_idx(terminal_idx)
    {
    }

    void solve();
};