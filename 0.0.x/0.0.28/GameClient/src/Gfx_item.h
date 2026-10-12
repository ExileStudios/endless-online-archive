#ifndef Gfx_itemH
#define Gfx_itemH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one item lying on the map, filled from a bitmap resource of its library.
struct ItemResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ItemResource();
    ~ItemResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
