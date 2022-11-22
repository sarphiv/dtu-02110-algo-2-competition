#pragma once


using zone_coord_t = unsigned int;
using zone_coord_signed_t = int;
using zone_coord2_t = long long int;
using zone_coord_dist_t = double;
using zone_idx_t = unsigned int;
using zone_capacity_t = unsigned int;
// NOTE: Only works if zone_capacity_t is unsigned
#define ZONE_CAPACITY_MAX ((zone_capacity_t)-2)
#define ZONE_COORD_MAX (100000)

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
    zone_idx_t idx;
    zone_coord_t x;
    zone_coord_t y;
    zone_capacity_t capacity[ZONE_OBSTACLE_SIZE];
};


