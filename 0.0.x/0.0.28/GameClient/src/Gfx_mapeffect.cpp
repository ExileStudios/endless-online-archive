#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapeffect.h"
#include "Gfx_effect.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

EffectWrapper::EffectWrapper()
{
}

EffectWrapper::EffectWrapper(int requested_id)
{
    graphic_id = requested_id;
    width = 0;
    height = 0;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

EffectWrapper::~EffectWrapper()
{
}

void EffectWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new EffectResource();
}

void EffectWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
