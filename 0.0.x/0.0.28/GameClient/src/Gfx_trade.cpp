#include <vcl.h>
#pragma hdrstop

#include "Gfx_trade.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

TradeGraphics::TradeGraphics()
{
    item_slot_surface = new TDx_DrawSurface(NULL);
    background_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

TradeGraphics::~TradeGraphics()
{
    item_slot_surface->Destroy();
    background_surface->Destroy();
    delete item_slot_surface;
    delete background_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// Loads the trade window background from the library into a surface sized to the bitmap; returns its size,
// or -1 when the bitmap could not be drawn onto the trade background surface.
GraphicSize TradeGraphics::LoadBackground(HMODULE library, int resource_id)
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
    if (!background_surface)
        background_surface->Restore();
    if (background_surface->Created)
        background_surface->Destroy();
    background_surface->GetSurfaceDesc(surface_desc);
    background_surface->Create(surface_desc, MainForm->Dx_Draw1);
    size.width = MainForm->resource_bitmap->Width;
    size.height = MainForm->resource_bitmap->Height;
    background_left = 0;
    background_top = 0;
    background_width = MainForm->resource_bitmap->Width;
    background_height = MainForm->resource_bitmap->Height;
    // The trade background bitmap goes on by a canvas Draw, or by BitBlt if configured.
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (background_surface->GetDC(&dc))
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
                background_surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            background_surface->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (background_surface->GetDC(&dc))
        {
            try
            {
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376);
            }
            catch (...)
            {
                background_surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            background_surface->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
    return size;
}

// Loads the trade item slot frame into its own surface; returns its size, or -1 when it could not be drawn.
DEFINE_LOAD_ITEM_SLOT(TradeGraphics)
