#pragma once

#include <vector>


using zone_coord_t = unsigned int;
using zone_coord_signed_t = int;
using zone_coord2_t = long long int;
using zone_coord_dist_t = double;
using zone_idx_t = unsigned int;
using zone_idx_signed_t = long long;
using zone_capacity_t = unsigned int;
// NOTE: Max only works if type is unsigned
#define ZONE_IDX_NONE ((zone_idx_t)-2)
#define ZONE_IDX_MAX ((zone_idx_t)-3)
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
#define ZONE_OBSTACLE_NODE_STRIDE (3)


struct ZoneInfo
{
    zone_idx_t idx;
    zone_coord_t x;
    zone_coord_t y;
    zone_capacity_t capacity[ZONE_OBSTACLE_SIZE];
};


struct ZoneInfos
{
    zone_idx_t size;
    std::vector<zone_idx_t> idx;
    std::vector<zone_coord_t> x;
    std::vector<zone_coord_t> y;
    std::vector<std::vector<zone_capacity_t>> capacity;


    ZoneInfos(zone_idx_t size)
        : size(size), idx(size), x(size), y(size), capacity(ZONE_OBSTACLE_SIZE, std::vector<zone_capacity_t>(size))
    {
    }

    ZoneInfos(const ZoneInfos& other, const std::vector<zone_idx_t>& rank)
        : ZoneInfos(rank.size())
    {
        for (zone_idx_t i = 0; i < size; ++i)
        {
            idx[i] = other.idx[rank[i]];
            x[i] = other.x[rank[i]];
            y[i] = other.y[rank[i]];

            for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
                capacity[o][i] = other.capacity[o][rank[i]];
        }
    }


    void set_zone(zone_idx_t rank, const ZoneInfo&& zone)
    {
        idx[rank] = zone.idx;
        x[rank] = zone.x;
        y[rank] = zone.y;

        for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
            capacity[o][rank] = zone.capacity[o];
    }


    void resize(const zone_idx_t size)
    {
        this->size = size;
        idx.resize(size);
        x.resize(size);
        y.resize(size);
        for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
            capacity[o].resize(size);
    }


    ZoneInfo operator[](const zone_idx_t rank) const
    {
        ZoneInfo zone;

        zone.idx = idx[rank];
        zone.x = x[rank];
        zone.y = y[rank];
        for (zone_obstacle_val_t o = 0; o < ZONE_OBSTACLE_SIZE; ++o)
            zone.capacity[o] = capacity[o][rank];

        return zone;
    }
};


