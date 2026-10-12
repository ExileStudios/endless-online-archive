#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapwall.h"
#include "Gfx_wall.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

WallWrapper::WallWrapper()
{
}

WallWrapper::WallWrapper(int requested_id)
{
    graphic_id = requested_id;
    width = 0;
    height = 0;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

WallWrapper::~WallWrapper()
{
}

void WallWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new WallResource();
}

void WallWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
