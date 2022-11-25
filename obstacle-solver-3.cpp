#include <vector>

// #define EMH_REHASH_LOG 1
// #define EMH_STATIS 1
#define EMH_SIZE_TYPE_64BIT 1
#define EMH_LRU_SET 1
#include "emhash7.hpp"

#include "common.hpp"
#include "obstacle-solver-3.hpp"



void ObstacleSolver3::solve()
{
    // NOTE: Lots of comments because this function is the bottleneck of the program.
    //  Lots of crazy optimization for the hash map has been tried, measured, and discarded.
    //  The below are the ones that worked.


    // Cache zones_sorted size
    // NOTE: This actually helps by a few percentage points - was measured.
    const zone_idx_t zones_size = zones_sorted.size();

    // Counter for zones on the same line (all lines go through zone_i)
    // NOTE: Defined out here to avoid reallocating memory
    emhash7::HashMap<int64_t, zone_idx_t> slope_counter;
    slope_counter.reserve(zones_size*1.7);
    slope_counter.max_load_factor(0.6);
    
    // Temporary cache storage for slopes
    std::vector<double> slopes(zones_size);


    // Loop through all zones sorted by x and y
    for (zone_idx_t i = 0; i < zones_size; ++i)
    {
        // Cache first zone
        const auto& zone_i = zones_sorted[i];

        // Clear slope counter to count for current zone_i only
        slope_counter.clear();


        // Loop through all zones "in front"
        // NOTE: Have to loop through even if there is no capacity.
        //  This is because zones in front need to connect back to this zone.
        // NOTE: Doing it over two loops because of better performance through cache locality and vectorization.
        //  This comes at the cost of increased space usage via the slopes vector.
        for (zone_idx_t j = i+1; j < zones_size; ++j)
            // Increment slope zone counter
            // NOTE: dx is never negative, and if dx == 0, then dy > 0, so always +inf if infinite
            // NOTE: Somehow the floating point math is accurate enough for hashing.
            //  Guessing it is because only one floating point operation happens on fully accurate numbers,
            //  so errors newer get to compound inconsistently.
            slopes[j] = (double)((zone_coord_signed_t)zones_sorted[j].y - (zone_coord_signed_t)zone_i.y) 
                / (double)(zones_sorted[j].x - zone_i.x);


        // Loop through all zones processed above, this time to add edges
        for (zone_idx_t j = i+1; j < zones_size; ++j)
        {
            // Manually hash slope to integer outside of emhash.
            // NOTE: Increases performance since integers have identity hash in emhash by default.
            //  Also, this is faster than giving emhash a custom hasher.
            // NOTE: No need to handle -0.0 vs. 0.0 because never -0.0.
            //  This is because delta_x is always >= 0.0, and all points are unique.

            // // Based upon xxHash at: https://github.com/Cyan4973/xxHash
            int64_t hash = *((int64_t*)(&slopes[j]));
            hash ^= hash >> 33;
            hash *= 14029467366897019727UL;
            hash ^= hash >> 29;
            // NOTE: Uncommented because of increased speed for large inputs,
            //  while little to no difference for small inputs.
            // hash *= 1609587929392839161UL;
            // hash ^= hash >> 32;


            // If more than two zones are in front of zone_i on the same line, add edges.
            if (++slope_counter[hash] > 2)
            {
                // Cache second zone
                const auto& zone_j = zones_sorted[j];

                // NOTE: Edges are not mirrored. 
                //  The edges start from real output nodes rather than virtual output nodes.
                //  A virtual output node is an input node also working as an output node.

                // If capacity available, add edge from zone_i to zone_j
                if (zone_i.capacity[O3] > 0)
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