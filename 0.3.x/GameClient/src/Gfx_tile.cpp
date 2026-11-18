#include <vcl.h>
#pragma hdrstop

#include "Gfx_tile.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

TileResource::TileResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    is_animated = false;
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

TileResource::~TileResource()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// One animation frame of a ground tile: a quarter of a wide sheet, or the whole of a narrow one.
TRect TileResource::GetFrameRect(int frame)
{
    TRect frame_rect;
    if (surface_rect.right > 250)
    {
        int frame_width = surface_rect.right / 4;
        frame_rect.left = frame_width * frame;
        frame_rect.top = 0;
        frame_rect.right = frame_width + frame_width * frame;
        frame_rect.bottom = surface_rect.bottom;
    }
    else
    {
        frame_rect.left = 0;
        frame_rect.top = 0;
        frame_rect.right = surface_rect.right;
        frame_rect.bottom = surface_rect.bottom;
    }
    return frame_rect;
}

// Loads a ground tile picture from the library into its surface, animated when 250 or more wide; its size, or -1,
// -1 when it cannot be drawn.
TPoint TileResource::LoadSurface(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y);
    surface_rect.left = 0;
    surface_rect.top = 0;
    surface_rect.right = size.x;
    surface_rect.bottom = size.y;
    if (size.x >= 250)
        is_animated = true;
    if (MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
        surface->SetColorKey(DDCKEY_SRCBLT, color_key);
        return size;
    }
    surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    return TPoint(-1, -1);
}
