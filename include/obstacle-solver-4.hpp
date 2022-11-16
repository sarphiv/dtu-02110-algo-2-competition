#pragma once
#include <vector>

#include "common.hpp"
#include "graph-builder.hpp"


class ObstacleSolver4
{
private:
    GraphBuilder& graph_builder;
    const std::vector<ZoneInfo>& zones;

public:
    ObstacleSolver4() = delete;
    ObstacleSolver4(GraphBuilder& graph_bulider, const std::vector<ZoneInfo>& zones)
        : graph_builder(graph_bulider), zones(zones)
    {
    }

    void solve();
};