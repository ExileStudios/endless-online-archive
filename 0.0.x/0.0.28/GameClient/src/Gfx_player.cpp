#include <vcl.h>
#pragma hdrstop

#include "Gfx_player.h"
#include "Tmainform.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)
#define PLAYER_FRAME_WIDTH 68

PlayerResources::PlayerResources()
{
    front_sheet = new TDx_DrawSurface(NULL);
    back_sheet = new TDx_DrawSurface(NULL);
    emote_strip = new TDx_DrawSurface(NULL);
    name_tag_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = 748;
    surface_desc->Height = 84;
    front_sheet->GetSurfaceDesc(surface_desc);
    front_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    back_sheet->GetSurfaceDesc(surface_desc);
    back_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 143;
    surface_desc->Height = 14;
    emote_strip->GetSurfaceDesc(surface_desc);
    emote_strip->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 240;
    surface_desc->Height = 26;
    name_tag_surface->GetSurfaceDesc(surface_desc);
    name_tag_surface->Create(surface_desc, MainForm->Dx_Draw1);
    sheet_rect.left = 0;
    sheet_rect.top = 0;
    sheet_rect.right = 748;
    sheet_rect.bottom = 84;
    emote_strip_rect.left = 0;
    emote_strip_rect.top = 0;
    emote_strip_rect.right = 143;
    emote_strip_rect.bottom = 14;
    name_tag_rect.left = 0;
    name_tag_rect.top = 0;
    name_tag_rect.right = 240;
    name_tag_rect.bottom = 26;
    name_tag_text_rect.left = 0;
    name_tag_text_rect.top = 0;
    front_sheet->SetColorKey(8, color_key);
    back_sheet->SetColorKey(8, color_key);
    emote_strip->SetColorKey(8, color_key);
}

PlayerResources::~PlayerResources()
{
    front_sheet->Destroy();
    back_sheet->Destroy();
    emote_strip->Destroy();
    name_tag_surface->Destroy();
    delete front_sheet;
    delete back_sheet;
    delete emote_strip;
    delete name_tag_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    fill_fx->Clear();
    delete color_key;
    delete fill_fx;
}

// Clears both player sheets to black, the see-through colour, before the layers are drawn again.
void PlayerResources::ClearSheets()
{
    front_sheet->Blt(&sheet_rect, 0, 0, 16778240, fill_fx);
    back_sheet->Blt(&sheet_rect, 0, 0, 16778240, fill_fx);
}

// Draws one layer bitmap into a frame slot of the front player sheet at an offset; its black stays see-through.
void PlayerResources::DrawFrontLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    HDC dc;
    HDC CompatibleDC;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (front_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(x_offset + 68 * frame_slot, y_offset, MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            front_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (front_sheet->GetDC(&dc))
        {
            try
            {
                TransparentBlt(dc, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, CompatibleDC, 0, 0,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, 0);
            }
            catch (Exception &)
            {
            }
            front_sheet->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// Draws one layer bitmap into a frame slot of the back player sheet at an offset; its black stays see-through.
void PlayerResources::DrawBackLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    HDC dc;
    HDC CompatibleDC;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(x_offset + 68 * frame_slot, y_offset, MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            back_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                TransparentBlt(dc, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, CompatibleDC, 0, 0,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, 0);
            }
            catch (Exception &)
            {
            }
            back_sheet->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// Like DrawFrontLayer, but the see-through colour is read from a pixel of the layer bitmap itself.
void PlayerResources::DrawFrontLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    int Pixel = MainForm->resource_bitmap->Canvas->Pixels[1][32];
    BYTE red = GetRValue(Pixel);
    BYTE blue = GetBValue(Pixel);
    BYTE green = GetGValue(Pixel);
    MainForm->resource_bitmap->TransparentColor = (TColor)RGB(red, green, blue);
    MainForm->resource_bitmap->Transparent = true;
    HDC dc;
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (front_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            front_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        HDC CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (front_sheet->GetDC(&dc))
        {
            try
            {
                TransparentBlt(dc, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, CompatibleDC, 0, 0,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, RGB(red, green, blue));
            }
            catch (Exception &)
            {
            }
            front_sheet->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// Like DrawBackLayer, but the see-through colour is read from a pixel of the layer bitmap itself.
void PlayerResources::DrawBackLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    int Pixel = MainForm->resource_bitmap->Canvas->Pixels[1][32];
    BYTE red = GetRValue(Pixel);
    BYTE blue = GetBValue(Pixel);
    BYTE green = GetGValue(Pixel);
    MainForm->resource_bitmap->TransparentColor = (TColor)RGB(red, green, blue);
    MainForm->resource_bitmap->Transparent = true;
    HDC dc;
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            back_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        HDC CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                TransparentBlt(dc, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, CompatibleDC, 0, 0,
                    MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height, RGB(red, green, blue));
            }
            catch (Exception &)
            {
            }
            back_sheet->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// Draws a name onto the name tag surface in the colour that color_index picks.
// The measured text size is kept for the draw, cut to the tag surface.
void PlayerResources::RenderNameTag(String text, int color_index)
{
    HDC dc;
    name_tag_surface->Blt(&name_tag_rect, 0, 0, 16778240, fill_fx);
    if (name_tag_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Color = clBlack;
            MainForm->draw_canvas->Brush->Style = bsClear;
            if (!color_index)
            {
                MainForm->draw_canvas->Pen->Color = clWhite;
                MainForm->draw_canvas->Font->Color = clWhite;
            }
            if (color_index == 1)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xB9B9B9;
                MainForm->draw_canvas->Font->Color = (TColor)0xB9B9B9;
            }
            if (color_index == 2)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xE1E1E1;
                MainForm->draw_canvas->Font->Color = (TColor)0xE1E1E1;
            }
            if (color_index == 3)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0x9CC8F5;
                MainForm->draw_canvas->Font->Color = (TColor)0x9CC8F5;
            }
            if (color_index == 4)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0x9CC8F5;
                MainForm->draw_canvas->Font->Color = (TColor)0x9CC8F5;
            }
            if (color_index == 5)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xA5F0FF;
                MainForm->draw_canvas->Font->Color = (TColor)0xA5F0FF;
            }
            if (color_index == 6)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xD2FAFA;
                MainForm->draw_canvas->Font->Color = (TColor)0xD2FAFA;
            }
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            MainForm->draw_canvas->TextOut(1, 0, text);
            name_tag_text_rect.right = MainForm->draw_canvas->TextWidth(text) + 4;
            name_tag_text_rect.bottom = MainForm->draw_canvas->TextHeight(text) + 1;
            if (name_tag_text_rect.right > 240)
                name_tag_text_rect.right = 240;
            if (name_tag_text_rect.bottom > 26)
                name_tag_text_rect.bottom = 26;
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        name_tag_surface->ReleaseDC(dc);
    }
    name_tag_surface->SetColorKey(8, color_key);
}

// One frame of a player sheet.
Windows::TRect PlayerResources::Gfx_player_GetFrameRect(int frame)
{
    TRect frame_rect;
    frame_rect.left = 68 * frame;
    frame_rect.top = 0;
    frame_rect.right = 68 * frame + 68;
    frame_rect.bottom = 84;
    return frame_rect;
}

// One face of the emote strip.
Windows::TRect PlayerResources::Gfx_player_GetEmoteFaceRect(int emote_index)
{
    TRect emote_face_rect;
    emote_face_rect.left = 13 * emote_index;
    emote_face_rect.top = 0;
    emote_face_rect.right = 13 * emote_index + 13;
    emote_face_rect.bottom = 14;
    return emote_face_rect;
}
