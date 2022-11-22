#include <vector>
#include <utility>
#include <list>

#include "robin_map.h"

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




void FlowGraph::push(Edge& edge)
{
    node_capacity_t delta = std::min(excess[edge.start], edge.capacity);
    edge.capacity -= delta;
    graph[edge.end][edge.reverse_idx].capacity += delta;
    excess[edge.start] -= delta;
    excess[edge.end] += delta;
}


void FlowGraph::relabel(const node_idx_t node)
{
    node_idx_t delta = NODE_IDX_MAX;
    for (const auto& edge : graph[node])
        if (edge.capacity > 0)
            delta = std::min(delta, height[edge.end]);

    if (delta < NODE_IDX_MAX)
        height[node] = delta + 1;
}

std::vector<node_idx_t> FlowGraph::max_height_nodes() {
    std::vector<node_idx_t> max_height;
    for (node_idx_t i = 0; i < height.size(); i++) {
        if (i != source && i != terminal && excess[i] > 0) {
            if (!max_height.empty() && height[i] > height[max_height[0]])
                max_height.clear();
            if (max_height.empty() || height[i] == height[max_height[0]])
                max_height.push_back(i);
        }
    }
    return max_height;
}

node_capacity_t FlowGraph::calculate_maximum_flow()
{
    height.assign(node_last+1, 0);
    height[source] = node_last+1;

    excess.assign(node_last+1, 0);
    excess[source] = NODE_CAPACITY_MAX;


    for (auto& edge : graph[source])
        push(edge);


    std::vector<node_idx_t> current;
    while (!(current = max_height_nodes()).empty()) 
    {
        for (node_idx_t i : current) 
        {
            bool pushed = false;
            for (auto& edge : graph[i])
            {
                if (!excess[i])
                    break;


                if (edge.capacity > 0 && height[i] == height[edge.end] + 1) {
                    push(edge);
                    pushed = true;
                }
            }

            if (!pushed) {
                relabel(i);
                break;
            }
        }
    }


    return -excess[source];
}