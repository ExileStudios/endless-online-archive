#ifndef Gfx_msgboardH
#define Gfx_msgboardH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The message board sheet: two window frames stacked one above the other.
// The quest window draws from it too.
struct MsgBoardGraphics
{
    TDx_DrawSurface *surface;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MsgBoardGraphics();
    ~MsgBoardGraphics();
    TRect GetFrameRect(int frame);
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
