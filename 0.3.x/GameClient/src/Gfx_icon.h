#ifndef Gfx_iconH
#define Gfx_iconH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one spell icon, filled from a bitmap resource of its library.
struct IconResource
{
    TDx_DrawSurface *surface;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    IconResource();
    ~IconResource();
    TRect GetIconRect(int frame);
    TPoint LoadSurface(int library, int resource_id);
};

#endif
