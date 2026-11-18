#include <vcl.h>
#pragma hdrstop

#include "Gfx_mask.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

MaskResource::MaskResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

MaskResource::~MaskResource()
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

// Loads bitmap resource_id from the library into the map mask surface and returns its size.
// The size is -1 when the bitmap could not be drawn onto the map mask surface.
DEFINE_LOAD_SURFACE(MaskResource)
