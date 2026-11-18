#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapnpc.h"
#include "Gfx_npc.h"

#pragma package(smart_init)

NpcWrapper::NpcWrapper()
{
}

NpcWrapper::NpcWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = false;
    has_resource = false;
    fail_count = 0;
    load_stage = 0;
    was_used = false;
    idle_ticks = 0;
}

NpcWrapper::~NpcWrapper()
{
}

void NpcWrapper::CreateResource()
{
    if (!has_resource)
    {
        resource = new NpcResource();
        has_resource = true;
    }
}

void NpcWrapper::FreeResource()
{
    if (has_resource)
    {
        delete resource;
        has_resource = false;
    }
}
