#ifndef Gfx_shopH
#define Gfx_shopH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The shop window surfaces: the opaque background and the item slot frame.
// The guild, inn, law, learn, locker, memo and report windows draw from it too.
struct ShopGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *item_slot_surface;
    TRect background_rect;
    TRect item_slot_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ShopGraphics();
    ~ShopGraphics();
    TPoint LoadBackground(int library, int resource_id);
    TPoint LoadItemSlot(int library, int resource_id);
};

#endif
