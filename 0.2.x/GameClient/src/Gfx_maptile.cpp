#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptile.h"
#include "Gfx_tile.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

TileWrapper::TileWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = false;
    was_used = false;
    idle_ticks = 0;
    fetched = false;
    unfetched_frames = 0;
}

TileWrapper::~TileWrapper()
{
}

void TileWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new TileResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(TileWrapper)
