#include <vcl.h>
#pragma hdrstop

#include "Gfx_subitem.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

SubItemResource::SubItemResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

SubItemResource::~SubItemResource()
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

// Loads bitmap resource_id from the library into the item picture for the game windows surface and returns its size.
// The size is -1 when the bitmap could not be drawn onto the item picture for the game windows surface.
DEFINE_LOAD_SURFACE(SubItemResource)
