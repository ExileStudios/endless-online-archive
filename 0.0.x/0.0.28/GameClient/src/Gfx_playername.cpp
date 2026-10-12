#include <vcl.h>
#pragma hdrstop

#include "Gfx_playername.h"
#include "Tmainform.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

// Draws the capitalised name onto a label surface sized to the text, in the colour that color_index picks.
PlayerNameGraphics::PlayerNameGraphics(int label_width, unsigned int label_height, int color_index, String name)
{
    HDC dc;
    text_width = label_width;
    text_height = label_height;
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    surface_rect.left = 0;
    surface_rect.top = 0;
    surface_rect.right = label_width + 8;
    surface_rect.bottom = label_height;
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = label_width + 8;
    surface_desc->Height = label_height;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface->Blt(&surface_rect, 0, 0, 16778240, fill_fx);
    if (surface->GetDC(&dc))
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
            MainForm->draw_canvas->TextOut(1, 0, Client::Capitalize(MainForm->net_client, name));
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        surface->ReleaseDC(dc);
    }
    surface->SetColorKey(8, color_key);
}

PlayerNameGraphics::~PlayerNameGraphics()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    fill_fx->Clear();
    delete color_key;
    delete fill_fx;
}
