#ifndef Gfx_introH
#define Gfx_introH

class TDDBltFX;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

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
