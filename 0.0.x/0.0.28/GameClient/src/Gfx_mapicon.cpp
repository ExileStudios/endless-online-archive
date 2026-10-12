#include <vcl.h>
#pragma hdrstop

#include "Gfx_mapicon.h"
#include "Gfx_icon.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

IconWrapper::IconWrapper()
{
}

IconWrapper::IconWrapper(int requested_id)
{
    graphic_id = requested_id;
    is_loaded = 0;
    was_used = 0;
    idle_ticks = 0;
}

IconWrapper::~IconWrapper()
{
}

void IconWrapper::CreateResource()
{
    if (!is_loaded)
        resource = new IconResource();
}
