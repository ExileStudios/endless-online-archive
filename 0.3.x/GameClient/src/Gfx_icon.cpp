#include <vcl.h>
#pragma hdrstop

#include "Gfx_icon.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

IconResource::IconResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

IconResource::~IconResource()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// One of the two frames of a spell icon sheet; any other frame number gives the first.
TRect IconResource::GetIconRect(int frame)
{
    TRect frame_rect;
    if (frame < 0 || frame > 1)
        frame = 0;
    frame_rect.top = 0;
    frame_rect.bottom = 32;
    frame_rect.left = 34 * frame;
    frame_rect.right = 34 * frame + 34;
    return frame_rect;
}

DEFINE_ICON_LOAD_SURFACE(IconResource)
