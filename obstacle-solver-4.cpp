#include "common.hpp"
#include "obstacle-solver-4.hpp"


void ObstacleSolver4::solve()
{
    zone_idx_t zone_count = zones_sorted.size();

    for (zone_idx_t i = 0; i < zone_count; ++i)
        if (zones_sorted[i].capacity[O4] > 0)
            graph_builder.add_edge
            (
                zones_idx[i],
                zone_count-1,
                O4,
                zones_sorted[i].capacity[O4]
            );
}