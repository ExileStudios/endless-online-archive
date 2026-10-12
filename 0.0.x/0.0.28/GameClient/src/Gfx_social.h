#ifndef Gfx_socialH
#define Gfx_socialH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the friends and ignore window, drawn opaque.
struct SocialGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    SocialGraphics();
    ~SocialGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
