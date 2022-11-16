#pragma once
#include <list>

#include "common.hpp"
#include "flow-graph.hpp"


class GraphBuilder
{
public:
    std::list<ZoneEdge> edges;

    void add_edge(const ZoneEdge& edge)
    {
        edges.push_back(edge);
    }

    void add_edge
    (
        const zone_idx_t start, 
        const zone_idx_t& end, 
        const ZoneObstacle& obstacle,
        const zone_capacity_t& capacity
    )
    {
        edges.push_back({start, end, obstacle, capacity});
    }
    
    std::list<FlowEdge> build();
};