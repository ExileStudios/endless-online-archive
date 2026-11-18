#ifndef Gfx_topH
#define Gfx_topH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one top layer graphic, filled from a bitmap resource of its library.
struct TopResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TopResource();
    ~TopResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
