#ifndef Gfx_statsH
#define Gfx_statsH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the character stats window, drawn opaque.
struct StatGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    StatGraphics();
    ~StatGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
