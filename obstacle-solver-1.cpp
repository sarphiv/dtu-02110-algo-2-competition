#include "common.hpp"
#include "obstacle-solver-1.hpp"



void ObstacleSolver1::connect_range(const std::vector<zone_coord_t>& val, const std::vector<zone_idx_t>& idx, const zone_idx_t& start, const zone_idx_t& end)
{
    // Loop over all zones in the range
    for (zone_idx_t i = start; i <= end; i++)
    {
        // Get zone ID and capacity
        auto id = idx[i];
        auto capacity = zones[id].capacity[O1];

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


void ObstacleSolver1::solve(const std::vector<zone_coord_t>& val, const std::vector<zone_idx_t>& idx)
{
    zone_idx_t zone_count = zones.size();

    // Find all sequences of common values 
    //  by iterating over sorted values 
    //  and finding sequences of equal values
    zone_idx_t start = 0;
    zone_idx_t end = 0;
    zone_coord_t prev_val = val[idx[0]];

    for (zone_idx_t i = 1; i < zone_count; i++)
    {
        if (prev_val != val[i])
        {
            if (start != end)
                connect_range(val, idx, start, end);

            start = end;
            prev_val = val[i];
        }

        end = i;
    }


    // If there is a final sequence, connect it
    if (start != end)
        connect_range(val, idx, start, end);
}


void ObstacleSolver1::solve()
{
    solve(x_sorted, x_idx);
    solve(y_sorted, y_idx);
}