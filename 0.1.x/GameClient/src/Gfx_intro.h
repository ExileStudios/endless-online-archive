#ifndef Gfx_introH
#define Gfx_introH

#include <TDDBltFX.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

// The surface of the intro picture, kept until the intro ends.
struct IntroGraphics
{
    TDx_DrawSurface *surface;
    TDDBltFX *fill_fx;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;

    IntroGraphics();
    ~IntroGraphics();
    void LoadSurface(int library, int resource_id);
};

#endif
