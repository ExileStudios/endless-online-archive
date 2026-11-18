#ifndef Gfx_jukeboxH
#define Gfx_jukeboxH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the jukebox window, drawn opaque.
struct JukeboxGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    JukeboxGraphics();
    ~JukeboxGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
