#include <list>

#include <graph-builder.hpp>



std::list<FlowEdge> GraphBuilder::build()
{
    // Store the edges of a flow graph
    std::list<FlowEdge> flow_edges(edges.size());


    // All internal edges for each zone
    for (zone_idx_t i = 0; i < zones_sorted.size(); ++i)
        for (zone_obstacle_val_t j = 0; j < ZONE_OBSTACLE_SIZE; ++j)
            if (zones_sorted[i].capacity[j] > 0)
                flow_edges.push_back
                ({
                    // NOTE: Adding 1 because an input node also needs to be added for each zone.
                    get_zone_input(zones_idx[i]),
                    get_zone_output(zones_idx[i], j),
                    zones_sorted[i].capacity[j]
                });

    // Add edges between zones
    for (const auto &edge : edges)
        flow_edges.push_back
        ({
            get_zone_output(edge.start, edge.obstacle),
            get_zone_input(edge.end),
            edge.capacity
        });



    // Return flow edges
    return flow_edges;
}