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
    int name_width = 0;
    if (MainForm->fonts->BeginOn(surface))
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
                MainForm->fonts->color = 0xE7B790;
            if (color_index == LabelColor_Violet)
                MainForm->fonts->color = 14327000;
            if (color_index == LabelColor_Gold)
                MainForm->fonts->color = 0x5BC8FB;
            if (color_index == LabelColor_Cream)
                MainForm->fonts->color = 0xD2FAFA;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            MainForm->fonts->DrawString(1, 0, MainForm->net_client->Capitalize(name));
            name_width = (short)MainForm->fonts->TextWidth(MainForm->net_client->Capitalize(name));
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
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
