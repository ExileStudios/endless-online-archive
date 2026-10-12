#include <vcl.h>
#pragma hdrstop

#include "Gfx_musician.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

MusicianGraphics::MusicianGraphics()
{
    background_surface = new TDx_DrawSurface(NULL);
    cell_sheet_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

MusicianGraphics::~MusicianGraphics()
{
    background_surface->Destroy();
    delete background_surface;
    cell_sheet_surface->Destroy();
    delete cell_sheet_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// One cell of the musician cell sheet, by column and row.
Windows::TRect MusicianGraphics::GetCellRect(MusicianGraphics *self, int column, int row)
{
    TRect cell_rect;
    cell_rect.top = 20 * row;
    cell_rect.bottom = 20 * row + 20;
    cell_rect.left = 20 * column;
    cell_rect.right = 20 * column + 20;
    return cell_rect;
}

// Loads the musician window background into a surface sized to the bitmap; returns its size,
// or -1 when the bitmap could not be drawn onto the musician background surface.
DEFINE_LOAD_BACKGROUND(MusicianGraphics)

// Loads the musician cell sheet into its own surface, without a colour key; returns its size,
// or -1 when it could not be drawn.
GraphicSize MusicianGraphics::LoadCellSheet(HMODULE library, int resource_id)
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
    if (!cell_sheet_surface)
        cell_sheet_surface->Restore();
    if (cell_sheet_surface->Created)
        cell_sheet_surface->Destroy();
    cell_sheet_surface->GetSurfaceDesc(surface_desc);
    cell_sheet_surface->Create(surface_desc, MainForm->Dx_Draw1);
    size.width = MainForm->resource_bitmap->Width;
    size.height = MainForm->resource_bitmap->Height;
    // The musician cell sheet bitmap goes on by a canvas Draw, or by BitBlt if configured.
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (cell_sheet_surface->GetDC(&dc))
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
                cell_sheet_surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            cell_sheet_surface->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (cell_sheet_surface->GetDC(&dc))
        {
            try
            {
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376);
            }
            catch (...)
            {
                cell_sheet_surface->ReleaseDC(dc);
                size.width = -1;
                size.height = -1;
            }
            cell_sheet_surface->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
    return size;
}
