#ifndef Gfx_barberH
#define Gfx_barberH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the barber window, drawn opaque.
struct BarberGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    BarberGraphics();
    ~BarberGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
