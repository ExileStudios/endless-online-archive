#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapitem.h"
#include "Gfx_item.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

ItemWrapper::ItemWrapper()
{
}

ItemWrapper::ItemWrapper(int requested_id)
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

ItemWrapper::~ItemWrapper()
{
}

void ItemWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new ItemResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(ItemWrapper)
