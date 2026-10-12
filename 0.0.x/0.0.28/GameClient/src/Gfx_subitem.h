#ifndef Gfx_subitemH
#define Gfx_subitemH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one item picture for the game windows, filled from a bitmap resource of its library.
struct SubItemResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    SubItemResource();
    ~SubItemResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
