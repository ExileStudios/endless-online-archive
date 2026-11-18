#include <vcl.h>
#pragma hdrstop

#include "Mapobject.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MapObject::MapObject(int tile_x, int tile_y, int tile_spec)
{
    x = tile_x;
    y = tile_y;
    spec = tile_spec;
}

MapObject::~MapObject()
{
}
