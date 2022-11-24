#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"



void ObstacleSolver1::connect_range(const std::vector<ZoneInfo>& zones, const zone_idx_t start, const zone_idx_t end)
{
    // Loop over all zones in the range
    for (zone_idx_t i = start; i <= end; i++)
    {
        // Get zone ID and capacity
        const auto& zone_i = zones[i];
        auto capacity = zone_i.capacity[O1];

        // If zone has capacity, connect it everything else
        if (capacity > 0)
            for (zone_idx_t j = start; j <= end; j++)
                if (i != j)
                    graph.add_zone_edge
                    (
                        zone_i.idx, 
                        zones[j].idx,
                        O1, 
                        capacity
                    );
    }
}


void ObstacleSolver1::solve()
{
    // Find sequences in x coordinate
    find_sequences<ZoneInfo, zone_coord_t, zone_idx_t>
    (
        zones_sorted,
        [&](const ZoneInfo& zone)
        {
            return zone.x;
        },
        [&](const zone_idx_t start, const zone_idx_t end)
        { 
            connect_range(zones_sorted, start, end); 
        }
    );


    // Sort by y coordinate
    std::vector<ZoneInfo> zones_sorted_y(zones_sorted);
    argsort<ZoneInfo, zone_idx_t>
    (
        zones_sorted_y, 
        [&](const zone_idx_t a, const zone_idx_t b)
        {
            return zones_sorted_y[a].y < zones_sorted_y[b].y;
        }
    );

    // Find sequences in y coordinate
    find_sequences<ZoneInfo, zone_coord_t, zone_idx_t>
    (
        zones_sorted_y,
        [&](const ZoneInfo& zone)
        {
            return zone.y;
        },
        [&](const zone_idx_t start, const zone_idx_t end)
        { 
            connect_range(zones_sorted_y, start, end); 
        }
    );
}