#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapobject.h"
#include "Gfx_object.h"

#pragma package(smart_init)

ObjectWrapper::ObjectWrapper(int requested_id)
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

ObjectWrapper::~ObjectWrapper()
{
}

void ObjectWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new ObjectResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(ObjectWrapper)
