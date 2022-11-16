#pragma once


using zone_coord_t = unsigned int;
using zone_coord2_t = unsigned long long;
using zone_idx_t = unsigned int;
using zone_capacity_t = unsigned int;


enum ZoneObstacle
{
    O1 = 0,
    O2 = 1,
    O3 = 2,
    O4 = 3,
    Max = O4
};


struct ZoneInfo
{
    zone_coord_t x;
    zone_coord_t y;
    zone_capacity_t capacity[ZoneObstacle::Max];
};


struct ZoneEdge
{
    zone_idx_t start;
    zone_idx_t end;
    ZoneObstacle obstacle;
    zone_capacity_t capacity;
};

