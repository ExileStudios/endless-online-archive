#ifndef MapobjectH
#define MapobjectH

enum TileSpec
{
    TileSpec_ChairYHigh = 0,
    TileSpec_ChairXLow = 1,
    TileSpec_ChairXHigh = 2,
    TileSpec_ChairYLow = 3,
    TileSpec_ChairYHighXHigh = 4,
    TileSpec_ChairXLowYLow = 5,
    TileSpec_ChairAnySide = 6,
    TileSpec_Door = 7,
    TileSpec_Chest = 8,
    TileSpec_OpenDoor = 9,
    TileSpec_Passable = 11,
    TileSpec_Locker = 15,
    TileSpec_Jukebox = 27,
    TileSpec_Jump = 28,
    TileSpec_Splash = 29,
    TileSpec_CountdownSpot = 31,
    TileSpec_AmbientSource = 32,
    TileSpec_Trap = 33
};

// A map tile and its spec.
struct MapObject
{
    unsigned short x;
    unsigned short y;
    unsigned short spec;

    MapObject(int tile_x, int tile_y, int tile_spec);
    ~MapObject();
};

#endif
