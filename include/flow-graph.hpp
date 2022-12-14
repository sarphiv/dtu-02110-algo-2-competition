#pragma once
#include <vector>

#include "common.hpp"



using node_capacity_t = long long;
using node_idx_t = unsigned int;
// NOTE: The below trick to find the max only works 
//  if the capacity type is unsigned.
#define NODE_CAPACITY_MAX ((node_capacity_t)-2)
#define NODE_IDX_MAX ((node_idx_t)-1)


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
    const ZoneInfos& zones;
    const node_idx_t source, terminal;
    const node_idx_t node_size;

    // Adjacency list of graph
    std::vector<std::vector<Edge>> graph;
    // Excess flow of each node
    std::vector<node_capacity_t> excess;
    // Height of each node, and amount of nodes at each height
    std::vector<node_idx_t> height, count;
    // Nodes enqueued for discharge
    // Maps height to stack of nodes to discharge at that height
    std::vector<std::vector<node_idx_t>> discharge_stack;
    std::vector<bool> active;
    // Max height of active (enqueued) nodes
    // NOTE: So this only includes nodes where excess flow has not flowed back to source
    node_idx_t active_max_height;


    void push(Edge &edge);
    void relabel(const node_idx_t node);
    void discharge(node_idx_t v);
    void relabel_gap(node_idx_t k);
    void discharge_stack_enqueue(node_idx_t v);

    std::vector<node_idx_t> max_height_nodes();
    

public:
    FlowGraph(const ZoneInfos& zones, const zone_idx_t source, const zone_idx_t terminal);


    node_idx_t get_input(const zone_idx_t zone_idx) const
    {
        return (node_idx_t)zone_idx * ZONE_OBSTACLE_NODE_STRIDE;
    }

    node_idx_t get_output(const zone_idx_t zone_idx, const zone_obstacle_val_t obstacle) const
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
        return (zone.capacity[O1] > 0) + (zone.capacity[O3] > 0);
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