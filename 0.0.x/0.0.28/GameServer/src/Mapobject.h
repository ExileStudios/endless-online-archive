#ifndef MapobjectH
#define MapobjectH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

// The special tiles, by their value ( the map file's tile code less one ).
enum TileSpec
{
    TileSpec_ChairDown = 0,
    TileSpec_ChairLeft = 1,
    TileSpec_ChairRight = 2,
    TileSpec_ChairUp = 3,
    TileSpec_ChairDownRight = 4,
    TileSpec_ChairUpLeft = 5,
    TileSpec_ChairAll = 6,
    TileSpec_Door = 7,
    TileSpec_DoorOpen = 9,
    TileSpec_PassableDoor = 10,
    TileSpec_PassableDoorOpen = 11,
    TileSpec_BankVault = 15,
    TileSpec_NpcBoundary = 16,
    TileSpec_Arena = 31
};



struct MapObject;

// A special tile of a map ( a chair, a door, a vault, .. ); a door's ticks count down while it is open.
struct MapObject
{
    unsigned short x;
    unsigned short y;
    unsigned short value;
    short ticks;

    MapObject(int x, int y, int value);
    ~MapObject();
};


#endif
