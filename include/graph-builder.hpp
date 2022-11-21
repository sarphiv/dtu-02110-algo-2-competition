#pragma once
#include <vector>
#include <list>

#include "common.hpp"
#include "flow-graph.hpp"



class GraphBuilder
{
private:
    const std::vector<ZoneInfo>& zones;


public:
    std::list<ZoneEdge> edges;


    GraphBuilder() = delete;
    GraphBuilder(const std::vector<ZoneInfo>& zones)
        : zones(zones)
    {
    }

    zone_idx_t get_zone_input(zone_idx_t zone_idx)
    {
        return zone_idx * (ZONE_OBSTACLE_SIZE + 1);
    }

    zone_idx_t get_zone_output(zone_idx_t zone_idx, zone_obstacle_val_t obstacle)
    {
        return zone_idx * (ZONE_OBSTACLE_SIZE + 1) + 1 + obstacle;
    }


    void add_edge(const ZoneEdge& edge)
    {
        edges.push_back(edge);
    }

    void add_edge
    (
        const zone_idx_t& start, 
        const zone_idx_t& end, 
        const ZoneObstacle& obstacle,
        const zone_capacity_t& capacity
    )
    {
        edges.push_back({start, end, obstacle, capacity});
    }

    std::list<FlowEdge> build();
};