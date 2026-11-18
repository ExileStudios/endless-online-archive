#include <vcl.h>
#pragma hdrstop

#include "Gfx_playername.h"
#include "Tmainform.h"
#include "Gfx_interface.h"

#pragma package(smart_init)

// Draws the capitalised name onto a label surface sized to the text, in the colour that color_index picks, with room
// for an element icon when element is set.
PlayerNameGraphics::PlayerNameGraphics(int label_width, int label_height, int color_index, int element, String name)
{
    int name_width;
    HDC dc;
    if (element > 0)
        label_width += 7;
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
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = label_width + 8;
    surface_desc->Height = label_height;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface->Blt(&surface_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
    name_width = 0;
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
            if (color_index == LabelColor_Gray)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xB9B9B9;
                MainForm->draw_canvas->Font->Color = (TColor)0xB9B9B9;
            }
            if (color_index == LabelColor_Silver)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xE1E1E1;
                MainForm->draw_canvas->Font->Color = (TColor)0xE1E1E1;
            }
            if (color_index == LabelColor_LightBlue)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xE7B790;
                MainForm->draw_canvas->Font->Color = (TColor)0xE7B790;
            }
            if (color_index == LabelColor_Violet)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)14327000;
                MainForm->draw_canvas->Font->Color = (TColor)14327000;
            }
            if (color_index == LabelColor_Gold)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0x5BC8FB;
                MainForm->draw_canvas->Font->Color = (TColor)0x5BC8FB;
            }
            if (color_index == LabelColor_Cream)
            {
                MainForm->draw_canvas->Pen->Color = (TColor)0xD2FAFA;
                MainForm->draw_canvas->Font->Color = (TColor)0xD2FAFA;
            }
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            MainForm->draw_canvas->TextOut(1, 0, MainForm->net_client->Capitalize(name));
            name_width = MainForm->draw_canvas->TextWidth(MainForm->net_client->Capitalize(name));
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        surface->ReleaseDC(dc);
    }
    if (element > 0 && element < 7)
        surface->BltFast(name_width + 4, 2, MainForm->gfx->interface_graphics->element_icons,
            &MainForm->gfx->interface_graphics->GetElementIconRect(element), DDBLTFAST_WAIT);
    surface->SetColorKey(DDCKEY_SRCBLT, color_key);
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
