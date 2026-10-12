#ifndef Gfx_wallH
#define Gfx_wallH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one wall graphic, filled from a bitmap resource of its library.
struct WallResource
{
    TDx_DrawSurface *surface;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    char is_animated;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    WallResource();
    ~WallResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

Windows::TRect Gfx_wall_GetFrameRect(WallResource *self, int frame);

#endif
