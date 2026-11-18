#ifndef Gfx_wallH
#define Gfx_wallH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one wall graphic, filled from a bitmap resource of its library.
struct WallResource
{
    TDx_DrawSurface *surface;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    char is_animated;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    WallResource();
    ~WallResource();
    TRect GetFrameRect(int frame);
    TPoint LoadSurface(int library, int resource_id);
};

#endif
