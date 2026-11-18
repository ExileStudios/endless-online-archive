#ifndef MapobjectH
#define MapobjectH

enum MapTileSpec
{
    MapTileSpec_Wall = 0,
    MapTileSpec_ChairDown = 1,
    MapTileSpec_ChairLeft = 2,
    MapTileSpec_ChairRight = 3,
    MapTileSpec_ChairUp = 4,
    MapTileSpec_ChairDownRight = 5,
    MapTileSpec_ChairUpLeft = 6,
    MapTileSpec_ChairAll = 7,
    MapTileSpec_Door = 8,
    MapTileSpec_Chest = 9,
    MapTileSpec_BankVault = 16,
    MapTileSpec_NpcBoundary = 17,
    MapTileSpec_Edge = 18,
    MapTileSpec_FakeWall = 19,
    MapTileSpec_Board1 = 20,
    MapTileSpec_Board2 = 21,
    MapTileSpec_Board3 = 22,
    MapTileSpec_Board4 = 23,
    MapTileSpec_Board5 = 24,
    MapTileSpec_Board6 = 25,
    MapTileSpec_Board7 = 26,
    MapTileSpec_Board8 = 27,
    MapTileSpec_Jukebox = 28,
    MapTileSpec_Jump = 29,
    MapTileSpec_Water = 30,
    MapTileSpec_Arena = 32,
    MapTileSpec_AmbientSource = 33,
    MapTileSpec_TimedSpikes = 34,
    MapTileSpec_Spikes = 35,
    MapTileSpec_HiddenSpikes = 36
};

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
    TileSpec_Trap = 33,
    TileSpec_PassableDoor = 10,
    TileSpec_NpcBoundary = 16,
    TileSpec_Board1 = 19,
    TileSpec_Board2 = 20,
    TileSpec_Board3 = 21,
    TileSpec_Board4 = 22,
    TileSpec_Board5 = 23,
    TileSpec_Board6 = 24,
    TileSpec_Board7 = 25,
    TileSpec_Board8 = 26,
    TileSpec_Spikes = 34,
    TileSpec_HiddenSpikes = 35
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
