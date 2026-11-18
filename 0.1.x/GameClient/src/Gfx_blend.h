#ifndef Gfx_blendH
#define Gfx_blendH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one blended overlay, filled from a bitmap resource of its library.
struct BlendResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    BlendResource();
    ~BlendResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

#endif
