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
    node_size(this->terminal + ZONE_OBSTACLE_NODE_STRIDE),
    graph(node_size, std::vector<Edge>()),
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







// Push-Relabel based on: https://github.com/mochow13/competitive-programming-library/blob/master/Graph/Push%20Relabel%202.cpp
void FlowGraph::enqueue(node_idx_t v) {
    if (!active[v] && excess[v] > 0 && dist[v] < node_size) {
        active[v] = true;
        B[dist[v]].push_back(v);
        b = std::max(b, dist[v]);
    }
}

void FlowGraph::push(Edge& edge) {
    node_capacity_t amt = std::min(excess[edge.start], edge.capacity);
    if (dist[edge.start] == dist[edge.end] + 1 && amt > 0) {
        edge.capacity -= amt;
        graph[edge.end][edge.reverse_idx].capacity += amt;
        excess[edge.end] += amt;    
        excess[edge.start] -= amt;
        enqueue(edge.end);
    }
}

void FlowGraph::gap(node_idx_t k) {
    for (node_idx_t v = 0; v < node_size; v++) if (dist[v] >= k) {
        --count[dist[v]];
        dist[v] = std::max(dist[v], node_size);
        ++count[dist[v]];
        enqueue(v);
    }
}

void FlowGraph::relabel(node_idx_t v) {
    count[dist[v]]--;
    dist[v] = node_size;
    for (const auto& edge: graph[v]) if (edge.capacity > 0) {
        dist[v] = std::min(dist[v], dist[edge.end] + 1);
    }
    count[dist[v]]++;
    enqueue(v);
}

void FlowGraph::discharge(node_idx_t v) {
    for (auto &edge: graph[v]) {
        if (excess[v] > 0) {
            push(edge);
        } else {
            break;
        }
    }

    if (excess[v] > 0) {
        if (count[dist[v]] == 1) {
            gap(dist[v]); 
        } else {
            relabel(v);
        }
    }
}

node_capacity_t FlowGraph::calculate_maximum_flow() {   
    b = 0;
    dist.assign(node_size, 0);
    excess.assign(node_size, 0);
    count.assign(node_size + 1, 0);
    active.assign(node_size, false);
    B = std::vector<std::vector<node_idx_t>>(node_size, std::vector<node_idx_t>());


    for (const auto &edge: graph[source]) {
        excess[source] += edge.capacity;
    }

    count[0] = node_size;
    enqueue(source);
    active[terminal] = true;
    
    while (b != NODE_IDX_MAX) {
        if (!B[b].empty()) {
            node_idx_t v = B[b].back();
            B[b].pop_back();
            active[v] = false;
            discharge(v);
        } else {
            b--;
        }
    }

    return excess[terminal];
}
