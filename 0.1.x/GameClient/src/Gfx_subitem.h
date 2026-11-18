#ifndef Gfx_subitemH
#define Gfx_subitemH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one item picture for the game windows, filled from a bitmap resource of its library.
struct SubItemResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    SubItemResource();
    ~SubItemResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
