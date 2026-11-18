#ifndef Gfx_crafterH
#define Gfx_crafterH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct CrafterGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *parts_surface;
    int background_left;
    int background_top;
    int background_right;
    int background_bottom;
    TRect parts_rect;
    TRect upper_background_rect;
    TRect lower_background_rect;
    TRect full_background_rect;
    TRect slot_frame_rect;
    TRect right_link_rect;
    TRect left_link_rect;
    TRect center_link_rect;
    TRect left_glow_rect;
    TRect right_glow_rect;
    TRect lower_glow_rect;
    TRect four_glow_rect;
    TRect blocked_mark_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    CrafterGraphics();
    ~CrafterGraphics();
    TRect GetSlotRect(int slot);
    TPoint LoadBackground(int library, int resource_id);
    TPoint LoadParts(int library, int resource_id);
};

#endif
