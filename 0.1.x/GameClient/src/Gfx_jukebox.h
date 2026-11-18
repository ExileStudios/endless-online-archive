#ifndef Gfx_jukeboxH
#define Gfx_jukeboxH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the jukebox window, drawn opaque.
struct JukeboxGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    JukeboxGraphics();
    ~JukeboxGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
