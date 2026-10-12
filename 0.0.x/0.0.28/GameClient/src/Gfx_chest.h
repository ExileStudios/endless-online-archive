#ifndef Gfx_chestH
#define Gfx_chestH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The chest window surfaces: the opaque background and the item slot frame.
struct ChestGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *item_slot_surface;
    Windows::TRect background_rect;
    Windows::TRect item_slot_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    ChestGraphics();
    ~ChestGraphics();
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadItemSlot(HMODULE library, int resource_id);
};

#endif
