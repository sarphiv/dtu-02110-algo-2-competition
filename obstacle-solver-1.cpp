#include "common.hpp"
#include "common-templates.hpp"
#include "obstacle-solver-1.hpp"



void ObstacleSolver1::connect_range(const std::vector<zone_coord_t>& val, const std::vector<zone_idx_t>& idx, const zone_idx_t& start, const zone_idx_t& end)
{
    // Loop over all zones in the range
    for (zone_idx_t i = start; i <= end; i++)
    {
        // Get zone ID and capacity
        auto id = idx[i];
        auto capacity = zones_sorted[zones_idx_inv[id]].capacity[O1];

        // If zone has capacity, connect it everything else
        if (capacity > 0)
            for (zone_idx_t j = start; j <= end; j++)
                if (i != j)
                    graph_builder.add_edge
                    (
                        id, 
                        idx[j],
                        O1, 
                        capacity
                    );
    }
}


void ObstacleSolver1::solve()
{
    find_sequences<zone_coord_t, zone_idx_t>
    (
        x_sorted, 
        [&](const zone_idx_t &start, const zone_idx_t &end)
        { 
            connect_range(x_sorted, x_idx, start, end); 
        }
    );
    find_sequences<zone_coord_t, zone_idx_t>
    (
        y_sorted, 
        [&](const zone_idx_t &start, const zone_idx_t &end)
        { 
            connect_range(y_sorted, y_idx, start, end);
        }
    );
}