#include <vcl.h>
#pragma hdrstop

#include "Gfx_useritem.h"
#include "Gfx_subitem.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

SubItemWrapper::SubItemWrapper(int requested_id)
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

SubItemWrapper::~SubItemWrapper()
{
}

void SubItemWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new SubItemResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(SubItemWrapper)
