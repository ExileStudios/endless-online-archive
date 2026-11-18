#include <vcl.h>
#pragma hdrstop

#include "Gfx_effect.h"
#include "Tmainform.h"
#include "Gfx_bank.h"

#pragma package(smart_init)

EffectResource::EffectResource()
{
    layer0_surface = new TDx_DrawSurface(NULL);
    layer1_surface = new TDx_DrawSurface(NULL);
    layer2_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

EffectResource::~EffectResource()
{
    layer0_surface->Destroy();
    layer1_surface->Destroy();
    layer2_surface->Destroy();
    delete layer0_surface;
    delete layer1_surface;
    delete layer2_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// One frame of an effect: the sheet cut into the effect's frame count, or the whole of a narrow sheet.
TRect EffectResource::GetFrameRect(int frame)
{
    TRect frame_rect;
    EffectResource *this_copy = this;
    EffectResource *second_this_copy = this;
    int right = sheet_width;
    if (right > 80)
    {
        right = right / frame_count;
        frame_rect.left = right * frame;
        frame_rect.top = 0;
        frame_rect.right = right + right * frame;
        frame_rect.bottom = sheet_height;
    }
    else
    {
        frame_rect.left = 0;
        frame_rect.top = 0;
        frame_rect.right = right;
        frame_rect.bottom = sheet_height;
    }
    for (;;)
        return frame_rect;
}

// Loads the effect's three layer pictures into surfaces from the library, keyed for transparency, and returns their
// size; a failed draw is counted.
TPoint EffectResource::LoadSurfaces(int library, int base_resource_id)
{
    int effect_index;
    TPoint size;
    int layer = 0;
    // Each effect owns three resource ids in a row, one per layer.
    effect_index = (base_resource_id - 101) / 3;
    // The frame count comes from the effect table.
    frame_count = MainForm->magic_mapper->GetFrameCount(effect_index);
    size.x = -1;
    size.y = -1;
    do
    {
        if ((!layer && MainForm->magic_mapper->base_layer[effect_index]) ||
            (layer == 1 && MainForm->magic_mapper->blend_layer[effect_index]) ||
            (layer == 2 && MainForm->magic_mapper->solid_layer[effect_index]))
        {
            if (!MainForm->gfx_transfers->LoadResourceBitmap(library, base_resource_id + layer))
                return TPoint(-1, -1);
            size = *(TPoint *)MainForm->gfx_transfers;
            TDx_DrawSurface *surface = layer0_surface;
            if (layer == 1)
                surface = layer1_surface;
            if (layer == 2)
                surface = layer2_surface;
            surface_desc->SCaps = surface_caps;
            MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y);
            sheet_left = 0;
            sheet_top = 0;
            sheet_width = size.x;
            sheet_height = size.y;
            if (MainForm->gfx_transfers->BeginDraw(surface))
            {
                MainForm->gfx_transfers->Draw(surface);
                MainForm->gfx_transfers->EndDraw(surface);
                surface->SetColorKey(DDCKEY_SRCBLT, color_key);
            }
            else
                MainForm->CountDrawError();
        }
        layer++;
    } while (layer < 3);
    if (size.x > 0)
        size.x = size.x / frame_count;
    return size;
}
