#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapeffect.h"
#include "Gfx_effect.h"
#include "Gfx_mapobject.h"

#pragma package(smart_init)

EffectWrapper::EffectWrapper()
{
}

EffectWrapper::EffectWrapper(int requested_id)
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

EffectWrapper::~EffectWrapper()
{
}

void EffectWrapper::CreateResource()
{
    if (!was_used)
    {
        resource = new EffectResource();
        was_used = true;
    }
}

DEFINE_FREE_RESOURCE(EffectWrapper)
