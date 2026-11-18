#include <vcl.h>
#pragma hdrstop

#include "Gfx_crafter.h"
#include "Tmainform.h"

#pragma package(smart_init)

// Creates the crafter window's surfaces and sets the source rects of its parts.
CrafterGraphics::CrafterGraphics()
{
    parts_surface = new TDx_DrawSurface(NULL);
    background_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    upper_background_rect.left = 0;
    upper_background_rect.right = 290;
    upper_background_rect.top = 0;
    upper_background_rect.bottom = 190;
    full_background_rect.left = 0;
    full_background_rect.right = 290;
    full_background_rect.top = 0;
    full_background_rect.bottom = 260;
    lower_background_rect.left = 0;
    lower_background_rect.right = 290;
    lower_background_rect.top = 260;
    lower_background_rect.bottom = 290;
    slot_frame_rect.left = 290;
    slot_frame_rect.right = 367;
    slot_frame_rect.top = 0;
    slot_frame_rect.bottom = 70;
    left_link_rect.left = 291;
    left_link_rect.right = 302;
    left_link_rect.top = 225;
    left_link_rect.bottom = 236;
    right_link_rect.left = 302;
    right_link_rect.right = 313;
    right_link_rect.top = 225;
    right_link_rect.bottom = 236;
    center_link_rect.left = 291;
    center_link_rect.right = 343;
    center_link_rect.top = 214;
    center_link_rect.bottom = 225;
    four_glow_rect.left = 292;
    four_glow_rect.right = 329;
    four_glow_rect.top = 237;
    four_glow_rect.bottom = 277;
    left_glow_rect.left = 330;
    left_glow_rect.right = 363;
    left_glow_rect.top = 224;
    left_glow_rect.bottom = 250;
    right_glow_rect.left = 330;
    right_glow_rect.right = 363;
    right_glow_rect.top = 250;
    right_glow_rect.bottom = 276;
    lower_glow_rect.left = 300;
    lower_glow_rect.right = 312;
    lower_glow_rect.top = 80;
    lower_glow_rect.bottom = 99;
    blocked_mark_rect.left = 314;
    blocked_mark_rect.right = 324;
    blocked_mark_rect.top = 225;
    blocked_mark_rect.bottom = 236;
}

CrafterGraphics::~CrafterGraphics()
{
    parts_surface->Destroy();
    background_surface->Destroy();
    delete parts_surface;
    delete background_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

TRect CrafterGraphics::GetSlotRect(int slot)
{
    TRect rect;
    rect.left = 294;
    rect.right = 365;
    rect.top = 70 * slot + 74;
    rect.bottom = 70 * slot + 138;
    return rect;
}

// Loads the crafting window's background from the library; its size, or -1, -1 when it cannot be drawn.
TPoint CrafterGraphics::LoadBackground(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(background_surface, surface_desc, size.x, size.y);
    background_left = 0;
    background_top = 0;
    background_right = size.x;
    background_bottom = size.y;
    if (MainForm->gfx_transfers->BeginDraw(background_surface))
    {
        MainForm->gfx_transfers->Draw(background_surface);
        MainForm->gfx_transfers->EndDraw(background_surface);
        background_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
        return size;
    }
    return TPoint(-1, -1);
}

// Loads the crafting window's parts sheet from the library; its size, or -1, -1 when it cannot be drawn.
TPoint CrafterGraphics::LoadParts(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(parts_surface, surface_desc, size.x, size.y);
    parts_rect.left = 0;
    parts_rect.top = 0;
    parts_rect.right = size.x;
    parts_rect.bottom = size.y;
    if (MainForm->gfx_transfers->BeginDraw(parts_surface))
    {
        MainForm->gfx_transfers->Draw(parts_surface);
        MainForm->gfx_transfers->EndDraw(parts_surface);
        parts_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
        return size;
    }
    parts_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    return TPoint(-1, -1);
}
