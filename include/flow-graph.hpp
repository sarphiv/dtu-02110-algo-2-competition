#pragma once
#include <list>
#include <unordered_map>

#include "common.hpp"


#define FLOW_GRAPH_DFS 1

using capacity_t = zone_capacity_t;
// NOTE: The below trick to find the max only works 
//  if the capacity type is unsigned.
#define CAPACITY_MAX ((capacity_t)-1)
using node_idx_t = zone_idx_t;
using node_map_t = std::unordered_map<node_idx_t, node_idx_t>;


#ifdef FLOW_GRAPH_DFS
#include <stack>
#else
#include <queue>
#endif


struct FlowEdge
{
    node_idx_t start;
    node_idx_t end;
    capacity_t capacity;

    FlowEdge(node_idx_t start, node_idx_t end, capacity_t capacity)
        : start(start), end(end), capacity(capacity)
    {
    }
};


class FlowGraph
{
private:
    struct Edge
    {
        capacity_t capacity;
        //NOTE: Rehashing does not make pointer invalid
        Edge* reverse;


        Edge()
            : capacity(0), reverse(nullptr)
        {
        }

        Edge(capacity_t capacity, Edge* reverse)
            : capacity(capacity), reverse(reverse)
        {
        }
    };


    std::unordered_map<node_idx_t, std::unordered_map<node_idx_t, Edge>> graph;
    bool unchanged_since_last_max_flow;
    capacity_t max_flow;



    const std::unordered_map<node_idx_t, Edge>& get_edges_outgoing(const node_idx_t& node) const
    {
        return graph.at(node);
    }

    std::unordered_map<node_idx_t, Edge>& get_edges_outgoing(const node_idx_t& node)
    {
        return graph.at(node);
    }


    const Edge& get_edge(const node_idx_t& start, const node_idx_t& end) const
    {
        return graph.at(start).at(end);
    }

    Edge& get_edge(const node_idx_t& start, const node_idx_t& end)
    {
        return graph.at(start).at(end);
    }


    bool contains_edge(const node_idx_t& start, const node_idx_t& end)
    {
        auto it = graph.find(start);
        if (it == graph.end())
            return false;

        auto it2 = it->second.find(end);
        if (it2 == it->second.end())
            return false;

        return true;
    }


public:
    const node_idx_t node_source;
    const node_idx_t node_terminal;

    FlowGraph() = delete;
    FlowGraph(const node_idx_t &source, const node_idx_t &terminal);
    FlowGraph(const node_idx_t &source, const node_idx_t &terminal, const std::list<FlowEdge> &edges);

    void add_edges(const std::list<FlowEdge>& edges);
    node_map_t find_predecessors(const node_idx_t& start, const node_idx_t& stop) const;
    capacity_t maximize_flow();
};
