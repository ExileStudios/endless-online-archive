#ifndef MapwarpH
#define MapwarpH

// A warp tile: the map and tile it leads to, and the level it needs.
struct MapWarp
{
    unsigned short x;
    unsigned short y;
    int dest_map;
    int level_required;
    short dest_x;
    short dest_y;

    MapWarp(int tile_x, int tile_y, int target_map, int min_level, int arrival_x, int arrival_y);
    ~MapWarp();
};

#endif
