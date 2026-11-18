#include <vcl.h>
#pragma hdrstop

#include "Ctrl_popupelement.h"
#include "Tmainform.h"
#include "Gfx_interface.h"
#include <time.h>

#pragma package(smart_init)

// A hidden popup and the surface it is drawn on.
PopupElement::PopupElement()
{
    is_visible = false;
    needs_render = false;
    is_rendered = false;
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    blt_fx = new TDDBltFX(NULL);
    blt_fx->FillColor = 0;
    rect.left = 0;
    rect.top = 0;
    rect.right = 184;
    rect.bottom = 38;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 184;
    surface_desc->Height = 38;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface->Blt(&rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, blt_fx);
    surface->SetColorKey(DDCKEY_SRCBLT, color_key);
}

PopupElement::~PopupElement()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    blt_fx->Clear();
    delete color_key;
    delete blt_fx;
}

// Shows the text and weakness icon at the position, timed from now.
void PopupElement::Show(int new_x, int new_y, int new_weakness, String new_text)
{
    needs_render = true;
    is_rendered = false;
    is_visible = true;
    x = new_x;
    y = new_y;
    text = new_text;
    weakness = new_weakness;
    unused = 0;
    shown_at = clock();
}

// Draws the element popup once: its frame, whether a weakness was found, and its text below, each centred.
void PopupElement::Render()
{
    int text_half_width;
    HDC dc;
    if (needs_render)
    {
        surface->BltFast(0, 0, MainForm->gfx->interface_graphics->element_icons,
            &MainForm->gfx->interface_graphics->GetPopupFrameRect(), DDBLTFAST_WAIT);
        int center_x = 100;
        if (surface->GetDC(&dc))
        {
            try
            {
                TCanvas *canvas = new TCanvas();
                canvas->Handle = dc;
                canvas->Brush->Style = bsClear;
                canvas->Font->Color = clWhite;
                canvas->Font->Name = "Arial";
                canvas->Font->Size = 8;
                String title = "no weakness found";
                if (weakness > 0)
                    title = "weakness discovered !";
                int title_half_width = canvas->TextWidth(title) / 2;
                canvas->TextOut(center_x - title_half_width, 6, title);
                canvas->Font->Color = (TColor)0xBBC4C4;
                text_half_width = canvas->TextWidth(text) / 2;
                canvas->TextOut(center_x - text_half_width, 17, text);
                canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            surface->ReleaseDC(dc);
        }
        MainForm->redraw_needed = true;
        needs_render = false;
        is_rendered = true;
    }
}

// Hides the popup 1.5 seconds after it was shown.
void PopupElement::Tick()
{
    if (is_visible && is_rendered && clock() - shown_at > 1500)
    {
        MainForm->redraw_needed = false;
        is_visible = false;
    }
}

// Draws the popup and its weakness icon, capping the weakness at 6.
void PopupElement::Draw()
{
    if (is_visible && is_rendered)
    {
        if (weakness > 6)
            weakness = 6;
        MainForm->OffscreenSurface->BltFast(x, y, surface, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(x + 16, y + 12, MainForm->gfx->interface_graphics->element_icons,
            &MainForm->gfx->interface_graphics->GetWeaknessIconRect(weakness), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}
