#ifndef Gfx_barberH
#define Gfx_barberH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The background surface of the barber window, drawn opaque.
struct BarberGraphics
{
    TDx_DrawSurface *surface;
    TDx_DrawSurface *styles_surface;
    TDx_DrawSurface *part_surface;
    TRect styles_rect;
    int surface_left;
    int surface_top;
    int surface_right;
    int surface_bottom;
    TRect background_rect;
    TRect arrow_glow_rect;
    TRect previous_arrow_rect;
    TRect next_arrow_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;
    unsigned char shown_gender;
    unsigned short shown_skin;
    unsigned short shown_page;
    unsigned short shown_hair_color;

    BarberGraphics();
    ~BarberGraphics();
    bool RenderHairStyles(int gender, int skin, int hair_color, int page);
    TPoint LoadSurface(int library, int resource_id);
};

#endif
