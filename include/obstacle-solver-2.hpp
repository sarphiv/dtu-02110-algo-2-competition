#pragma once
#include <cmath>
#include <vector>
#include <tuple>

#include "common.hpp"
#include "flow-graph.hpp"


class ObstacleSolver2
{
private:
    FlowGraph& graph;
    // NOTE: Needs to be sorted to find convex hull in O(n) time
    const std::vector<ZoneInfo>& zones_sorted;
    const zone_idx_t terminal_idx;


    zone_coord_dist_t dist(const ZoneInfo &a, const ZoneInfo &b)
    {
        const zone_coord2_t dx = (zone_coord2_t)a.x - (zone_coord2_t)b.x;
        const zone_coord2_t dy = (zone_coord2_t)a.y - (zone_coord2_t)b.y;

        return sqrt(dx * dx + dy * dy);
    }

    zone_coord2_t cross(const ZoneInfo& o, const ZoneInfo& a, const ZoneInfo& b)
    {
        // DISCLAIMER: This function is based on public domain code.
        //  Specifically, Andrew's monotone chain C++ implementation on wikiboks.

        // 3D cross product of OA and OB vectors, 
        // (i.e z-component of their "2D" cross product, but remember that it is not defined in "2D").
        // Returns a positive value, if OAB makes a counter-clockwise turn,
        // negative for clockwise turn, and zero if the points are collinear.
        return ((zone_coord2_t)a.x - (zone_coord2_t)o.x) 
            * ((zone_coord2_t)b.y - (zone_coord2_t)o.y) 
            - ((zone_coord2_t)a.y - (zone_coord2_t)o.y) 
            * ((zone_coord2_t)b.x - (zone_coord2_t)o.x);
    }

    std::tuple<std::vector<ZoneInfo>, std::vector<zone_idx_t>> find_convex_hull();

public:
    ObstacleSolver2() = delete;
    ObstacleSolver2
    (
        FlowGraph& graph, 
        const std::vector<ZoneInfo>& zones_sorted,
        const zone_idx_t terminal_idx
    )
        : graph(graph), zones_sorted(zones_sorted), terminal_idx(terminal_idx)
    {
    }

    void solve();
};