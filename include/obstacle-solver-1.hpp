#pragma once
#include <vector>

#include "common.hpp"
#include "graph-builder.hpp"


class ObstacleSolver1
{
private:
    GraphBuilder& graph_builder;
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
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones_sorted
    )
        : graph_builder(graph_bulider), zones_sorted(zones_sorted)
    {
    }

    void solve();
};