#ifndef Gfx_objectH
#define Gfx_objectH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Gfx_bank.h"

// The surface of one map object, filled from a bitmap resource of its library.
struct ObjectResource
{
    TDx_DrawSurface *surface;
    TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ObjectResource();
    ~ObjectResource();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
