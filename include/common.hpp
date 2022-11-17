#pragma once


using zone_coord_t = unsigned int;
using zone_coord_signed_t = int;
using zone_coord2_t = long long int;
using zone_coord_dist_t = double;
using zone_idx_t = unsigned int;
using zone_capacity_t = unsigned int;

using zone_obstacle_val_t = unsigned char;
enum ZoneObstacle : zone_obstacle_val_t
{
    O1 = 0,
    O2 = 1,
    O3 = 2,
    O4 = 3
};
#define ZONE_OBSTACLE_SIZE (O4+1)


struct ZoneInfo
{
    zone_coord_t x;
    zone_coord_t y;
    zone_capacity_t capacity[ZONE_OBSTACLE_SIZE];
};


struct ZoneEdge
{
    zone_idx_t start;
    zone_idx_t end;
    ZoneObstacle obstacle;
    zone_capacity_t capacity;
};

