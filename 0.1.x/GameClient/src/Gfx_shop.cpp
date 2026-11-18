#include <vcl.h>
#pragma hdrstop

#include "Gfx_shop.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

ShopGraphics::ShopGraphics()
{
    item_slot_surface = new TDx_DrawSurface(NULL);
    background_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

ShopGraphics::~ShopGraphics()
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

// Loads the shop window background from the library into a surface sized to the bitmap; returns its size,
// or -1 when the bitmap could not be drawn onto the shop background surface.
DEFINE_LOAD_BACKGROUND(ShopGraphics)

// Loads the shop item slot frame into its own surface; returns its size, or -1 when it could not be drawn.
DEFINE_LOAD_ITEM_SLOT(ShopGraphics)
