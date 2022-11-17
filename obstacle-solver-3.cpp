#include <unordered_map>
#include <unordered_set>
#include <list>
#include <tuple>


#include "common.hpp"
#include "obstacle-solver-3.hpp"


#define OBSTACLE_SOLVER_3_LINE_SEED 34127


struct Line
{
    obstacle_solver_3_line_val_t slope;
    obstacle_solver_3_line_val_t intercept;


    bool operator==(const Line& other) const
    {
        return slope == other.slope && intercept == other.intercept;
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

            std::hash<obstacle_solver_3_line_val_t> hasher;
            size_t seed = OBSTACLE_SOLVER_3_LINE_SEED;
            seed ^= (hasher(l.slope)     + 0x9e3779b9 + (seed << 6) + (seed >> 2));
            seed ^= (hasher(l.intercept) + 0x9e3779b9 + (seed << 6) + (seed >> 2));

            return seed;
        }
    };
}


void ObstacleSolver3::solve()
{
    // Creae a map of lines to zones (pressence and order of zones)
    // NOTE: Stores RANK of zones, not index
    std::unordered_map
    <
        Line, 
        std::tuple
        <
            std::unordered_set<zone_idx_t>,
            std::list<zone_idx_t>
        >
    > line_zone_map;


    // Loop through all zones in order of x and y
    for (zone_idx_t i = 0; i < zones_sorted.size(); i++)
    {
        // Cache first zone
        const auto& zone_i = zones_sorted[i];


        // Loop through all zones "in front"
        for (zone_idx_t j = i+1; j < zones_sorted.size(); j++)
        {
            // Cache target zone
            const auto& zone_j = zones_sorted[j];


            // Calculate slope and intercept
            // NOTE: x_j > x_i or x_j == x_i -> y_j > y_i
            //  Last inequality is strict, because unique points.
            // NOTE: Division by zero is intentional.
            //  In this case it always maps to positive infinity.
            const obstacle_solver_3_line_val_t slope = (obstacle_solver_3_line_val_t)
                  (zone_j.y - zone_i.y) 
                / (zone_j.x - zone_i.x);

            const obstacle_solver_3_line_val_t intercept = (obstacle_solver_3_line_val_t)
                zone_i.y - slope * zone_i.x;


            // Create line
            const Line line{slope, intercept};

            // If line already exists, add zone if not already added
            auto it = line_zone_map.find(line);
            if (it != line_zone_map.end())
            {
                auto& [set, list] = it->second;

                if (!set.count(j))
                {
                    set.insert(j);
                    list.push_back(j);
                }
            }
            // Else, create line, and add the two zones
            else
            {
                auto& [set, list] = line_zone_map[line];
                set.insert(i);
                set.insert(j);
                list.push_back(i);
                list.push_back(j);
            }
        }
    }


    // Loop over all lines
    for (const auto& line_pair : line_zone_map)
    {
        // Get zones on line
        // NOTE: Leftmost zone is always first zone, they are ordered
        const auto& pair = std::get<1>(line_pair);
        const auto& line_zones = std::get<1>(pair);

        
        // If line does not have enough zones, skip
        if (line_zones.size() < 4)
            continue;


        // Loop over all zones in the line
        //  while keeping track of the i'th zone on the line.
        long long line_zones_i = 0;
        for (const auto &zone_rank : line_zones)
        {
            // Cache zone
            const auto& zone_i = zones_sorted[zone_rank];
            const auto& zone_i_idx = zones_idx[zone_rank];
            const auto& capacity = zone_i.capacity[O3];

            // If zone does not support obstacle 3, skip
            if (capacity == 0)
                continue;


            // Add edges for all zones in front of (i-2)'th zone
            zone_idx_t j = 0;
            auto it = line_zones.begin();
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
            j = line_zones.size() - 1;
            auto rit = line_zones.rbegin();
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