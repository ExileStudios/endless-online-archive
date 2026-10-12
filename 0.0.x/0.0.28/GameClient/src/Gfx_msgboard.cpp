#include <vcl.h>
#pragma hdrstop

#include "Gfx_msgboard.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
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
Windows::TRect MsgBoardGraphics::GetFrameRect(MsgBoardGraphics *self, int frame)
{
    TRect frame_rect;
    if (frame > 1)
        frame = 1;
    frame_rect.top = TALL_FRAME_HEIGHT * frame;
    frame_rect.bottom = (TALL_FRAME_HEIGHT *frame) + 290;
    frame_rect.left = 0;
    frame_rect.right = 484;
    return frame_rect;
}

// Loads the message board sheet from the library into a surface sized to the bitmap; returns its
// size, or -1 when the bitmap could not be drawn onto the message board surface.
GraphicSize MsgBoardGraphics::LoadSurface(HMODULE library, int resource_id)
{
    GraphicSize size;
    HDC dc;
    HDC CompatibleDC;
    size.width = -1;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID((int)library, resource_id);
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = MainForm->resource_bitmap->Width;
    surface_desc->Height = MainForm->resource_bitmap->Height;
    if (!surface)
        surface->Restore();
    if (surface->Created)
        surface->Destroy();
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    size.width = MainForm->resource_bitmap->Width;
    size.height = MainForm->resource_bitmap->Height;
    sheet_left = 0;
    sheet_top = 0;
    sheet_width = MainForm->resource_bitmap->Width;
    sheet_height = MainForm->resource_bitmap->Height;
    // The message board bitmap goes on by a canvas Draw, or by BitBlt if configured.
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (surface->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
                surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            surface->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (surface->GetDC(&dc))
        {
            try
            {
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376);
            }
            catch (...)
            {
                surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            surface->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
    return size;
}
