#include "common.hpp"
#include "obstacle-solver-4.hpp"


void ObstacleSolver4::solve()
{
    for (zone_idx_t i = 0; i < zones.size; ++i)
        if (zones.capacity[O4][i] > 0)
            graph.add_zone_edge
            (
                zones.idx[i],
                terminal_idx,
                O4,
                zones.capacity[O4][i]
            );
}