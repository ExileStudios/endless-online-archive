#ifndef Gfx_objectH
#define Gfx_objectH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one map object, filled from a bitmap resource of its library.
struct ObjectResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ObjectResource();
    ~ObjectResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
