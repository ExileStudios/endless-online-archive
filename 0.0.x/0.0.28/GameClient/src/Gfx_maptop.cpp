#include <vcl.h>
#pragma hdrstop

#include "Gfx_maptop.h"
#include "Gfx_top.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

TopWrapper::TopWrapper()
{
}

TopWrapper::TopWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

TopWrapper::~TopWrapper()
{
}

void TopWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new TopResource();
}

void TopWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
