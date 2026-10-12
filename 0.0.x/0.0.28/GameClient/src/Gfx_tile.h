#ifndef Gfx_tileH
#define Gfx_tileH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one ground tile, filled from a bitmap resource of its library.
struct TileResource
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    char is_animated;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TileResource();
    ~TileResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

Windows::TRect Gfx_tile_GetFrameRect(TileResource *self, int frame);

#endif
