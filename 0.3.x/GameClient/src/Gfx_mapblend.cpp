#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapblend.h"
#include "Gfx_blend.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

BlendWrapper::BlendWrapper()
{
}

// A slot for one blend graphic, not loaded yet.
BlendWrapper::BlendWrapper(int requested_id)
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

BlendWrapper::~BlendWrapper()
{
}

void BlendWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new BlendResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(BlendWrapper)
