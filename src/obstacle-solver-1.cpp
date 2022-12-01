#include <vector>

#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"



void ObstacleSolver1::connect_range(const std::vector<zone_idx_t>& zone_idx, const std::vector<zone_capacity_t>& zone_cap, const zone_idx_t start, const zone_idx_t end)
{
    // Loop over all zones in the range
    for (zone_idx_t i = start; i <= end; i++)
    {
        // Get zone ID and capacity
        const auto idx_i = zone_idx[i];
        const auto cap_i = zone_cap[i];

        // If zone has capacity, connect it everything else
        if (cap_i > 0)
            for (zone_idx_t j = start; j <= end; j++)
                if (i != j)
                    graph.add_zone_edge
                    (
                        idx_i, 
                        zone_idx[j],
                        O1, 
                        cap_i
                    );
    }
}


void ObstacleSolver1::solve()
{
    // Find sequences in x coordinate
    find_sequences<zone_coord_t, zone_idx_t>
    (
        zones_sorted.x,
        [&](const zone_idx_t start, const zone_idx_t end)
        { 
            connect_range(zones_sorted.idx, zones_sorted.capacity[O1], start, end);
        }
    );



    // Sort by y coordinate
    std::vector<zone_coord_t> zones_sorted_y_y(zones_sorted.y);
    std::vector<zone_idx_t> zones_sorted_y_idx(zones_sorted.idx);
    std::vector<zone_capacity_t> zones_sorted_y_cap(zones_sorted.capacity[O1]);
    const auto idx = argsort<zone_coord_t, zone_idx_t>
    (
        zones_sorted_y_y, 
        [&](const zone_idx_t a, const zone_idx_t b)
        {
            return zones_sorted_y_y[a] < zones_sorted_y_y[b];
        }
    );
    argsort<zone_coord_t, zone_idx_t>(zones_sorted_y_idx, idx);
    argsort<zone_coord_t, zone_idx_t>(zones_sorted_y_cap, idx);


    // Find sequences in y coordinate
    find_sequences<zone_coord_t, zone_idx_t>
    (
        zones_sorted_y_y,
        [&](const zone_idx_t start, const zone_idx_t end)
        { 
            connect_range(zones_sorted_y_idx, zones_sorted_y_cap, start, end); 
        }
    );
}