#include <vcl.h>
#pragma hdrstop

#include "Gfx_wall.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

WallResource::WallResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    is_animated = false;
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

WallResource::~WallResource()
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

// One animation frame of a wall graphic: a quarter of a wide sheet, or the whole of a narrow one.
TRect WallResource::GetFrameRect(int frame)
{
    TRect frame_rect;
    if (sheet_width > 120)
    {
        int frame_width = sheet_width / 4;
        frame_rect.left = frame_width * frame;
        frame_rect.top = 0;
        frame_rect.right = frame_width + frame_width * frame;
        frame_rect.bottom = sheet_height;
    }
    else
    {
        frame_rect.left = 0;
        frame_rect.top = 0;
        frame_rect.right = sheet_width;
        frame_rect.bottom = sheet_height;
    }
    return frame_rect;
}

// Loads a wall picture from the library into its surface, animated when 120 or more wide; its size, or -1, -1 when
// it cannot be drawn.
TPoint WallResource::LoadSurface(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y);
    sheet_left = 0;
    sheet_top = 0;
    sheet_width = size.x;
    sheet_height = size.y;
    if (size.x >= 120)
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
