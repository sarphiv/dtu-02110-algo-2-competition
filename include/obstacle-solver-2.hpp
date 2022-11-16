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

    const std::vector<zone_idx_t>& zones_idx;


    zone_coord2_t dist(const ZoneInfo &a, const ZoneInfo &b)
    {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }

    zone_coord2_t cross(const ZoneInfo &o, const ZoneInfo &a, const ZoneInfo &b)
    {
        // DISCLAIMER: This function is based on public domain code.
        //  Specifically, Andrew's monotone chain C++ implementation on wikiboks.

        // 3D cross product of OA and OB vectors, 
        // (i.e z-component of their "2D" cross product, but remember that it is not defined in "2D").
        // Returns a positive value, if OAB makes a counter-clockwise turn,
        // negative for clockwise turn, and zero if the points are collinear.
        return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
    }

    std::tuple<std::vector<ZoneInfo>, std::vector<zone_idx_t>> find_convex_hull();

public:
    ObstacleSolver2() = delete;
    ObstacleSolver2
    (
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones_sorted, 
        const std::vector<zone_idx_t>& zones_idx
    )
        : graph_builder(graph_bulider), zones_sorted(zones_sorted), zones_idx(zones_idx)
    {
    }

    void solve();
};