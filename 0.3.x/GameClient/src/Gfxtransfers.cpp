#include <vcl.h>
#pragma hdrstop

#include "Gfxtransfers.h"
#include "Tmainform.h"

#pragma package(smart_init)

GfxTransfers::GfxTransfers()
{
    bitmap = new Graphics::TBitmap();
    canvas = new TCanvas();
    width = -1;
    height = -1;
    has_dc = false;
    is_loaded = false;
}

GfxTransfers::~GfxTransfers()
{
    delete bitmap;
    delete canvas;
}

// Loads a bitmap resource from the library, noting its size; false, with a size of -1, when it is missing.
bool GfxTransfers::LoadResourceBitmap(int library, int resource_id)
{
    bitmap->Width = 0;
    bitmap->Height = 0;
    try
    {
        bitmap->LoadFromResourceID(library, resource_id);
    }
    catch (...)
    {
        width = -1;
        height = -1;
        is_loaded = false;
        return false;
    }
    bitmap->Transparent = false;
    width = bitmap->Width;
    height = bitmap->Height;
    is_loaded = true;
    return true;
}

// Makes the colour of the bitmap's pixel at the point the transparent one.
void GfxTransfers::SetTransparentPixel(int x, int y)
{
    if (x <= bitmap->Width && y <= bitmap->Height)
    {
        int pixel = bitmap->Canvas->Pixels[x][y];
        key_red = pixel;
        key_blue = pixel >> 16;
        key_green = (unsigned short)pixel >> 8;
        SetTransparentColor((unsigned char)key_red | ((unsigned short)(unsigned char)key_green << 8) |
            (key_blue << 16));
    }
}

void GfxTransfers::SetTransparentColor(int color)
{
    bitmap->TransparentColor = (TColor)color;
    bitmap->Transparent = true;
}

// Takes the surface's drawing context for the loaded bitmap, with a memory context too when drawing by BitBlt;
// false when nothing is loaded or the surface refuses.
bool GfxTransfers::BeginDraw(TDx_DrawSurface *surface)
{
    if (!is_loaded)
        return false;
    if (surface->GetDC(&dc))
    {
        if (MainForm->config->GetUseBitBltDraw())
        {
            HDC memory_dc = CreateCompatibleDC(0);
            bitmap_dc = memory_dc;
            SelectObject(memory_dc, bitmap->Handle);
        }
        has_dc = true;
        return true;
    }
    has_dc = false;
    return false;
}

// Releases the surface's DC after drawing.
void GfxTransfers::EndDraw(TDx_DrawSurface *surface)
{
    if (has_dc)
    {
        surface->ReleaseDC(dc);
        if (MainForm->config->GetUseBitBltDraw())
            DeleteDC(bitmap_dc);
        has_dc = false;
    }
}

void GfxTransfers::Draw(TDx_DrawSurface *surface)
{
    DrawAt(surface, 0, 0);
}

// Draws the loaded bitmap onto the surface at the point, by BitBlt when the setting asks for it, keeping its
// transparent colour when it has one; false when nothing is loaded.
bool GfxTransfers::DrawAt(TDx_DrawSurface *surface, int x, int y)
{
    if (!is_loaded)
        return false;
    if (bitmap->Width <= 0)
        return false;
    if (MainForm->config->GetUseBitBltDraw())
    {
        try
        {
            if (!bitmap->Transparent)
                BitBlt(dc, x, y, bitmap->Width, bitmap->Height, bitmap_dc, 0, 0, SRCCOPY);
            else
            {
                unsigned char red = bitmap->TransparentColor;
                unsigned char green = (unsigned short)bitmap->TransparentColor >> 8;
                unsigned char blue = bitmap->TransparentColor >> 16;
                Graphics::TBitmap *buffer = new Graphics::TBitmap();
                buffer->Width = bitmap->Width;
                buffer->Height = bitmap->Height;
                HDC buffer_dc = CreateCompatibleDC(0);
                SelectObject(buffer_dc, buffer->Handle);
                BitBlt(buffer_dc, 0, 0, bitmap->Width, bitmap->Height, dc, x, y, SRCCOPY);
                TransparentBlt(buffer_dc, 0, 0, bitmap->Width, bitmap->Height, bitmap_dc, 0, 0, bitmap->Width,
                    bitmap->Height, (unsigned int)(red | ((unsigned short)green << 8) | (blue << 16)));
                BitBlt(dc, x, y, buffer->Width, buffer->Height, buffer_dc, 0, 0, SRCCOPY);
            }
        }
        catch (Exception &)
        {
            return false;
        }
    }
    else
    {
        try
        {
            canvas->Handle = dc;
            canvas->Draw(x, y, bitmap);
            canvas->Handle = 0;
        }
        catch (Exception &)
        {
            return false;
        }
    }
    return true;
}

// Creates the surface at the size as a plain offscreen one in system memory, destroying an earlier one.
void GfxTransfers::CreateSurface(TDx_DrawSurface *surface, TDDSurfaceDesc *desc, int surface_width, int surface_height)
{
    desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    desc->Width = surface_width;
    desc->Height = surface_height;
    if (!surface)
        surface->Restore();
    if (surface->Created)
        surface->Destroy();
    surface->GetSurfaceDesc(desc);
    surface->Create(desc, MainForm->Dx_Draw1);
}
