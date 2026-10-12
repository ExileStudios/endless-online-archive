#include <vcl.h>
#pragma hdrstop

#include "Gfx_intro.h"
#include "Tmainform.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

IntroGraphics::IntroGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 16777215;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = 358;
    surface_desc->Height = 187;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
}

IntroGraphics::~IntroGraphics()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    fill_fx->Clear();
    delete fill_fx;
}

// Draws bitmap resource_id of the library onto the intro surface, opaque.
void IntroGraphics::LoadSurface(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        surface->ReleaseDC(dc);
    }
}
