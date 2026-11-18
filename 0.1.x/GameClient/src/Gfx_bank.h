#ifndef Gfx_bankH
#define Gfx_bankH

struct GraphicSize
{
    int width;
    int height;
};
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The background surface of the bank window, drawn opaque.
struct BankGraphics
{
    TDx_DrawSurface *surface;
    Windows::TRect surface_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    BankGraphics();
    ~BankGraphics();
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
