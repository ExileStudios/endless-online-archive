#include <vcl.h>
#pragma hdrstop

#include "Gfx_visualdoc.h"
#include "Tmainform.h"

#pragma package(smart_init)

VisualDocGraphics::VisualDocGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

VisualDocGraphics::~VisualDocGraphics()
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

DEFINE_ICON_LOAD_SURFACE(VisualDocGraphics)
