#ifndef Gfx_iconH
#define Gfx_iconH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surface of one spell icon, filled from a bitmap resource of its library.
struct IconResource
{
    TDx_DrawSurface *surface;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    IconResource();
    ~IconResource();
    GraphicSize LoadSurface(int library, int resource_id);
};

Windows::TRect Gfx_icon_GetFrameRect(IconResource *self, int frame);

#endif
