#include <vcl.h>
#pragma hdrstop

#include "Mapdoor.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MapDoor::MapDoor(int tile_x, int tile_y, int door_key)
{
    x = tile_x;
    y = tile_y;
    key = door_key;
    closed_graphic = 0;
    open = 0;
}

MapDoor::~MapDoor()
{
}
