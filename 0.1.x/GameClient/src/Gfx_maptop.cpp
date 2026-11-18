#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptop.h"
#include "Gfx_top.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

TopWrapper::TopWrapper()
{
}

TopWrapper::TopWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = false;
    was_used = false;
    idle_ticks = 0;
    fetched = false;
    unfetched_frames = 0;
}

TopWrapper::~TopWrapper()
{
}

void TopWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new TopResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(TopWrapper)
