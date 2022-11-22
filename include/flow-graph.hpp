#pragma once
#include <vector>
#include <tuple>

#include "common.hpp"



using node_capacity_t = unsigned long long;
using node_idx_t = unsigned int;
// NOTE: The below trick to find the max only works 
//  if the capacity type is unsigned.
#define NODE_CAPACITY_MAX ((node_capacity_t)-2)
#define NODE_IDX_MAX ((node_idx_t)-2)

#define FLOW_GRAPH_DFS 1


class FlowGraph
{
public:
    struct Edge
    {
        node_capacity_t capacity;
        node_idx_t start;
        node_idx_t end;
        node_idx_t reverse_idx;
    };


private:
    const std::vector<ZoneInfo>& zones;
    const node_idx_t source, terminal;
    const node_idx_t node_last;

    std::vector<std::vector<Edge>> graph;


    // const Edge& get_edge(const node_idx_t start, const node_idx_t end) const
    // {
    //     return graph.at(start).at(end);
    // }

    // Edge& get_edge(const node_idx_t start, const node_idx_t end)
    // {
    //     return graph.at(start).at(end);
    // }


    // bool contains_edge(const node_idx_t start, const node_idx_t end)
    // {
    //     auto it = graph.find(start);
    //     if (it == graph.end())
    //         return false;

    //     auto it2 = it->second.find(end);
    //     if (it2 == it->second.end())
    //         return false;

    //     return true;
    // }


public:
    node_idx_t terminal_base_offset;


    FlowGraph() = default;
    FlowGraph(const std::vector<ZoneInfo>& zones, const zone_idx_t source, const zone_idx_t terminal);


    node_idx_t get_input(zone_idx_t zone_idx) const
    {
        return (node_idx_t)zone_idx * ZONE_OBSTACLE_NODE_STRIDE;
    }

    node_idx_t get_output(zone_idx_t zone_idx, zone_obstacle_val_t obstacle) const
    {
        switch (obstacle)
        {
        case O1:
            return (node_idx_t)zone_idx * ZONE_OBSTACLE_NODE_STRIDE + (node_idx_t)1;
        case O3:
            return (node_idx_t)zone_idx * ZONE_OBSTACLE_NODE_STRIDE + (node_idx_t)2;

        case O2:
        case O4:
            return get_input(zone_idx);

        default:
            return -1;
        }
    }


    node_idx_t get_terminal_offset(const ZoneInfo& zone) const
    {
        return terminal_base_offset +
            (zone.capacity[O1] > 0) +
            (zone.capacity[O3] > 0);
    }


    std::vector<Edge>& get_edges_outgoing(const node_idx_t node)
    {
        return graph[node];
    }


    void add_zone_edge
    (
        const zone_idx_t start, 
        const zone_idx_t end, 
        const ZoneObstacle obstacle,
        const zone_capacity_t capacity_forward,
        const zone_capacity_t capacity_backward = 0
    );

    void add_node_edge
    (
        const node_idx_t start, 
        const node_idx_t end, 
        const node_capacity_t capacity_forward,
        const node_capacity_t capacity_backward = 0
    );


    void increment_zone_edge
    (
        const zone_idx_t start, 
        const node_idx_t node_idx,
        const ZoneObstacle obstacle,
        const zone_capacity_t capacity
    );

    void increment_node_edge
    (
        const node_idx_t start, 
        const node_idx_t node_idx,
        const node_capacity_t capacity
    );


    node_capacity_t calculate_maximum_flow();
};