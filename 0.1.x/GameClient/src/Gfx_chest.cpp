#include <vcl.h>
#pragma hdrstop

#include "Gfx_chest.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

ChestGraphics::ChestGraphics()
{
    item_slot_surface = new TDx_DrawSurface(NULL);
    background_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

ChestGraphics::~ChestGraphics()
{
    item_slot_surface->Destroy();
    background_surface->Destroy();
    delete item_slot_surface;
    delete background_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// Loads the chest window background from the library into a surface sized to the bitmap; returns its size,
// or -1 when the bitmap could not be drawn onto the chest background surface.
DEFINE_LOAD_BACKGROUND(ChestGraphics)

// Loads the chest item slot frame into its own surface; returns its size, or -1 when it could not be drawn.
DEFINE_LOAD_ITEM_SLOT(ChestGraphics)
