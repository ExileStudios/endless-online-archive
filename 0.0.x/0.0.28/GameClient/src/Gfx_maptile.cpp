#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptile.h"
#include "Gfx_tile.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

TileWrapper::TileWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

TileWrapper::~TileWrapper()
{
}

void TileWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new TileResource();
}

void TileWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
