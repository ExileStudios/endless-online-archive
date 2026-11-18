#include <vcl.h>
#pragma hdrstop

#include "Mapwarp.h"

#pragma package(smart_init)

MapWarp::MapWarp(int tile_x, int tile_y, int target_map, int min_level, int arrival_x, int arrival_y)
{
    x = tile_x;
    y = tile_y;
    dest_map = target_map;
    level_required = min_level;
    dest_x = arrival_x;
    dest_y = arrival_y;
}

MapWarp::~MapWarp()
{
}
