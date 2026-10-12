#include <vcl.h>
#pragma hdrstop

#include "Gfx_tile.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

TileResource::TileResource()
{
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    is_animated = 0;
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
Windows::TRect Gfx_tile_GetFrameRect(TileResource *self, int frame)
{
    TRect frame_rect;
    if (self->surface_rect.right > 250)
    {
        int frame_width = self->surface_rect.right / 4;
        frame_rect.left = frame_width * frame;
        frame_rect.top = 0;
        frame_rect.right = frame_width * frame + frame_width;
        frame_rect.bottom = self->surface_rect.bottom;
    }
    else
    {
        frame_rect.left = 0;
        frame_rect.top = 0;
        frame_rect.right = self->surface_rect.right;
        frame_rect.bottom = self->surface_rect.bottom;
    }
    return frame_rect;
}

// Loads bitmap resource_id from the library into the ground tile surface and returns its size.
// The size is -1 when the bitmap could not be drawn onto the ground tile surface.
GraphicSize TileResource::LoadSurface(int library, int resource_id)
{
    GraphicSize size;
    HDC dc;
    HDC CompatibleDC;
    size.width = -1;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
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
    surface_rect.left = 0;
    surface_rect.top = 0;
    surface_rect.right = MainForm->resource_bitmap->Width;
    surface_rect.bottom = MainForm->resource_bitmap->Height;
    // A ground tile sheet this wide holds four animation frames, so the map keeps redrawing it.
    if (MainForm->resource_bitmap->Width >= 250)
        is_animated = 1;
    // The ground tile bitmap goes on by a canvas Draw, or by BitBlt if configured.
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
    // The colour key makes black see-through on the ground tile.
    if (size.width > 0)
        surface->SetColorKey(8, color_key);
    return size;
}
