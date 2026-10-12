#include <vcl.h>
#pragma hdrstop

#include "Gfx_useritem.h"
#include "Gfx_subitem.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

SubItemWrapper::SubItemWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = 0;
    was_used = 0;
    width = 0;
    height = 0;
    idle_ticks = 0;
}

SubItemWrapper::~SubItemWrapper()
{
}

void SubItemWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new SubItemResource();
}

void SubItemWrapper::FreeResource()
{
    if (is_loaded)
        delete resource;
}
