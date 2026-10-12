#include <vcl.h>
#pragma hdrstop

#include "Gfx_effect.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include <TDx_Draw_Library_Install.H>
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
Windows::TRect Gfx_effect_GetFrameRect(EffectResource *self, int frame)
{
    TRect frame_rect;
    int right;
    EffectResource *self_copy_3 = self;
    EffectResource *self_copy_2 = self;
    EffectResource *self_copy = self;
    right = self->sheet_width;
    if (right > 80)
    {
        right = right / self->frame_count;
        frame_rect.left = right * frame;
        frame_rect.top = 0;
        frame_rect.right = right * frame + right;
        frame_rect.bottom = self->sheet_height;
    }
    else
    {
        frame_rect.left = 0;
        frame_rect.top = 0;
        frame_rect.right = right;
        frame_rect.bottom = self->sheet_height;
    }
    for (;;)
        return frame_rect;
}

// Loads the layers an effect has, from consecutive bitmap resources, and returns the size of one frame.
// The size is -1 when a layer could not be drawn onto its surface.
GraphicSize EffectResource::LoadSurfaces(int library, int base_resource_id)
{
    int layer;
    int effect_index;
    GraphicSize size;
    HDC dc;
    layer = 0;
    // Each effect owns three resource ids in a row, one per layer.
    effect_index = (base_resource_id - 101) / 3;
    // The frame count comes from the effect table.
    frame_count = MainForm->magic_mapper->GetFrameCount(effect_index);
    do
    {
        if ((!layer && MainForm->magic_mapper->base_layer[effect_index]) ||
            (layer == 1 && MainForm->magic_mapper->blend_layer[effect_index]) ||
            (layer == 2 && MainForm->magic_mapper->solid_layer[effect_index]))
        {
            size.width = -1;
            MainForm->resource_bitmap->Width = 0;
            MainForm->resource_bitmap->Height = 0;
            MainForm->resource_bitmap->LoadFromResourceID(library, base_resource_id + layer);
            surface_desc->Flags = 6;
            surface_desc->SCaps = surface_caps;
            surface_desc->SCaps->Caps = 2112;
            surface_desc->Width = MainForm->resource_bitmap->Width;
            surface_desc->Height = MainForm->resource_bitmap->Height;
            if (!layer)
            {
                if (!layer0_surface)
                    layer0_surface->Restore();
                if (layer0_surface->Created)
                    layer0_surface->Destroy();
                layer0_surface->GetSurfaceDesc(surface_desc);
                layer0_surface->Create(surface_desc, MainForm->Dx_Draw1);
            }
            if (layer == 1)
            {
                if (!layer1_surface)
                    layer1_surface->Restore();
                if (layer1_surface->Created)
                    layer1_surface->Destroy();
                layer1_surface->GetSurfaceDesc(surface_desc);
                layer1_surface->Create(surface_desc, MainForm->Dx_Draw1);
            }
            if (layer == 2)
            {
                if (!layer2_surface)
                    layer2_surface->Restore();
                if (layer2_surface->Created)
                    layer2_surface->Destroy();
                layer2_surface->GetSurfaceDesc(surface_desc);
                layer2_surface->Create(surface_desc, MainForm->Dx_Draw1);
            }
            size.width = MainForm->resource_bitmap->Width / frame_count;
            size.height = MainForm->resource_bitmap->Height;
            sheet_left = 0;
            sheet_top = 0;
            sheet_width = MainForm->resource_bitmap->Width;
            sheet_height = MainForm->resource_bitmap->Height;
            if (!MainForm->config->GetUseBitBltDraw())
            {
                if (!layer && layer0_surface->GetDC(&dc))
                {
                    try
                    {
                        MainForm->draw_canvas->Handle = dc;
                        MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
                        MainForm->draw_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                        MainForm->CountDrawError();
                        layer0_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer0_surface->ReleaseDC(dc);
                }
                if (layer == 1 && layer1_surface->GetDC(&dc))
                {
                    try
                    {
                        MainForm->draw_canvas->Handle = dc;
                        MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
                        MainForm->draw_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                        MainForm->CountDrawError();
                        layer1_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer1_surface->ReleaseDC(dc);
                }
                if (layer == 2 && layer2_surface->GetDC(&dc))
                {
                    try
                    {
                        MainForm->draw_canvas->Handle = dc;
                        MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
                        MainForm->draw_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                        MainForm->CountDrawError();
                        layer2_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer2_surface->ReleaseDC(dc);
                }
            }
            else
            {
                HDC CompatibleDC = CreateCompatibleDC(0);
                SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
                if (!layer && layer0_surface->GetDC(&dc))
                {
                    try
                    {
                        BitBlt(dc, 0, 0, size.width * frame_count, size.height, CompatibleDC, 0, 0, 13369376);
                    }
                    catch (...)
                    {
                        layer0_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer0_surface->ReleaseDC(dc);
                }
                if (layer == 1 && layer1_surface->GetDC(&dc))
                {
                    try
                    {
                        BitBlt(dc, 0, 0, size.width * frame_count, size.height, CompatibleDC, 0, 0, 13369376);
                    }
                    catch (...)
                    {
                        layer1_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer1_surface->ReleaseDC(dc);
                }
                if (layer == 2 && layer2_surface->GetDC(&dc))
                {
                    try
                    {
                        BitBlt(dc, 0, 0, size.width * frame_count, size.height, CompatibleDC, 0, 0, 13369376);
                    }
                    catch (...)
                    {
                        layer2_surface->ReleaseDC(dc);
                        size.width = -1;
                        size.height = -1;
                    }
                    layer2_surface->ReleaseDC(dc);
                }
                DeleteDC(CompatibleDC);
            }
            if (size.width > 0)
            {
                if (!layer)
                    layer0_surface->SetColorKey(8, color_key);
                if (layer == 1)
                    layer1_surface->SetColorKey(8, color_key);
                if (layer == 2)
                    layer2_surface->SetColorKey(8, color_key);
            }
        }
        layer++;
    } while (layer < 3);
    return size;
}
