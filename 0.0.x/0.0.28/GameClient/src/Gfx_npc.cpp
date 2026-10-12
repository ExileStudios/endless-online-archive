#include <vcl.h>
#pragma hdrstop

#include "Gfx_npc.h"
#include "Tmainform.h"
#include <TDx_Draw_Library_Install.H>
#pragma package(smart_init)

NpcResources::NpcResources()
{
    front_sheet = new TDx_DrawSurface(NULL);
    back_sheet = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    sheets_created = false;
    frame_width = 0;
    frame_height = 0;
}

NpcResources::~NpcResources()
{
    front_sheet->Destroy();
    back_sheet->Destroy();
    delete front_sheet;
    delete back_sheet;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    fill_fx->Clear();
    delete color_key;
    delete fill_fx;
}

// Creates both sheets once, sized from the first frame bitmap: its width becomes every frame's width.
void NpcResources::CreateSheets(int library, int resource_id, char has_idle_animation)
{
    if (!sheets_created)
    {
        int frame_count = 8;
        MainForm->resource_bitmap->Width = 0;
        MainForm->resource_bitmap->Height = 0;
        MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
        frame_width = MainForm->resource_bitmap->Width;
        frame_height = MainForm->resource_bitmap->Height;
        sheets_created = true;
        surface_desc->Width = frame_width * frame_count;
        surface_desc->Height = frame_height;
        front_sheet->GetSurfaceDesc(surface_desc);
        front_sheet->Create(surface_desc, MainForm->Dx_Draw1);
        back_sheet->GetSurfaceDesc(surface_desc);
        back_sheet->Create(surface_desc, MainForm->Dx_Draw1);
        sheet_left = 0;
        sheet_top = 0;
        sheet_width = frame_width * frame_count;
        sheet_height = frame_height;
        front_sheet->SetColorKey(8, color_key);
        back_sheet->SetColorKey(8, color_key);
    }
}

int NpcResources::GetFrameWidth()
{
    return frame_width;
}

int NpcResources::GetFrameHeight()
{
    return frame_height;
}

// Draws bitmap resource_id of the library into frame slot frame of the front sheet.
void NpcResources::LoadFrontFrame(int frame, int library, int resource_id)
{
    HDC dc;
    HDC blit_dc;
    HDC CompatibleDC;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
    // A canvas Draw keeps black see-through; BitBlt, if configured, copies the npc front frame whole.
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (front_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(frame * frame_width, 0, MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            front_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (front_sheet->GetDC(&blit_dc))
        {
            try
            {
                BitBlt(blit_dc, frame * frame_width, 0, MainForm->resource_bitmap->Width,
                    MainForm->resource_bitmap->Height, CompatibleDC, 0, 0, 13369376);
            }
            catch (Exception &)
            {
            }
            front_sheet->ReleaseDC(blit_dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// Draws bitmap resource_id of the library into frame slot frame of the back sheet.
void NpcResources::LoadBackFrame(int frame, int library, int resource_id)
{
    HDC dc;
    HDC CompatibleDC;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
    // A canvas Draw keeps black see-through; BitBlt, if configured, copies the npc back frame whole.
    if (!MainForm->config->GetUseBitBltDraw())
    {
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Draw(frame * frame_width, 0, MainForm->resource_bitmap);
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
                MainForm->CountDrawError();
            }
            back_sheet->ReleaseDC(dc);
        }
    }
    else
    {
        CompatibleDC = CreateCompatibleDC(0);
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle);
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                BitBlt(dc, frame * frame_width, 0, MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height,
                    CompatibleDC, 0, 0, 13369376);
            }
            catch (Exception &)
            {
            }
            back_sheet->ReleaseDC(dc);
        }
        DeleteDC(CompatibleDC);
    }
}

// The rect of one frame on an npc sheet.
Windows::TRect Gfx_npc_GetFrameRect(NpcResources *self, int frame)
{
    TRect frame_rect;
    frame_rect.left = frame * self->frame_width;
    frame_rect.top = 0;
    frame_rect.right = self->frame_width + frame * self->frame_width;
    frame_rect.bottom = self->frame_height;
    return frame_rect;
}
