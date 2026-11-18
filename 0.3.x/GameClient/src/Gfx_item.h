#ifndef Gfx_itemH
#define Gfx_itemH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one item lying on the map, filled from a bitmap resource of its library.
struct ItemResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ItemResource();
    ~ItemResource();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
