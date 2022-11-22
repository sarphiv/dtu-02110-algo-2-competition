#include <vector>
#include <utility>
#include <list>

#include "robin_map.h"







#include <iostream>








#include "common.hpp"
#include <flow-graph.hpp>


#ifdef FLOW_GRAPH_DFS
#include <stack>
#else
#include <queue>
#endif



FlowGraph::FlowGraph(const std::vector<ZoneInfo>& zones, const zone_idx_t source, const zone_idx_t terminal)
    : zones(zones), 
    source(get_input(source)), 
    terminal(get_input(terminal)),
    node_last(this->terminal + ZONE_OBSTACLE_NODE_STRIDE - 1),
    graph(node_last + 1, std::vector<Edge>()),
    terminal_base_offset(0)
{
    // Add all internal edges for each zone
    for (zone_idx_t i = 0; i < zones.size(); ++i)
    {
        // Cache zone info
        const auto zone = zones[i];

        // Add internal edge for obstacle O1
        if (zone.capacity[O1] > 0)
            add_node_edge
            (
                get_input(zone.idx),
                get_output(zone.idx, O1),
                zone.capacity[O1],
                0
            );

        // Add internal edge for obstacle O3
        if (zone.capacity[O3] > 0)
            add_node_edge
            (
                get_input(zone.idx),
                get_output(zone.idx, O3),
                zone.capacity[O3],
                0
            );
    }
}



void FlowGraph::add_zone_edge
(
    const zone_idx_t start, 
    const zone_idx_t end, 
    const ZoneObstacle obstacle,
    const zone_capacity_t capacity_forward,
    const zone_capacity_t capacity_backward
)
{
    std::cout << start << " " << end << " " << capacity_forward << std::endl;
    add_node_edge
    (
        get_output(start, obstacle),
        get_input(end),
        capacity_forward,
        capacity_backward
    );
}

void FlowGraph::add_node_edge
(
    const node_idx_t start, 
    const node_idx_t end, 
    const node_capacity_t capacity_forward,
    const node_capacity_t capacity_backward
)
{
    // Get outgoing edge vectors
    auto& forward = graph[start];
    auto& backward = graph[end];

    // Add residual graph edges
    forward.push_back(Edge{capacity_forward, start, end, (node_idx_t)backward.size()});
    backward.push_back(Edge{capacity_backward, end, start, (node_idx_t)forward.size() - 1});
}


void FlowGraph::increment_zone_edge
(
    const zone_idx_t start, 
    const node_idx_t node_idx,
    const ZoneObstacle obstacle,
    const zone_capacity_t capacity
)
{
    increment_node_edge
    (
        get_output(start, obstacle), 
        node_idx, 
        capacity
    );
}

void FlowGraph::increment_node_edge
(
    const node_idx_t start, 
    const node_idx_t node_idx,
    const node_capacity_t capacity
)
{
    graph[start][node_idx].capacity += capacity;
}






tsl::robin_map<node_idx_t, FlowGraph::Edge*> find_predecessors
(
    FlowGraph& graph, 
    const node_idx_t start, 
    const node_idx_t stop
)
{
    // Predecessor index map
    // NOTE: Not mutating graph vectors anymore, so addresses should stay valid.
    tsl::robin_map<node_idx_t, FlowGraph::Edge*> predecessor;


    #ifdef FLOW_GRAPH_DFS
    // Create DFS stack
    std::stack<node_idx_t> stack;

    // Enqueue source node
    stack.push(start);
    predecessor[start] = nullptr;


    // Depth first search
    while (!stack.empty())
    {
        node_idx_t const parent = stack.top();
        stack.pop();

        auto& edges = graph.get_edges_outgoing(parent);
        for (node_idx_t edge_idx = 0; edge_idx < edges.size(); ++edge_idx)
        {
            auto& edge = edges[edge_idx];

            if (edge.capacity == 0)
                continue;

            if (!predecessor.contains(edge.end))
            {
                predecessor[edge.end] = &edge;
                stack.push(edge.end);
            }


            if (edge.end == stop)
                return predecessor;
        }
    }
    
#else
    // Create BFS queue
    std::queue<node_idx_t> queue;

    // Enqueue source node
    queue.push(start);
    predecessor[start] = nullptr;

    // Breadth first search
    while (!queue.empty())
    {
        node_idx_t const parent = queue.front();
        queue.pop();

        for (auto& edge : get_edges_outgoing(parent))
        {
            if (edge.capacity == 0)
                continue;
            else if (!predecessor.contains(edge.end))
            {
                predecessor[edge.end] = &edge;
                queue.push(edge.end);
            }

            if (edge.end == stop)
                return predecessor;
        }
    }
#endif


    // No path found, return reached nodes
    return predecessor;
}



node_capacity_t FlowGraph::calculate_maximum_flow()
{
    // Storage for max flow
    node_capacity_t max_flow = 0;


    // Store predecessor map of search
    tsl::robin_map<node_idx_t, FlowGraph::Edge*> predecessors;

    // Find path from source to terminal and augment flow repeatedly
    while ((predecessors = find_predecessors(*this, source, terminal)).contains(terminal))
    {
        // Store traversed edges
        std::vector<Edge*> path;

        // Find minimum capacity
        node_capacity_t min_capacity = NODE_CAPACITY_MAX;
        node_idx_t node_curr = terminal;
        while (node_curr != source)
        {
            auto& edge_prev = *predecessors[node_curr];
            path.push_back(&edge_prev);
            node_curr = edge_prev.start;

            if (edge_prev.capacity < min_capacity)
                min_capacity = edge_prev.capacity;
        }


        // Update graph
        for (auto edge_ptr : path)
        {
            auto& edge = *edge_ptr;
            edge.capacity -= min_capacity;
            graph[edge.end][edge.reverse_idx].capacity += min_capacity;
        }


        // Update max flow
        max_flow += min_capacity;
    }


    // Return max flow
    return max_flow;
}

