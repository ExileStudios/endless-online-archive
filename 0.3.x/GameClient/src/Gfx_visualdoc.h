#ifndef Gfx_visualdocH
#define Gfx_visualdocH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct VisualDocGraphics
{
    TDx_DrawSurface *surface;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    VisualDocGraphics();
    ~VisualDocGraphics();
    TPoint LoadSurface(int library, int resource_id);
};

#endif
