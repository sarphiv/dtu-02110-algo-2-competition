#include <list>
#include <tuple>
#include <numeric>

#include "robin_map.h"

#include "common.hpp"
#include "obstacle-solver-3.hpp"



void ObstacleSolver3::solve()
{
    // Counter for zones on the same line (all lines go through zone_i)
    // NOTE: Defined out here to avoid reallocating memory
    tsl::robin_map<double, zone_idx_t> slope_counter;


    // Loop through all zones sorted by x and y
    for (zone_idx_t i = 0; i < zones_sorted.size(); ++i)
    {
        // Cache first zone
        const auto& zone_i = zones_sorted[i];

        // Clear slope counter to count for current zone_i only
        slope_counter.clear();


        // Loop through all zones "in front"
        // NOTE: Have to loop through even if there is no capacity.
        //  This is because zones in front need to connect back to this zone.
        for (zone_idx_t j = i+1; j < zones_sorted.size(); ++j)
        {
            // Cache second zone
            const auto& zone_j = zones_sorted[j];


            // Increment slope zone counter
            // NOTE: dx is never negative. This hash is unique for each line.
            const double slope = 
                  (double)((zone_coord_signed_t)zone_j.y - (zone_coord_signed_t)zone_i.y) 
                / (double)(zone_j.x - zone_i.x);
            const auto count = ++slope_counter[slope];


            // If more than two zones are in front of zone_i on the same line, add edges.
            if (count > 2)
            {
                // NOTE: Edges are not mirrored. 
                //  The edges start from real output nodes rather than virtual output nodes.
                //  A virtual output node is an input node also working as an output node.

                // If capacity available, add edge from zone_i to zone_j
                if ((zone_i.capacity[O3] > 0))
                    graph.add_zone_edge
                    (
                        zone_i.idx,
                        zone_j.idx,
                        O3,
                        zone_i.capacity[O3]
                    );

                // If capacity available, add edge from zone_j to zone_i
                if (zone_j.capacity[O3] > 0)
                    graph.add_zone_edge
                    (
                        zone_j.idx,
                        zone_i.idx,
                        O3,
                        zone_j.capacity[O3]
                    );
            }
        }
    }
}