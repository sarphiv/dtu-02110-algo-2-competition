#pragma once
#include <vector>

#include "common.hpp"
#include "graph-builder.hpp"


class ObstacleSolver4
{
private:
    GraphBuilder& graph_builder;
    const std::vector<ZoneInfo>& zones_sorted;
    const std::vector<zone_idx_t>& zones_idx;


public:
    ObstacleSolver4() = delete;
    ObstacleSolver4(GraphBuilder& graph_bulider, const std::vector<ZoneInfo>& zones_sorted, const std::vector<zone_idx_t>& zones_idx)
        : graph_builder(graph_bulider), zones_sorted(zones_sorted), zones_idx(zones_idx)
    {
    }

    void solve();
};