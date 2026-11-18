#include <vcl.h>
#pragma hdrstop

#include "Gfx_intro.h"
#include "Tmainform.h"

#pragma package(smart_init)

IntroGraphics::IntroGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0xFFFFFF;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
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

// Loads the library's bitmap onto the intro surface.
void IntroGraphics::LoadSurface(int library, int resource_id)
{
    if (MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id) &&
        MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
    }
}
