#ifndef Gfx_emotebufferH
#define Gfx_emotebufferH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// A small surface, cleared to black with black see-through, that the sprite code draws bitmap layers into.
struct EmoteBuffer
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;

    EmoteBuffer();
    ~EmoteBuffer();
};

#endif
