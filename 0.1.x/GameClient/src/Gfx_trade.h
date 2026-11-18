#ifndef Gfx_tradeH
#define Gfx_tradeH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The trade window surfaces: the opaque background and the item slot frame.
struct TradeGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *item_slot_surface;
    TRect background_rect;
    TRect item_slot_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TradeGraphics();
    ~TradeGraphics();
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadItemSlot(HMODULE library, int resource_id);
};

#endif
