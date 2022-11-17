#include <unordered_map>
#include <unordered_set>
#include <list>
#include <tuple>
#include <numeric>
#include <cmath>


#include "common.hpp"
#include "obstacle-solver-3.hpp"


#define OBSTACLE_SOLVER_3_LINE_SEED 34127


struct Line
{
    const zone_coord_signed_t dx;
    const zone_coord_signed_t dy;
    const zone_idx_t rank;

    bool operator==(const Line& other) const
    {
        return dx == other.dx && dy == other.dy && rank == other.rank;
    }
};

namespace std 
{
    template <>
    struct hash<Line>
    {
        std::size_t operator()(const Line& l) const
        {
            // DISCLAIMER: Based upon boost::hash_combine

            std::hash<zone_coord_signed_t> hasher_coord;
            std::hash<zone_idx_t> hasher_idx;
            size_t seed = OBSTACLE_SOLVER_3_LINE_SEED;
            seed ^= (hasher_coord(l.dx) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            seed ^= (hasher_coord(l.dy) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            seed ^= (hasher_idx(l.rank) + 0x9e3779b9 + (seed << 6) + (seed >> 2));

            return seed;
        }
    };
}



void ObstacleSolver3::solve()
{
    // Create a set of zones that have had their outgoing edges calculated
    // NOTE: Stores RANK of zones, not index
    std::unordered_set<Line> processed;


    // Loop through all zones in order of x and y
    for (zone_idx_t i = 0; i < zones_sorted.size(); ++i)
    {
        // Map lines to their zone ranks
        std::unordered_map<Line, std::list<zone_idx_t>> line_zones;

        // Cache first zone
        const auto& zone_i = zones_sorted[i];


        // Loop through all zones "in front"
        for (zone_idx_t j = i+1; j < zones_sorted.size(); ++j)
        {
            // Cache target zone
            const auto& zone_j = zones_sorted[j];

            // Create line from slope components
            // NOTE: x_j > x_i or x_j == x_i -> y_j > y_i
            //  Last inequality is strict, because unique points.
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

            Line line{dx, dy, j};


            // If line has been processed, skip it
            if (processed.count(line))
                continue;


            // Add line and zone_j to processed and processing queue
            // NOTE: Technically not processed until after this loop
            processed.insert(line);
            line_zones[{line.dx, line.dy, i}].push_back(j);
        }


        // Process all found lines going through zone_i
        for (auto& [line, zones] : line_zones)
        {
            // Put first zone in front of all zones
            // NOTE: Leftmost zone is always first zone, they are ordered.
            //  The "origin" zone_i was never added above
            zones.push_front(i);
            processed.insert({line.dx, line.dy, i});

            // If line does not have enough zones, skip
            if (zones.size() < 4)
                continue;


            // Loop over all zones in the line
            //  while keeping track of the i'th zone on the line.
            long long line_zones_i = 0;
            for (const auto &zone_rank : zones)
            {
                // Cache zone
                const auto& zone_i = zones_sorted[zone_rank];
                const auto& zone_i_idx = zones_idx[zone_rank];
                const auto& capacity = zone_i.capacity[O3];

                // If zone does not support obstacle 3, skip
                if (capacity == 0)
                {
                    ++line_zones_i;
                    continue;
                }


                // Add edges for all zones in front of (i-2)'th zone
                zone_idx_t j = 0;
                auto it = zones.begin();
                while (j < line_zones_i - 2)
                {
                    graph_builder.add_edge
                    (
                        zone_i_idx,
                        zones_idx[*it],
                        O3,
                        capacity
                    );

                    ++it;
                    ++j;
                }

                // Add edges for all zones behind (i+2)'th zone
                j = zones.size() - 1;
                auto rit = zones.rbegin();
                while (j > line_zones_i + 2)
                {
                    graph_builder.add_edge
                    (
                        zone_i_idx,
                        zones_idx[*rit],
                        O3,
                        capacity
                    );

                    ++rit;
                    --j;
                }


                // Increment index for i'th zone on line
                ++line_zones_i;
            }
        }
    }
}