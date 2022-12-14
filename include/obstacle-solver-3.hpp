#pragma once
#include <vector>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver3
{
private:
    FlowGraph& graph;
    const ZoneInfos& zones_sorted;


public:
    ObstacleSolver3() = delete;
    ObstacleSolver3
    (
        FlowGraph& graph, 
        const ZoneInfos& zones_sorted
    )
        : graph(graph), zones_sorted(zones_sorted)
    {
    }

    void solve();
};