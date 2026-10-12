#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapitem.h"
#include "Gfx_item.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#pragma package(smart_init)

ItemWrapper::ItemWrapper()
{
}

ItemWrapper::ItemWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = 0;
    was_used = 0;
    width = 0;
    height = 0;
    idle_ticks = 0;
}

ItemWrapper::~ItemWrapper()
{
}

void ItemWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new ItemResource();
}

void ItemWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
