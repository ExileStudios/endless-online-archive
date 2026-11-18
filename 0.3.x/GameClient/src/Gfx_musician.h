#ifndef Gfx_musicianH
#define Gfx_musicianH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The musician window surfaces: the opaque background and a sheet of square cells.
struct MusicianGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *cell_sheet_surface;
    TRect background_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MusicianGraphics();
    ~MusicianGraphics();
    TRect GetCellRect(int column, int row);
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadCellSheet(HMODULE library, int resource_id);
};

#endif
