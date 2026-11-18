#include <vcl.h>
#pragma hdrstop

#include "Mapdoor.h"

#pragma package(smart_init)

MapDoor::MapDoor(int tile_x, int tile_y, int door_key)
{
    x = tile_x;
    y = tile_y;
    key = door_key;
    closed_graphic = 0;
    open = false;
}

MapDoor::~MapDoor()
{
}
