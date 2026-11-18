#ifndef Gfx_effectH
#define Gfx_effectH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surfaces of one effect graphic: up to three layers (base, blended, solid), each a strip of frames.
struct EffectResource
{
    TDx_DrawSurface *layer0_surface;
    TDx_DrawSurface *layer1_surface;
    TDx_DrawSurface *layer2_surface;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    int frame_count;

    EffectResource();
    ~EffectResource();
    TRect GetFrameRect(int frame);
    GraphicSize LoadSurfaces(int library, int base_resource_id);
};

#endif
