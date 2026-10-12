#include <vcl.h>
#pragma hdrstop

#include "Gfx_balloon.h"
#include "Tmainform.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

// Creates both balloon surfaces sized to the text. The fill surface gets the filled bubble and its tail; the text
// surface, whose black is see-through, gets the bubble's outline, the tail's outline and the six text lines.
Balloon::Balloon(int text_width, int text_height, String *lines, int style)
{
    HDC dc;
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
    bubble_rect.bottom = text_height + 12;
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = text_width + 16;
    surface_desc->Height = text_height + 12;
    fill_surface->GetSurfaceDesc(surface_desc);
    fill_surface->Create(surface_desc, MainForm->Dx_Draw1);
    text_surface->GetSurfaceDesc(surface_desc);
    text_surface->Create(surface_desc, MainForm->Dx_Draw1);
    fill_surface->Blt(&bubble_rect, 0, 0, 16778240, fill_fx);
    text_surface->Blt(&bubble_rect, 0, 0, 16778240, fill_fx);
    if (fill_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            // A group message balloon has its own fill colour; every other balloon is white.
            if (style == 1)
                MainForm->draw_canvas->Brush->Color = (TColor)0xAAC8DC;
            else
                MainForm->draw_canvas->Brush->Color = clWhite;
            MainForm->draw_canvas->Brush->Style = bsSolid;
            MainForm->draw_canvas->Pen->Color = (TColor)0x101010;
            MainForm->draw_canvas->Pen->Style = psSolid;
            MainForm->draw_canvas->Font->Color = (TColor)0x101010;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            MainForm->draw_canvas->RoundRect(0, 0, text_width + 15, text_height + 9, 10, 10);
            TPoint tail_points[4];
            if (style == 1)
                MainForm->draw_canvas->Pen->Color = (TColor)0xAAC8DC;
            else
                MainForm->draw_canvas->Pen->Color = clWhite;
            tail_points[0].x = (text_width + 15) / 2 - 1; tail_points[0].y = text_height + 8;
            tail_points[1].x = (text_width + 15) / 2 + 1; tail_points[1].y = text_height + 8;
            tail_points[2].x = (text_width + 15) / 2; tail_points[2].y = text_height + 8;
            tail_points[3].x = (text_width + 15) / 2; tail_points[3].y = text_height + 12;
            MainForm->draw_canvas->Polyline(tail_points, 3);
            MainForm->draw_canvas->Handle = 0;
            delete tail_points;
        }
        catch (Exception &)
        {
        }
        fill_surface->ReleaseDC(dc);
    }
    if (text_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Color = clBlack;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Pen->Color = (TColor)0x101010;
            MainForm->draw_canvas->Pen->Style = psSolid;
            MainForm->draw_canvas->Font->Color = (TColor)0x101010;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            MainForm->draw_canvas->RoundRect(0, 0, text_width + 15, text_height + 9, 10, 10);
            TPoint outline_points[5];
            MainForm->draw_canvas->Pen->Color = clBlack;
            outline_points[0].x = (text_width + 15) / 2 - 2; outline_points[0].y = text_height + 8;
            outline_points[1].x = (text_width + 15) / 2 + 2; outline_points[1].y = text_height + 8;
            MainForm->draw_canvas->Polyline(outline_points, 1);
            MainForm->draw_canvas->Pen->Color = (TColor)0x101010;
            outline_points[0].x = (text_width + 15) / 2 - 2; outline_points[0].y = text_height + 8;
            outline_points[1].x = (text_width + 15) / 2 - 1; outline_points[1].y = text_height + 11;
            outline_points[2].x = (text_width + 15) / 2; outline_points[2].y = text_height + 12;
            outline_points[3].x = (text_width + 15) / 2 + 1; outline_points[3].y = text_height + 11;
            outline_points[4].x = (text_width + 15) / 2 + 2; outline_points[4].y = text_height + 8;
            MainForm->draw_canvas->Polyline(outline_points, 4);
            MainForm->draw_canvas->TextOut(7, 5, lines[0]);
            MainForm->draw_canvas->TextOut(7, 18, lines[1]);
            MainForm->draw_canvas->TextOut(7, 31, lines[2]);
            MainForm->draw_canvas->TextOut(7, 44, lines[3]);
            MainForm->draw_canvas->TextOut(7, 57, lines[4]);
            MainForm->draw_canvas->TextOut(7, 70, lines[5]);
            MainForm->draw_canvas->Handle = 0;
            delete outline_points;
        }
        catch (Exception &)
        {
        }
        text_surface->ReleaseDC(dc);
        text_surface->SetColorKey(8, color_key);
    }
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
