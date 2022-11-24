#include <list>
#include <tuple>
#include <numeric>

// #define EMH_REHASH_LOG 1
#define EMH_SIZE_TYPE_64BIT 1
#define EMH_LRU_SET 1
#include "emhash7.hpp"

#include "common.hpp"
#include "obstacle-solver-3.hpp"



struct hash_double
{
    inline std::size_t operator()(const double& val) const
    {
        // NOTE: No need to handle -0.0 vs. 0.0 because never -0.0.
        //  This is because delta_x further below is >= 0.0,
        //  and all points are unique.

        // // Based upon xxHash at: https://github.com/Cyan4973/xxHash
        int64_t hash = *((int64_t*)(&val));
        hash ^= hash >> 33;
        hash *= 14029467366897019727UL;
        hash ^= hash >> 29;
        // hash *= 1609587929392839161UL;
        // hash ^= hash >> 32;
        return hash;
    }
};


void ObstacleSolver3::solve()
{
    // Counter for zones on the same line (all lines go through zone_i)
    // NOTE: Defined out here to avoid reallocating memory
    emhash7::HashMap<double, zone_idx_t, hash_double> slope_counter;
    slope_counter.reserve(zones_sorted.size()*1.7);
    slope_counter.max_load_factor(0.6);


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
            // NOTE: dx is never negative, and if dx == 0, then dy > 0, so always +inf
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