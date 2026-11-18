#ifndef Gfx_topH
#define Gfx_topH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one top layer graphic, filled from a bitmap resource of its library.
struct TopResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TopResource();
    ~TopResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
