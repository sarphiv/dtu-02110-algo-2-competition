#include "common.hpp"
#include "obstacle-solver-4.hpp"


void ObstacleSolver4::solve()
{
    for (const auto& zone : zones)
        if (zone.capacity[O4] > 0)
            graph.add_zone_edge
            (
                zone.idx,
                terminal_idx,
                O4,
                zone.capacity[O4]
            );

    ++graph.terminal_base_offset;
}