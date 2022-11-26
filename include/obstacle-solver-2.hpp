#pragma once
#include <vector>
#include <tuple>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver2
{
private:
    FlowGraph& graph;
    // NOTE: Needs to be sorted to find convex hull in O(n) time
    const ZoneInfos& zones_sorted;
    const zone_idx_t terminal_idx;


    zone_coord2_t dist2(const ZoneInfos &hull, const zone_idx_t hull_idx, const ZoneInfo &zone)
    {
        const zone_coord2_t dx = (zone_coord2_t)zone.x - (zone_coord2_t)hull.x[hull_idx];
        const zone_coord2_t dy = (zone_coord2_t)zone.y - (zone_coord2_t)hull.y[hull_idx];

        return dx * dx + dy * dy;
    }

    // zone_coord2_t cross(const ZoneInfo& o, const ZoneInfo& a, const ZoneInfo& b)
    zone_coord2_t cross(const ZoneInfos& hull, const zone_idx_t hull_idx, const zone_idx_t zone_rank)
    {
        // DISCLAIMER: This function is based on public domain code.
        //  Specifically, Andrew's monotone chain C++ implementation on wikibooks.

        // 3D cross product of OA and OB vectors, 
        // (i.e z-component of their "2D" cross product, but remember that it is not defined in "2D").
        // Returns a positive value, if OAB makes a counter-clockwise turn,
        // negative for clockwise turn, and zero if the points are collinear.
        const zone_coord2_t hull1_x = hull.x[hull_idx-1];
        const zone_coord2_t hull1_y = hull.y[hull_idx-1];
        const zone_coord2_t hull2_x = hull.x[hull_idx-2];
        const zone_coord2_t hull2_y = hull.y[hull_idx-2];
        const zone_coord2_t zone_x = zones_sorted.x[zone_rank];
        const zone_coord2_t zone_y = zones_sorted.y[zone_rank];

        return (hull2_x - hull1_x) 
            *  ( zone_y - hull1_y) 
            -  (hull2_y - hull1_y) 
            *  ( zone_x - hull1_x);
    }

    std::tuple<ZoneInfos, std::vector<zone_idx_t>> find_convex_hull();

public:
    ObstacleSolver2() = delete;
    ObstacleSolver2
    (
        FlowGraph& graph, 
        const ZoneInfos& zones_sorted,
        const zone_idx_t terminal_idx
    )
        : graph(graph), zones_sorted(zones_sorted), terminal_idx(terminal_idx)
    {
    }

    void solve();
};