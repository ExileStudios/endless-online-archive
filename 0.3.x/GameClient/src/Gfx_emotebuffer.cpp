#include <vcl.h>
#pragma hdrstop

#include "Gfx_emotebuffer.h"
#include "Tmainform.h"

#pragma package(smart_init)

EmoteBuffer::EmoteBuffer()
{
    surface = new TDx_DrawSurface(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 13;
    surface_desc->Height = 14;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_rect.left = 0;
    surface_rect.top = 0;
    surface_rect.right = 13;
    surface_rect.bottom = 14;
    surface->Blt(&surface_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
    surface->SetColorKey(DDCKEY_SRCBLT, color_key);
}

EmoteBuffer::~EmoteBuffer()
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
