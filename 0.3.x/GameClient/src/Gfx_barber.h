#ifndef Gfx_barberH
#define Gfx_barberH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the barber window, drawn opaque.
struct BarberGraphics
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    BarberGraphics();
    ~BarberGraphics();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
