#include <vcl.h>
#pragma hdrstop

#include "Gfx_musician.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

MusicianGraphics::MusicianGraphics()
{
    background_surface = new TDx_DrawSurface(NULL);
    cell_sheet_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    background_loaded = false;
    cell_sheet_loaded = false;
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
TRect MusicianGraphics::GetCellRect(int column, int row)
{
    TRect cell_rect;
    cell_rect.top = 20 * row;
    cell_rect.bottom = 20 * row + 20;
    cell_rect.left = 20 * column;
    cell_rect.right = 20 * column + 20;
    return cell_rect;
}

// Loads the musician window's background from the library; its size, or -1, -1 when it cannot be drawn.
TPoint MusicianGraphics::LoadBackground(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(background_surface, surface_desc, size.x, size.y);
    background_rect.left = 0;
    background_rect.top = 0;
    background_rect.right = size.x;
    background_rect.bottom = size.y;
    if (MainForm->gfx_transfers->BeginDraw(background_surface))
    {
        MainForm->gfx_transfers->Draw(background_surface);
        MainForm->gfx_transfers->EndDraw(background_surface);
        background_loaded = true;
        return size;
    }
    return TPoint(-1, -1);
}

// Loads the musician window's note cell sheet from the library; its size, or -1, -1 when it cannot be drawn.
TPoint MusicianGraphics::LoadCellSheet(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(cell_sheet_surface, surface_desc, size.x, size.y);
    if (MainForm->gfx_transfers->BeginDraw(cell_sheet_surface))
    {
        MainForm->gfx_transfers->Draw(cell_sheet_surface);
        MainForm->gfx_transfers->EndDraw(cell_sheet_surface);
        cell_sheet_loaded = true;
        return size;
    }
    return TPoint(-1, -1);
}
