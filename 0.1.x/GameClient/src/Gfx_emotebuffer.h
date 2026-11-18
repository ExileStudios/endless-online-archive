#ifndef Gfx_emotebufferH
#define Gfx_emotebufferH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

// A small surface, cleared to black with black see-through, that the sprite code draws bitmap layers into.
struct EmoteBuffer
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;

    EmoteBuffer();
    ~EmoteBuffer();
};

#endif
