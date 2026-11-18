#include <vcl.h>
#pragma hdrstop

#include "Gfx_msgboard.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

MsgBoardGraphics::MsgBoardGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

MsgBoardGraphics::~MsgBoardGraphics()
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

// One of the two frames of the message board sheet; a higher frame number gives the second.
TRect MsgBoardGraphics::GetFrameRect(int frame)
{
    TRect frame_rect;
    if (frame > 1)
        frame = 1;
    frame_rect.top = 290 * frame;
    frame_rect.bottom = 290 * frame + 290;
    frame_rect.left = 0;
    frame_rect.right = 484;
    return frame_rect;
}

// Loads a picture from the library into the message board surface; its size, or -1, -1 when it cannot be drawn.
TPoint MsgBoardGraphics::LoadSurface(int library, int resource_id)
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
    if (MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
        return size;
    }
    return TPoint(-1, -1);
}
