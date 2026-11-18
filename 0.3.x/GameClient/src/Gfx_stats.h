#ifndef Gfx_statsH
#define Gfx_statsH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the character stats window, drawn opaque.
struct StatGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    StatGraphics();
    ~StatGraphics();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
