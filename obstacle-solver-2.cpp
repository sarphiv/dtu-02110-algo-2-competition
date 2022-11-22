#include <tuple>
#include <array>

#include "common.hpp"
#include "obstacle-solver-2.hpp"


std::tuple<std::vector<ZoneInfo>, std::vector<zone_idx_t>> ObstacleSolver2::find_convex_hull()
{
    // DISCLAIMER: This function is based on public domain code.
    //  Specifically, Andrew's monotone chain C++ implementation on wikiboks.


    // Prepare counts
	zone_idx_t n = zones_sorted.size(), k = 0;

    // If three points, then convex hull is the same as the points, return it
	if (n <= 3)
    {
        std::vector<zone_idx_t> zones_rank(zones_sorted.size());
        for (zone_idx_t i = 0; i < zones_rank.size(); ++i)
            zones_rank[i] = i;


        return {zones_sorted, zones_rank};
    }


    // Prepare output
	std::vector<ZoneInfo> hull(2*n);
    std::vector<zone_idx_t> zones_rank(2*n);

    // Build lower hull
	for (zone_idx_t i = 0; i < n; ++i) {
		while (k >= 2 && cross(hull[k-2], hull[k-1], zones_sorted[i]) <= 0)
            k--;
		
        hull[k] = zones_sorted[i];
        zones_rank[k] = i;
        k++;
	}

	// Build upper hull
	for (zone_idx_t i = n-1, t = k+1; i > 0; --i) {
		while (k >= t && cross(hull[k-2], hull[k-1], zones_sorted[i-1]) <= 0)
            k--;

        hull[k] = zones_sorted[i-1];
        zones_rank[k] = i-1;
        k++;
	}


	hull.resize(k-1);
    zones_rank.resize(k-1);

    // Returns a vector of points on the convex hull in counter-clockwise order.
    // Note: There is a duplicate zone
	return {hull, zones_rank};
}


void ObstacleSolver2::solve()
{
    // Find zones construing convex hull and associated zone indices
    const auto& [hull, hull_rank] = find_convex_hull();


    // NOTE: Indexing this way to avoid searching the graph for residual edges.
    //  This method is less cache efficient 
    //  and may have lots of branch mispredictions though.

    // NOTE: Written under the assumption that there is always more than one zone.
    //  This assumption is consistent with the assignment constraints.

    // Store whether a zone has been processed, 
    //  and if so, the index of associated furthest zone
    std::vector<zone_idx_t> processed(zones_sorted.size(), ZONE_IDX_MAX);

    
    for (zone_idx_t i = 0; i < zones_sorted.size(); ++i)
    {
        if ((processed[i] != ZONE_IDX_MAX) | (processed[i] == ZONE_IDX_NONE))
            continue;


        // NOTE: Will break if matching zones found.
        //  A pair of matching zones have each other as furthest zones.
        zone_idx_t zone_curr_rank = i;
        zone_idx_signed_t steps = 0;
        while ((processed[zone_curr_rank] == ZONE_IDX_MAX)
            &  (processed[zone_curr_rank] != ZONE_IDX_NONE))
        {
            // Cache zone
            ZoneInfo zone = zones_sorted[zone_curr_rank];

            // If zone does not support obstacle 2, start connecting
            if (zone.capacity[O2] == 0)
            {
                processed[zone_curr_rank] = ZONE_IDX_NONE;
                break;
            }

            // Search for furthest point, which intuitively is on the convex hull
            zone_idx_t furthest_idx = zone.idx;
            zone_idx_t furthest_rank = zone_curr_rank;
            zone_coord_dist_t furthest_dist = 0;

            for (zone_idx_t j = 0; j < hull.size(); j++)
            {
                ZoneInfo zone_hull = hull[j];
                zone_coord_dist_t d = dist(zone, zone_hull);

                // If distance is greater than furthest distance, update
                // NOTE: In case of same distance, the lowest index zone is chosen
                if ((d > furthest_dist) | ((d == furthest_dist) & (zone_hull.idx < furthest_idx)))
                {
                    furthest_idx = zone_hull.idx;
                    furthest_rank = hull_rank[j];
                    furthest_dist = d;
                }
            }

            // Update trackers
            ++steps;
            processed[zone_curr_rank] = furthest_rank;
            zone_curr_rank = furthest_rank;
        }


        // Connect all except last zones in the path
        zone_idx_t zone_prev_rank = i;
        zone_curr_rank = processed[i];
        for (zone_idx_signed_t j = 0; j < steps-2; ++j)
        {
            // Cache zones
            ZoneInfo zone_prev = zones_sorted[zone_prev_rank];
            ZoneInfo zone_curr = zones_sorted[zone_curr_rank];

            // NOTE: Terminal zone is always at the end of the path,
            //  because its capacities have been forced to zero.
            //  There is no need to handle it here.
            
            // Add new edge
            graph.add_zone_edge
            (
                zone_prev.idx,
                zone_curr.idx,
                O2,
                zone_prev.capacity[O2]
            );

            // Traverse forwards in path
            zone_prev_rank = zone_curr_rank;
            zone_curr_rank = processed[zone_curr_rank];
        }


        // If there is a final (last) step, process it
        if (steps > 0)
        {
            // Get rank of next zone
            const zone_idx_t zone_next_rank = processed[zone_curr_rank];

            
            // If there are threee zones in the path left, connect them
            // NOTE: This means there is circular match
            if (zone_next_rank != zone_prev_rank)
            {
                // Cache zones
                ZoneInfo zone_start = zones_sorted[zone_prev_rank];
                ZoneInfo zone_end = zones_sorted[zone_curr_rank];

                // If already connected to terminal zone, 
                //  add capacity to existing edge
                if ((zone_end.idx == terminal_idx) & (zone_start.capacity[O4] > 0))
                    graph.increment_zone_edge
                    (
                        zone_start.idx, 
                        graph.get_terminal_offset(zone_start.idx),
                        O2,
                        zone_start.capacity[O2]
                    );
                // Else, add new zone edge
                else
                    graph.add_zone_edge
                    (
                        zone_start.idx,
                        zone_end.idx,
                        O2,
                        zone_start.capacity[O2]
                    );
                

                // If next zone leads nowhere, do not connect
                if ((zone_next_rank == ZONE_IDX_MAX) | (zone_next_rank == ZONE_IDX_NONE))
                    continue;


                // Advance cache
                zone_start = zone_end;
                zone_end = zones_sorted[zone_next_rank];

                // If already connected to terminal zone via obstacle O4, 
                //  add capacity to existing edge
                if ((zone_end.idx == terminal_idx) & (zone_start.capacity[O4] > 0))
                    graph.increment_zone_edge
                    (
                        zone_start.idx,
                        graph.get_terminal_offset(zone_start.idx),
                        O2,
                        zone_start.capacity[O2]
                    );
                // Else, add new zone edge
                else
                    graph.add_zone_edge
                    (
                        zone_start.idx,
                        zone_end.idx,
                        O2,
                        zone_start.capacity[O2]
                    );
            }
            // Else, zones have each other as furthest zones, connect them
            // NOTE: Terminal node never matches any other node.
            else
                graph.add_zone_edge
                (
                    zones_sorted[zone_prev_rank].idx,
                    zones_sorted[zone_curr_rank].idx,
                    O2,
                    zones_sorted[zone_prev_rank].capacity[O2],
                    zones_sorted[zone_curr_rank].capacity[O2]
                );

        }
    }
}