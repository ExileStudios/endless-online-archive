#ifndef Gfx_playernameH
#define Gfx_playernameH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

// A hover label: a name in its colour on a black, see-through surface.
struct PlayerNameGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    int text_width;
    int text_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;

    PlayerNameGraphics(int label_width, int label_height, int color_index, String name);
    ~PlayerNameGraphics();
};

#endif
