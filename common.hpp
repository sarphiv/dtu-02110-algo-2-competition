#pragma once

using zone_coord_t = unsigned int;
using zone_idx_t = unsigned int;
using zone_capacity_t = unsigned int;


struct ZoneInfo
{
    zone_coord_t x;
    zone_coord_t y;
    zone_capacity_t capacity[4];
};

