#pragma once
#include <vector>
#include <tuple>

// #include "robin_map.h"
#include <unordered_map>

#include "common.hpp"



using node_capacity_t = unsigned long long;
using node_idx_t = unsigned int;
// NOTE: The below trick to find the max only works 
//  if the capacity type is unsigned.
#define NODE_CAPACITY_MAX ((node_capacity_t)-2)
#define NODE_IDX_MAX ((node_idx_t)-2)

#define FLOW_GRAPH_DFS 1
using node_map_t = std::unordered_map<node_idx_t, node_idx_t>;


class FlowGraph
{
private:
    struct Edge
    {
        node_capacity_t capacity;
        //NOTE: Rehashing does not make pointer invalid
        Edge* reverse;


        Edge()
            : capacity(0), reverse(nullptr)
        {
        }

        Edge(node_capacity_t capacity, Edge* reverse)
            : capacity(capacity), reverse(reverse)
        {
        }
    };

    const std::vector<ZoneInfo>& zones;
    const node_idx_t source, terminal;
    const node_idx_t node_last;


    std::unordered_map<node_idx_t, std::unordered_map<node_idx_t, Edge>> graph;


    const std::unordered_map<node_idx_t, Edge>& get_edges_outgoing(const node_idx_t node) const
    {
        return graph.at(node);
    }

    std::unordered_map<node_idx_t, Edge>& get_edges_outgoing(const node_idx_t node)
    {
        return graph.at(node);
    }


    const Edge& get_edge(const node_idx_t start, const node_idx_t end) const
    {
        return graph.at(start).at(end);
    }

    Edge& get_edge(const node_idx_t start, const node_idx_t end)
    {
        return graph.at(start).at(end);
    }


    bool contains_edge(const node_idx_t start, const node_idx_t end)
    {
        auto it = graph.find(start);
        if (it == graph.end())
            return false;

        auto it2 = it->second.find(end);
        if (it2 == it->second.end())
            return false;

        return true;
    }


    node_map_t find_predecessors(const node_idx_t start, const node_idx_t stop) const;


public:
    FlowGraph() = default;
    FlowGraph(const std::vector<ZoneInfo>& zones, const zone_idx_t source, const zone_idx_t terminal);


    node_idx_t get_input(zone_idx_t zone_idx)
    {
        return (node_idx_t)zone_idx * (ZONE_OBSTACLE_SIZE + 1);
    }

    node_idx_t get_output(zone_idx_t zone_idx, zone_obstacle_val_t obstacle)
    {
        return (node_idx_t)zone_idx * (ZONE_OBSTACLE_SIZE + 1) + 1 + (node_idx_t)obstacle;
    }


    void add_zone_edge
    (
        const zone_idx_t start, 
        const zone_idx_t end, 
        const ZoneObstacle obstacle,
        const zone_capacity_t capacity
    );

    void add_node_edge
    (
        const node_idx_t start, 
        const node_idx_t end, 
        const node_capacity_t capacity
    );


    node_capacity_t calculate_maximum_flow();
};