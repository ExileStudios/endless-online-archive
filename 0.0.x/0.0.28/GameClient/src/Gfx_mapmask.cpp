#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapmask.h"
#include "Gfx_mask.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MaskWrapper::MaskWrapper()
{
}

MaskWrapper::MaskWrapper(int requested_id)
{
    graphic_id = requested_id;
    width = 0;
    height = 0;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

MaskWrapper::~MaskWrapper()
{
}

void MaskWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new MaskResource();
}

void MaskWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
