#include <vector>
#include <utility>
#include <list>

#include "common.hpp"
#include <flow-graph.hpp>



FlowGraph::FlowGraph(const std::vector<ZoneInfo>& zones, const zone_idx_t source, const zone_idx_t terminal)
    : zones(zones), 
    source(get_input(source)), terminal(get_input(terminal)),
    // NOTE: Not adding more, because terminal node has no output nodes
    node_size(this->terminal + 1),

    graph(node_size, std::vector<Edge>()),
    excess(node_size, 0),
    height(node_size, 0), count(node_size+2, 0),
    discharge_stack(node_size, std::vector<node_idx_t>()),
    active(node_size, false),
    active_max_height(0),
    terminal_base_offset(0)
{
    // Mark terminal as enqueued to prevent requeueing
    //  as there should never be a discharge from the terminal
    active[this->terminal] = true;

    // All nodes start at height 0
    count[0] = node_size;


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







// Push-Relabel (max height version) based heavily on: https://github.com/mochow13/competitive-programming-library/blob/master/Graph/Push%20Relabel%202.cpp
// Explanation for base algorithm: http://www.adrian-haarbach.de/idp-graph-algorithms/implementation/maxflow-push-relabel/index_en.html

void FlowGraph::discharge_stack_enqueue(node_idx_t node_idx)
{
    // If node is not already in queue, 
    //  and there is excess to discharge, 
    //  and excess has not flowed back to source from it,
    //  enqueue node to stack for discharge.
    if ((!active[node_idx]) & (excess[node_idx] > 0) & (height[node_idx] < node_size))
    {
        // Mark node as enqueued
        active[node_idx] = true;

        // Enqueue to associated height stack
        // NOTE: Some heights may be above node_size, 
        //  but those are filtered out by the height check above.
        discharge_stack[height[node_idx]].push_back(node_idx);

        // Update enqueued max height
        active_max_height = std::max(active_max_height, height[node_idx]);
    }
}


void FlowGraph::push(Edge& edge)
{
    // Calculate minimum excess that can be pushed
    node_capacity_t delta = std::min(excess[edge.start], edge.capacity);

    // Push allowable excess
    edge.capacity -= delta;
    graph[edge.end][edge.reverse_idx].capacity += delta;

    // Update excess
    excess[edge.end] += delta;    
    excess[edge.start] -= delta;

    // Enqueue end node, because it now has excess
    discharge_stack_enqueue(edge.end);
}


void FlowGraph::relabel_gap(node_idx_t height_limit)
{
    // Loop through all heights for nodes
    for (node_idx_t node_idx = 0; node_idx < height.size(); node_idx++) 
    {
        // If node is above or equal to limit, 
        //  ensure it is at least node_size high and enqueue for discharge (likely relabel)
        if (height[node_idx] >= height_limit)
        {
            if (height[node_idx] < node_size)
            {
                --count[height[node_idx]];
                ++count[node_size];
                height[node_idx] = node_size;
            }

            discharge_stack_enqueue(node_idx);
        }
    }
}


void FlowGraph::relabel(node_idx_t node_idx)
{
    // Relabelling, so needs to subtract one from counter for associated height
    count[height[node_idx]]--;

    // Find minimum height of reachable neighbors
    node_idx_t min_height = node_size;
    for (const auto& edge: graph[node_idx])
        if ((edge.capacity > 0) & (height[edge.end] < min_height)) 
            min_height = height[edge.end];

    // Update height
    height[node_idx] = min_height + 1;
    count[min_height + 1]++;

    // Enqueue again because there was excess left to discharge since it was relabelled
    discharge_stack_enqueue(node_idx);
}


void FlowGraph::discharge(node_idx_t node_idx)
{
    // Attempt discharging from node to all neighbors
    for (auto& edge: graph[node_idx])
    {
        // If no more excess to discharge, break
        if (excess[node_idx] == 0)
            break;
        // Else if capable of pushing, push excess along edge
        else if ((height[edge.start] == height[edge.end] + 1) & (edge.capacity > 0))
            push(edge);
    }


    // If there is still excess, node needs to be relabeled
    if (excess[node_idx] > 0)
    {
        // If there are no more nodes of this height, relabel all nodes above or equal to this height
        // NOTE: Since excess only flows from height + 1 to height,
        //  the flow will become broken by relabelling.
        //  Needs to relabel multiple nodes to ensure flow is not broken from source.
        //  The call to relabel_gap will cause all nodes above this height to be relabelled via the discharge queue.
        if (count[height[node_idx]] == 1)
            relabel_gap(height[node_idx]); 
        // Else, relabel node to be one higher than minimum height reachable neighbor
        else
            relabel(node_idx);
    }
}

node_capacity_t FlowGraph::calculate_maximum_flow()
{
    // WARN: This function may only be called once.
    //  It may not work if called again, because mutations are not reset.


    // Preload source with excess equal to maximum possible flow out
    for (const auto &edge: graph[source])
        excess[source] += edge.capacity;

    // Enqueue source for discharge
    discharge_stack_enqueue(source);


    // While there are still nodes with excess flow to discharge
    // NOTE: Intentionally waiting for overflow to max to occur
    while (active_max_height != NODE_IDX_MAX)
    {
        // If node available in associated height stack, discharge it
        if (!discharge_stack[active_max_height].empty())
        {
            // Retrieve node
            node_idx_t node_idx = discharge_stack[active_max_height].back();
            discharge_stack[active_max_height].pop_back();
            active[node_idx] = false;

            // Discharge node
            discharge(node_idx);
        }
        // Else, there were no nodes, decrement active max height
        // NOTE: Intentionally waiting for overflow to max to occur
        else 
            active_max_height--;
    }


    // Return accumulated excess in terminal node (maximum flow)
    return excess[terminal];
}
