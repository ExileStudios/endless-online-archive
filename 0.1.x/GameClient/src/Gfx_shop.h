#ifndef Gfx_shopH
#define Gfx_shopH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The shop window surfaces: the opaque background and the item slot frame.
// The guild, inn, law, learn, locker, memo and report windows draw from it too.
struct ShopGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *item_slot_surface;
    Windows::TRect background_rect;
    Windows::TRect item_slot_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ShopGraphics();
    ~ShopGraphics();
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadItemSlot(HMODULE library, int resource_id);
};

#endif
