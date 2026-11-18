#include <vcl.h>
#pragma hdrstop

#include "Gfx_player.h"
#include "Tmainform.h"

#pragma package(smart_init)

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
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
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
    front_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
    back_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
    emote_strip->SetColorKey(DDCKEY_SRCBLT, color_key);
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
    front_sheet->Blt(&sheet_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
    back_sheet->Blt(&sheet_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
}

TPoint PlayerResources::DrawFrontLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentColor(clBlack);
        if (MainForm->gfx_transfers->BeginDraw(front_sheet))
        {
            MainForm->gfx_transfers->DrawAt(front_sheet, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset);
            MainForm->gfx_transfers->EndDraw(front_sheet);
            return size;
        }
        return TPoint(-1, -1);
    }
}

// Draws a picture from the library onto the back sheet at the frame slot and offset, black as transparent; its
// size, or -1, -1.
TPoint PlayerResources::DrawBackLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentColor(clBlack);
        if (MainForm->gfx_transfers->BeginDraw(back_sheet))
        {
            MainForm->gfx_transfers->DrawAt(back_sheet, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset);
            MainForm->gfx_transfers->EndDraw(back_sheet);
            return size;
        }
        return TPoint(-1, -1);
    }
}

TPoint PlayerResources::DrawFrontLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library,
    int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentPixel(1, 32);
        if (MainForm->gfx_transfers->BeginDraw(front_sheet))
        {
            MainForm->gfx_transfers->DrawAt(front_sheet, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset);
            MainForm->gfx_transfers->EndDraw(front_sheet);
            return size;
        }
        return TPoint(-1, -1);
    }
}

TPoint PlayerResources::DrawBackLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library,
    int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    else
    {
        TPoint size = *(TPoint *)MainForm->gfx_transfers;
        MainForm->gfx_transfers->SetTransparentPixel(1, 32);
        if (MainForm->gfx_transfers->BeginDraw(back_sheet))
        {
            MainForm->gfx_transfers->DrawAt(back_sheet, PLAYER_FRAME_WIDTH * frame_slot + x_offset, y_offset);
            MainForm->gfx_transfers->EndDraw(back_sheet);
            return size;
        }
        return TPoint(-1, -1);
    }
}

char PlayerResources::DrawFrontLayerFrom(int frame_slot, int x_offset, int y_offset, TDx_DrawSurface *source,
    TRect source_rect)
{
    front_sheet->BltFast(68 * frame_slot + x_offset, y_offset, source, &source_rect,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

char PlayerResources::DrawBackLayerFrom(int frame_slot, int x_offset, int y_offset, TDx_DrawSurface *source,
    TRect source_rect)
{
    back_sheet->BltFast(68 * frame_slot + x_offset, y_offset, source, &source_rect,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the name on the name tag surface in the colour of its index and sizes the tag to it, at most 240 by 26.
void PlayerResources::RenderNameTag(String text, int color_index)
{
    name_tag_surface->Blt(&name_tag_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
    if (MainForm->fonts->BeginOn(name_tag_surface))
    {
        try
        {
            if (!color_index)
                MainForm->fonts->color = 0xFFFFFF;
            if (color_index == LabelColor_Gray)
                MainForm->fonts->color = 0xB9B9B9;
            if (color_index == LabelColor_Silver)
                MainForm->fonts->color = 0xE1E1E1;
            if (color_index == LabelColor_LightBlue)
                MainForm->fonts->color = 0x9CC8F5;
            if (color_index == LabelColor_Violet)
                MainForm->fonts->color = 0x9CC8F5;
            if (color_index == LabelColor_Gold)
                MainForm->fonts->color = 0xA5F0FF;
            if (color_index == LabelColor_Cream)
                MainForm->fonts->color = 0xD2FAFA;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            MainForm->fonts->DrawString(1, 0, text);
            name_tag_text_rect.right = (short)MainForm->fonts->TextWidth(text) + 4;
            name_tag_text_rect.bottom = (short)MainForm->fonts->TextHeight(text) + 1;
            if (name_tag_text_rect.right > 240)
                name_tag_text_rect.right = 240;
            if (name_tag_text_rect.bottom > 26)
                name_tag_text_rect.bottom = 26;
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    name_tag_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// One frame of a player sheet.
TRect PlayerResources::GetFrameRect(int frame)
{
    TRect frame_rect;
    frame_rect.left = 68 * frame;
    frame_rect.top = 0;
    frame_rect.right = 68 * frame + 68;
    frame_rect.bottom = 84;
    return frame_rect;
}

// One face of the emote strip.
TRect PlayerResources::GetEmoteFaceRect(int emote_index)
{
    TRect emote_face_rect;
    emote_face_rect.left = 13 * emote_index;
    emote_face_rect.top = 0;
    emote_face_rect.right = 13 * emote_index + 13;
    emote_face_rect.bottom = 14;
    return emote_face_rect;
}
