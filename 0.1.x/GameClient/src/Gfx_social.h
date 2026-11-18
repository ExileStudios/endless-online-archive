#ifndef Gfx_socialH
#define Gfx_socialH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the friends and ignore window, drawn opaque.
struct SocialGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    SocialGraphics();
    ~SocialGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
