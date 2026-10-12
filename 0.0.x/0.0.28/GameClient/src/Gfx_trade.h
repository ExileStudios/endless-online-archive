#ifndef Gfx_tradeH
#define Gfx_tradeH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The trade window surfaces: the opaque background and the item slot frame.
struct TradeGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *item_slot_surface;
    int background_left;
    int background_top;
    int background_width;
    int background_height;
    Windows::TRect item_slot_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    TradeGraphics();
    ~TradeGraphics();
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadItemSlot(HMODULE library, int resource_id);
};

#endif
