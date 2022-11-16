#include "common.hpp"
#include "obstacle-solver-2.hpp"


std::tuple<std::vector<ZoneInfo>, std::vector<zone_idx_t>> ObstacleSolver2::find_convex_hull()
{
    // DISCLAIMER: This function is based on public domain code.
    //  Specifically, Andrew's monotone chain C++ implementation on wikiboks.


    // Prepare counts
	zone_idx_t n = zones_sorted.size(), k = 0;

    // If three points, then convex hull is the same as the points, return it
	if (n <= 3) return {zones_sorted, zones_idx};


    // Prepare output
	std::vector<ZoneInfo> hull(2*n);
    std::vector<zone_idx_t> hull_idx(2*n);

    // Build lower hull
	for (zone_idx_t i = 0; i < n; ++i) {
		while (k >= 2 && cross(hull[k-2], hull[k-1], zones_sorted[i]) <= 0)
            k--;
		
        hull[k] = zones_sorted[i];
        hull_idx[k] = zones_idx[i];
        k++;
	}

	// Build upper hull
	for (zone_idx_t i = n-1, t = k+1; i > 0; --i) {
		while (k >= t && cross(hull[k-2], hull[k-1], zones_sorted[i-1]) <= 0)
            k--;

        hull[k] = zones_sorted[i-1];
        hull_idx[k] = zones_idx[i-1];
        k++;
	}


    // Note: the last point in the vector is the same as the first one.
	hull.resize(k-1);
    hull_idx.resize(k-1);

    // Returns a list of points on the convex hull in counter-clockwise order.
	return {hull, hull_idx};
}


void ObstacleSolver2::solve()
{
    // Find zones construing convex hull and associated zone indices
    auto [hull, hull_idx] = find_convex_hull();


    // Find furthest point for valid zones
    for (zone_idx_t i = 0; i < zones_sorted.size(); i++)
    {
        // Cache zone
        ZoneInfo zone = zones_sorted[i];
        zone_idx_t zone_idx = zones_idx[i];


        // If zone does not support obstacle 2, continue
        if (zone.capacity[O2] == 0)
            continue;


        // Search for furthest point, which intuitively is on the convex hull
        zone_idx_t furthest_idx = 0;
        zone_coord2_t furthest_dist = 0;

        for (zone_idx_t j = 0; j < hull.size(); j++)
        {
            auto d = dist(zone, hull[j]);

            // If distance is greater than furthest distance, update
            // NOTE: In case of same distance, the lowest index zone is chosen
            if (   d > furthest_dist 
                || d == furthest_dist && hull_idx[j] < furthest_idx)
            {
                furthest_idx = hull_idx[j];
                furthest_dist = d;
            }
        }


        // Add edge to furthest zone to graph
        graph_builder.add_edge(
            zone_idx,
            furthest_idx, 
            O2, 
            zone.capacity[O2]
        );
    }
}