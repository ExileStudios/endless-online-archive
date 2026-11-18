#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapmask.h"
#include "Gfx_mask.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

MaskWrapper::MaskWrapper()
{
}

MaskWrapper::MaskWrapper(int requested_id)
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

MaskWrapper::~MaskWrapper()
{
}

void MaskWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new MaskResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(MaskWrapper)
