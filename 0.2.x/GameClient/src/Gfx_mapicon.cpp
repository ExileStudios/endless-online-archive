#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapicon.h"
#include "Gfx_icon.h"

#pragma package(smart_init)

IconWrapper::IconWrapper()
{
}

IconWrapper::IconWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = false;
    was_used = false;
    idle_ticks = 0;
    fetched = false;
    unfetched_frames = 0;
}

IconWrapper::~IconWrapper()
{
}

void IconWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new IconResource();
        was_used = true;
    }
}

void IconWrapper::FreeResource()
{
    if (is_loaded)
    {
        delete resource;
        was_used = false;
    }
}
