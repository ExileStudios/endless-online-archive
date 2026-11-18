#include <vcl.h>
#pragma hdrstop

#include "Mapobject.h"

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
