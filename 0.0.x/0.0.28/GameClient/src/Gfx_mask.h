#ifndef Gfx_maskH
#define Gfx_maskH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one map mask, filled from a bitmap resource of its library.
struct MaskResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MaskResource();
    ~MaskResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
