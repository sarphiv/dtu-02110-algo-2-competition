#pragma once
#include <vector>

#include "common.hpp"
#include "graph-builder.hpp"


class ObstacleSolver1
{
private:
    GraphBuilder& graph_builder;
    const std::vector<ZoneInfo>& zones;

    const std::vector<zone_coord_t>& x_sorted;
    const std::vector<zone_coord_t>& y_sorted;
    const std::vector<zone_idx_t>& x_idx;
    const std::vector<zone_idx_t>& y_idx;

    void solve(const std::vector<zone_coord_t>& val, const std::vector<zone_idx_t>& idx);
    void connect_range
    (
        const std::vector<zone_coord_t>& val, 
        const std::vector<zone_idx_t>& idx, 
        const zone_idx_t& start, 
        const zone_idx_t& end
    );


public:
    ObstacleSolver1() = delete;
    ObstacleSolver1
    (
        GraphBuilder& graph_bulider, 
        const std::vector<ZoneInfo>& zones, 
        const std::vector<zone_coord_t>& x_sorted, 
        const std::vector<zone_coord_t>& y_sorted, 
        const std::vector<zone_idx_t>& x_idx, 
        const std::vector<zone_idx_t>& y_idx
    )
        : graph_builder(graph_bulider), zones(zones), x_sorted(x_sorted), y_sorted(y_sorted), x_idx(x_idx), y_idx(y_idx)
    {
    }

    void solve();
};