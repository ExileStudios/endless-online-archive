#ifndef Gfx_dialogH
#define Gfx_dialogH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the npc quest dialog window, drawn opaque.
struct DialogGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    DialogGraphics();
    ~DialogGraphics();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
