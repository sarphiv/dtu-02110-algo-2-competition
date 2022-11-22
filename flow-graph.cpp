#include <vector>
#include <utility>
#include <list>

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
    node_last(this->terminal + ZONE_OBSTACLE_SIZE)
{
    // Add all internal edges for each zone
    for (zone_idx_t i = 0; i < zones.size(); ++i)
        for (zone_obstacle_val_t j = 0; j < ZONE_OBSTACLE_SIZE; ++j)
            if (zones[i].capacity[j] > 0)
                add_node_edge
                (
                    get_input(zones[i].idx),
                    get_output(zones[i].idx, j),
                    zones[i].capacity[j]
                );
}


void FlowGraph::add_zone_edge
(
    const zone_idx_t start, 
    const zone_idx_t end, 
    const ZoneObstacle obstacle,
    const zone_capacity_t capacity
)
{
    // Get node indices
    const auto node_start = get_output(start, obstacle);
    const auto node_end   = get_input(end);

    // Add edge
    add_node_edge(node_start, node_end, capacity);
}

void FlowGraph::add_node_edge
(
    const node_idx_t node_start, 
    const node_idx_t node_end, 
    const node_capacity_t capacity
)
{
    // Edge already exists, add capacity
    if (contains_edge(node_start, node_end))
    {
        auto& forward = get_edge(node_start, node_end);
        forward.capacity += capacity;
    }
    else
    {
        // Assign forward edge without reverse edge
        graph[node_start][node_end] = Edge(capacity, nullptr);

        // Initialize reverse edge fully
        graph[node_end][node_start] = Edge(0, &get_edge(node_start, node_end));

        // Set reverse edge for forward edge
        graph[node_start][node_end].reverse = &get_edge(node_end, node_start);
    }
}




node_map_t FlowGraph::find_predecessors(const node_idx_t start, const node_idx_t stop) const
{
    // Predecessor index map
    node_map_t predecessor;


    #ifdef FLOW_GRAPH_DFS
    // Create DFS stack
    std::stack<node_idx_t> stack;

    // Enqueue source node
    stack.push(start);
    predecessor[start] = start;

    // Depth first search
    while (!stack.empty())
    {
        node_idx_t const parent = stack.top();
        stack.pop();

        for (const auto& [child, edge] : get_edges_outgoing(parent))
        {
            if (edge.capacity == 0)
                continue;
            else if (!predecessor.count(child))
            {
                predecessor[child] = parent;
                stack.push(child);
            }

            if (child == stop)
                return predecessor;
        }
    }
    
#else
    // Create BFS queue
    std::queue<node_idx_t> queue;

    // Enqueue source node
    queue.push(start);
    predecessor[start] = start;

    // Breadth first search
    while (!queue.empty())
    {
        node_idx_t const parent = queue.front();
        queue.pop();

        for (const auto& [child, edge] : get_edges_outgoing(parent))
        {
            if (edge.capacity == 0)
                continue;
            else if (!predecessor.count(child))
            {
                predecessor[child] = parent;
                queue.push(child);
            }

            if (child == stop)
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
    node_map_t predecessors;

    // Find path from source to terminal and augment flow repeatedly
    while ((predecessors = find_predecessors(source, terminal)).count(terminal))
    {
        // Store traversed edges
        std::list<Edge*> path;

        // Find minimum capacity
        node_capacity_t min_capacity = NODE_CAPACITY_MAX;
        node_idx_t node_cur = terminal;
        while (node_cur != source)
        {
            auto node_prev = predecessors[node_cur];
            auto& edge = get_edge(node_prev, node_cur);
            path.push_front(&edge);
            node_cur = node_prev;

            if (edge.capacity < min_capacity)
                min_capacity = edge.capacity;
        }


        // Update graph
        for (auto& edge : path)
        {
            edge->capacity -= min_capacity;
            edge->reverse->capacity += min_capacity;
        }


        // Update max flow
        max_flow += min_capacity;
    }


    // Return max flow
    return max_flow;
}

