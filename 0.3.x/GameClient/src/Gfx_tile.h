#ifndef Gfx_tileH
#define Gfx_tileH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one ground tile, filled from a bitmap resource of its library.
struct TileResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    char is_animated;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TileResource();
    ~TileResource();
    TRect GetFrameRect(int frame);
    TPoint LoadSurface(int library, int resource_id);
};

#endif
