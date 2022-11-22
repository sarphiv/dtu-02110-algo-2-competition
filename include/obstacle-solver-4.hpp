#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver4
{
private:
    FlowGraph& graph;
    const std::vector<ZoneInfo>& zones;


public:
    ObstacleSolver4() = delete;
    ObstacleSolver4(FlowGraph& graph, const std::vector<ZoneInfo>& zones)
        : graph(graph), zones(zones)
    {
    }

    void solve();
};