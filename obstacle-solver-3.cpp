#include <robin_map.h>
#include <list>
#include <tuple>
#include <numeric>


#include "common.hpp"
#include "obstacle-solver-3.hpp"


#define OBSTACLE_SOLVER_3_SLOPE_SEED 34127


struct Slope
{
    zone_coord_signed_t dx;
    zone_coord_signed_t dy;

    Slope()
        : dx(0), dy(0)
    {
    }
    Slope(zone_coord_signed_t dx, zone_coord_signed_t dy)
        : dx(dx), dy(dy)
    {
    }

    bool operator==(const Slope& other) const
    {
        return dx == other.dx && dy == other.dy;
    }
};

namespace std 
{
    template <>
    struct hash<Slope>
    {
        std::size_t operator()(const Slope& s) const
        {
            // DISCLAIMER: Based upon boost::hash_combine

            std::hash<zone_coord_signed_t> hasher;
            size_t seed = OBSTACLE_SOLVER_3_SLOPE_SEED;
            seed ^= (hasher(s.dx) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            seed ^= (hasher(s.dy) + 0x9e3779b9 + (seed << 6) + (seed >> 2));

            return seed;
        }
    };
}



void ObstacleSolver3::solve()
{
    // Counter for zones on the same line (all lines go through zone_i)
    // NOTE: Defined out here to avoid reallocating memory
    // std::unordered_map<Slope, zone_idx_t> slope_counter;
    tsl::robin_map<Slope, zone_idx_t> slope_counter;


    // Loop through all zones sorted by x and y
    for (zone_idx_t i = 0; i < zones_sorted.size(); ++i)
    {
        // Cache first zone
        const auto& zone_i = zones_sorted[i];
        const auto& zone_i_idx = zones_idx[i];

        // Clear slope counter to count for current zone_i only
        slope_counter.clear();


        // Loop through all zones "in front"
        // NOTE: Have to loop through even if there is no capacity.
        //  This is because zones in front need to connect back to this zone.
        for (zone_idx_t j = i+1; j < zones_sorted.size(); ++j)
        {
            // Cache second zone
            const auto& zone_j = zones_sorted[j];
            const auto& zone_j_idx = zones_idx[j];

            // Calculate slope
            zone_coord_signed_t dx = zone_j.x - zone_i.x;
            zone_coord_signed_t dy = (zone_coord_signed_t)zone_j.y - (zone_coord_signed_t)zone_i.y;

            if (dx == 0)
                dy = 1;
            else if (dy == 0)
                dx = 1;
            else
            {
                zone_coord_signed_t d = std::gcd(dx, abs(dy));
                dx /= d;
                dy /= d;
            }

            Slope slope{dx, dy};

            // Increment slope zone counter
            auto& count = ++slope_counter[slope];


            // If more than two zones are in front of zone_i on the same line, add edges.
            if (count > 2)
            {
                // If capacity available, add edge from zone_i to zone_j
                if (zone_i.capacity[O3] > 0)
                    graph_builder.add_edge
                    (
                        zone_i_idx,
                        zone_j_idx,
                        O3,
                        zone_i.capacity[O3]
                    );

                // If capacity available, add edge from zone_j to zone_i
                if (zone_j.capacity[O3] > 0)
                    graph_builder.add_edge
                    (
                        zone_j_idx,
                        zone_i_idx,
                        O3,
                        zone_j.capacity[O3]
                    );
            }
        }
    }
}