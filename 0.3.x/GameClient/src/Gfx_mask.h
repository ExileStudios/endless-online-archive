#ifndef Gfx_maskH
#define Gfx_maskH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one map mask, filled from a bitmap resource of its library.
struct MaskResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MaskResource();
    ~MaskResource();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
