#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapwall.h"
#include "Gfx_wall.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

WallWrapper::WallWrapper()
{
}

WallWrapper::WallWrapper(int requested_id)
{
    graphic_id = requested_id;
    width = 0;
    height = 0;
    is_loaded = false;
    was_used = false;
    idle_ticks = 0;
    fetched = false;
    unfetched_frames = 0;
}

WallWrapper::~WallWrapper()
{
}

void WallWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new WallResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(WallWrapper)
