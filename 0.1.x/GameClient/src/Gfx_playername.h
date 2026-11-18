#ifndef Gfx_playernameH
#define Gfx_playernameH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// A hover label: a name in its colour on a black, see-through surface.
struct PlayerNameGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    int text_width;
    int text_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;

    PlayerNameGraphics(int label_width, unsigned int label_height, int color_index, String name);
    ~PlayerNameGraphics();
};

#endif
