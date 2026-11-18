#include <vcl.h>
#pragma hdrstop

#include "Gfx_npc.h"
#include "Tmainform.h"

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
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
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

bool NpcResources::CreateSheets(void *library, int resource_id, char has_idle_animation)
{
    if (!sheets_created)
    {
        int frame_count = 8;
        MainForm->resource_bitmap->Width = 0;
        MainForm->resource_bitmap->Height = 0;
        try
        {
            MainForm->resource_bitmap->LoadFromResourceID((unsigned int)library, resource_id);
        }
        catch (...)
        {
            return false;
        }
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
        front_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
        back_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
    }
    return true;
}

int NpcResources::GetFrameWidth()
{
    return frame_width;
}

int NpcResources::GetFrameHeight()
{
    return frame_height;
}

bool NpcResources::LoadFrontFrame(int frame, void *library, int resource_id)
{
    HDC dc;
    HDC blit_dc;
    HDC compatible_dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    try
    {
        MainForm->resource_bitmap->LoadFromResourceID((unsigned int)library, resource_id);
    }
    catch (...)
    {
        return false;
    }
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
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
        compatible_dc = CreateCompatibleDC(0);
        SelectObject(compatible_dc, MainForm->resource_bitmap->Handle);
        if (front_sheet->GetDC(&blit_dc))
        {
            try
            {
                BitBlt(blit_dc, frame * frame_width, 0, MainForm->resource_bitmap->Width,
                    MainForm->resource_bitmap->Height, compatible_dc, 0, 0, SRCCOPY);
            }
            catch (Exception &)
            {
            }
            front_sheet->ReleaseDC(blit_dc);
        }
        DeleteDC(compatible_dc);
    }
    return true;
}

bool NpcResources::LoadBackFrame(int frame, void *library, int resource_id)
{
    HDC dc;
    HDC compatible_dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    try
    {
        MainForm->resource_bitmap->LoadFromResourceID((unsigned int)library, resource_id);
    }
    catch (...)
    {
        return false;
    }
    MainForm->resource_bitmap->TransparentColor = clBlack;
    MainForm->resource_bitmap->Transparent = true;
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
        compatible_dc = CreateCompatibleDC(0);
        SelectObject(compatible_dc, MainForm->resource_bitmap->Handle);
        if (back_sheet->GetDC(&dc))
        {
            try
            {
                BitBlt(dc, frame * frame_width, 0, MainForm->resource_bitmap->Width, MainForm->resource_bitmap->Height,
                    compatible_dc, 0, 0, SRCCOPY);
            }
            catch (Exception &)
            {
            }
            back_sheet->ReleaseDC(dc);
        }
        DeleteDC(compatible_dc);
    }
    return true;
}

// The rect of one frame on an npc sheet.
TRect NpcResources::GetFrameRect(int frame)
{
    TRect frame_rect;
    frame_rect.left = frame * frame_width;
    frame_rect.top = 0;
    frame_rect.right = frame_width + frame * frame_width;
    frame_rect.bottom = frame_height;
    return frame_rect;
}
