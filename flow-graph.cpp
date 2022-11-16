#include "flow-graph.hpp"


FlowGraph::FlowGraph(const node_idx_t& source, const node_idx_t& terminal)
    : unchanged_since_last_max_flow(false), max_flow(0), node_source(source), node_terminal(terminal)
{
}

FlowGraph::FlowGraph(const node_idx_t& source, const node_idx_t& terminal, const std::list<FlowEdge>& edges)
    : unchanged_since_last_max_flow(false), max_flow(0), node_source(source), node_terminal(terminal)
{
    add_edges(edges);
}



void FlowGraph::add_edges(const std::list<FlowEdge>& edges)
{
    for (const auto& edge : edges)
    {
        // Edge already exists, add capacity
        if (contains_edge(edge.start, edge.end))
        {
            auto& forward = get_edge(edge.start, edge.end);
            forward.capacity += edge.capacity;
        }
        else
        {
            // Assign forward edge without reverse edge
            graph[edge.start][edge.end] = Edge(edge.capacity, nullptr);

            // Initialize reverse edge fully
            graph[edge.end][edge.start] = Edge(0, &get_edge(edge.start, edge.end));

            // Set reverse edge for forward edge
            graph[edge.start][edge.end].reverse = &get_edge(edge.end, edge.start);
        }
    }


    unchanged_since_last_max_flow = false;
}



node_map_t FlowGraph::find_predecessors(const node_idx_t& start, const node_idx_t& stop) const
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
    while (!stack.empty())
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



capacity_t FlowGraph::maximize_flow()
{
    // If no new edges, return cached value
    if (unchanged_since_last_max_flow)
        return max_flow;


    // Reset max flow
    max_flow = 0;

    // Store predecessor map of search
    node_map_t predecessors;

    // Find path from source to terminal and augment flow repeatedly
    while ((predecessors = find_predecessors(node_source, node_terminal)).count(node_terminal))
    {
        // Store traversed edges
        std::list<Edge*> path;

        // Find minimum capacity
        capacity_t min_capacity = CAPACITY_MAX;
        node_idx_t node_cur = node_terminal;
        while (node_cur != node_source)
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


    // Update cached mark
    unchanged_since_last_max_flow = true;

    // Return max flow
    return max_flow;
}

