#ifndef Gfx_dialogH
#define Gfx_dialogH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the npc quest dialog window, drawn opaque.
struct DialogGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    DialogGraphics();
    ~DialogGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
