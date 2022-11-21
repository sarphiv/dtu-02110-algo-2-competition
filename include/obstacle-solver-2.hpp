#pragma once
#include <cmath>
#include <vector>
#include <tuple>

#include "common.hpp"
#include "graph-builder.hpp"


class ObstacleSolver2
{
private:
    GraphBuilder& graph_builder;
    const std::vector<ZoneInfo>& zones_sorted;


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

    std::vector<ZoneInfo> find_convex_hull();

public:
    ObstacleSolver2() = delete;
    ObstacleSolver2
    (
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones_sorted
    )
        : graph_builder(graph_bulider), zones_sorted(zones_sorted)
    {
    }

    void solve();
};