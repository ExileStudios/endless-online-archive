#include <vcl.h>
#pragma hdrstop

#include "Gfx_balloon.h"
#include "Tmainform.h"
#include "Gfx_game.h"

#pragma package(smart_init)

Balloon::Balloon(String sender, String text, int balloon_style)
{
    sender_name = sender;
    style = balloon_style;
    WrapText(text);
    CreateSurfaces();
    Render();
}

Balloon::~Balloon()
{
    fill_surface->Destroy();
    text_surface->Destroy();
    delete fill_surface;
    delete text_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    fill_fx->Clear();
    delete color_key;
    delete fill_fx;
}

// Wraps a speech balloon's text into at most six lines, with the sender's name first when the setting shows it, and
// sizes the balloon to the widest line.
void Balloon::WrapText(String text)
{
    int max_width = 150;
    int width = 0;
    int height = 0;
    text = text.Trim();
    for (int i = 0; i < 6; i++)
        lines[i] = "";
    if (MainForm->fonts->BeginOffscreenIf(6))
    {
        MainForm->fonts->color = 0x80808;
        MainForm->fonts->face = 1;
        MainForm->fonts->size = 8;
        if (MainForm->config->GetShowBalloonName() && sender_name.Length() > 0)
        {
            width = (short)MainForm->fonts->TextWidth(sender_name);
            height = (short)MainForm->fonts->TextHeight(sender_name) + height + 1;
        }
        for (int line = 0; line < 6; line++)
        {
            bool line_done = false;
            for (int length = 1; length <= text.Length(); length++)
            {
                String piece = text.SubString(1, length);
                if (length == text.Length())
                    line_done = true;
                else if ((short)MainForm->fonts->TextWidth(piece) > max_width - 50)
                {
                    if (piece[length] == ' ')
                        line_done = true;
                    if ((short)MainForm->fonts->TextWidth(piece) > max_width)
                    {
                        piece += "-";
                        line_done = true;
                    }
                }
                if (line_done)
                {
                    piece = piece.Trim();
                    if ((short)MainForm->fonts->TextWidth(piece) > width)
                        width = (short)MainForm->fonts->TextWidth(piece);
                    lines[line] = piece;
                    height = (short)MainForm->fonts->TextHeight(piece) + height + 1;
                    text.Delete(1, length);
                    break;
                }
            }
        }
        MainForm->fonts->End();
    }
    text_width = width;
    text_height = height;
}

// Creates the balloon's fill and text surfaces at its text size plus a margin, both cleared to black.
void Balloon::CreateSurfaces()
{
    fill_surface = new TDx_DrawSurface(NULL);
    text_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    bubble_rect.left = 0;
    bubble_rect.top = 0;
    bubble_rect.right = text_width + 16;
    bubble_rect.bottom = text_height + 13;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = bubble_rect.right;
    surface_desc->Height = bubble_rect.bottom;
    fill_surface->GetSurfaceDesc(surface_desc);
    fill_surface->Create(surface_desc, MainForm->Dx_Draw1);
    text_surface->GetSurfaceDesc(surface_desc);
    text_surface->Create(surface_desc, MainForm->Dx_Draw1);
    fill_surface->Blt(&bubble_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
    text_surface->Blt(&bubble_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
}

// Draws a speech balloon onto its surfaces: the rounded box and its pointer in the colours of its style, the sender's
// name when the setting shows it, and its lines of text.
void Balloon::Render()
{
    int text_y;
    if (MainForm->fonts->BeginOn(fill_surface))
    {
        try
        {
            MainForm->fonts->color = 0x101010;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            TCanvas *canvas = MainForm->fonts->GetCanvas();
            canvas->Brush->Color = clWhite;
            canvas->Brush->Style = bsSolid;
            canvas->Pen->Color = clWhite;
            canvas->Pen->Style = psSolid;
            canvas->RoundRect(0, 0, text_width + 15, text_height + 9, 10, 10);
            canvas->PenPos = TPoint((text_width + 15) / 2 - 1, text_height + 8);
            canvas->LineTo((text_width + 15) / 2 + 2, text_height + 8);
            canvas->PenPos = TPoint((text_width + 15) / 2 - 1, text_height + 9);
            canvas->LineTo((text_width + 15) / 2 + 2, text_height + 9);
            canvas->PenPos = TPoint((text_width + 15) / 2, text_height + 10);
            canvas->LineTo((text_width + 15) / 2, text_height + 12);
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    if (MainForm->fonts->BeginOn(text_surface))
    {
        try
        {
            MainForm->fonts->color = 0x101010;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            TCanvas *outline_canvas = MainForm->fonts->GetCanvas();
            outline_canvas->Brush->Color = clBlack;
            outline_canvas->Brush->Style = bsClear;
            outline_canvas->Pen->Color = (TColor)0x101010;
            outline_canvas->Pen->Style = psSolid;
            outline_canvas->RoundRect(0, 0, text_width + 15, text_height + 9, 10, 10);
            TPoint tail[5];
            outline_canvas->Pen->Color = clBlack;
            tail[0].x = (text_width + 15) / 2 - 2;
            tail[0].y = text_height + 8;
            tail[1].x = (text_width + 15) / 2 + 2;
            tail[1].y = text_height + 8;
            outline_canvas->Polyline(tail, 1);
            outline_canvas->Pen->Color = (TColor)0x101010;
            tail[0].x = (text_width + 15) / 2 - 2;
            tail[0].y = text_height + 8;
            tail[1].x = (text_width + 15) / 2 - 1;
            tail[1].y = text_height + 11;
            tail[2].x = (text_width + 15) / 2;
            tail[2].y = text_height + 12;
            tail[3].x = (text_width + 15) / 2 + 1;
            tail[3].y = text_height + 11;
            tail[4].x = (text_width + 15) / 2 + 2;
            tail[4].y = text_height + 8;
            outline_canvas->Polyline(tail, 4);
            text_y = 5;
            if (MainForm->config->GetShowBalloonName() && sender_name.Length() > 0)
            {
                if (!style)
                    MainForm->fonts->color = 0x214561;
                if (style == 1)
                    MainForm->fonts->color = 0x101096;
                if (style == 2)
                    MainForm->fonts->color = 0x109610;
                if (style == 3)
                    MainForm->fonts->color = 0x961010;
                if (style == 4)
                    MainForm->fonts->color = 0xAA0F78;
                if (style == 5)
                    MainForm->fonts->color = 0x787E1E;
                if (style == 6)
                    MainForm->fonts->color = 0x46F0FF;
                MainForm->fonts->DrawString(7, text_y, sender_name);
                text_y += 13;
            }
            MainForm->fonts->color = 0x101010;
            for (int i = 0; i <= 6; i++)
            {
                if (lines[i].Length() > 0)
                {
                    MainForm->fonts->DrawString(7, text_y, lines[i]);
                    text_y += 13;
                }
            }
            delete (char *)tail;
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
        text_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    }
}

// Draws the balloon's fill at alpha 160 with its text over it.
void Balloon::Draw(int x, int y)
{
    MainForm->gfx->game->BlitClippedAlpha(x, y, fill_surface, bubble_rect, 160);
    MainForm->gfx->game->BlitClipped(x, y, text_surface, bubble_rect);
}
