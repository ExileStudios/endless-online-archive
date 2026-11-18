#include <vcl.h>
#pragma hdrstop

#include "Gfx_npc.h"
#include "Tmainform.h"

#pragma package(smart_init)

// Creates the front and back sheets of an npc graphic and their mirrored copies.
NpcResource::NpcResource()
{
    front_sheet = new TDx_DrawSurface(NULL);
    back_sheet = new TDx_DrawSurface(NULL);
    front_mirrored = new TDx_DrawSurface(NULL);
    back_mirrored = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    mirror_fx = new TDDBltFX(NULL);
    mirror_fx->FX = 2;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    sheets_created = false;
    frame_width = 0;
    frame_height = 0;
    for (int i = 0; i < 8; i++)
        frame_mirrored[i] = false;
}

NpcResource::~NpcResource()
{
    front_sheet->Destroy();
    back_sheet->Destroy();
    front_mirrored->Destroy();
    back_mirrored->Destroy();
    delete front_sheet;
    delete back_sheet;
    delete front_mirrored;
    delete back_mirrored;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
    mirror_fx->Clear();
    fill_fx->Clear();
    delete mirror_fx;
    delete fill_fx;
}

// Creates the npc's four sheets of eight frames, front and back with their mirror images, sized by the first frame;
// none when they exist or the frame cannot be loaded.
TPoint NpcResource::CreateSheets(int library, int resource_id)
{
    int frame_count;
    TPoint size;
    int width;
    if (sheets_created)
        return TPoint(-1, -1);
    else if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    else
    {
        frame_count = 8;
        size = *(TPoint *)MainForm->gfx_transfers;
        frame_width = size.x;
        frame_height = size.y;
        width = size.x * frame_count;
        surface_desc->SCaps = surface_caps;
        MainForm->gfx_transfers->CreateSurface(front_sheet, surface_desc, width, size.y);
        MainForm->gfx_transfers->CreateSurface(back_sheet, surface_desc, width, size.y);
        front_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
        back_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
        MainForm->gfx_transfers->CreateSurface(front_mirrored, surface_desc, width, size.y);
        MainForm->gfx_transfers->CreateSurface(back_mirrored, surface_desc, width, size.y);
        front_mirrored->SetColorKey(DDCKEY_SRCBLT, color_key);
        back_mirrored->SetColorKey(DDCKEY_SRCBLT, color_key);
        sheet_left = 0;
        sheet_top = 0;
        sheet_width = frame_width * frame_count;
        sheet_height = frame_height;
        sheets_created = true;
    }
    return size;
}

char NpcResource::LoadFrontFrame(int frame, int library, int resource_id)
{
    LoadFrame(front_sheet, frame, library, resource_id);
}

char NpcResource::LoadBackFrame(int frame, int library, int resource_id)
{
    LoadFrame(back_sheet, frame, library, resource_id);
}

// Draws a frame from the library into its place on the sheet, black as transparent; false when it cannot.
bool NpcResource::LoadFrame(TDx_DrawSurface *sheet, int frame, int library, int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    MainForm->gfx_transfers->SetTransparentColor(clBlack);
    if (MainForm->gfx_transfers->BeginDraw(sheet))
    {
        MainForm->gfx_transfers->DrawAt(sheet, frame * frame_width, 0);
        MainForm->gfx_transfers->EndDraw(sheet);
        return true;
    }
    MainForm->CountDrawError();
    return false;
}

// Mirrors the frame of the front and back sheets into their mirrored sheets and marks it done.
void NpcResource::MirrorFrame(int frame)
{
    TRect rect;
    rect.left = frame * frame_width;
    rect.right = (frame + 1) * frame_width;
    rect.top = 0;
    rect.bottom = frame_height;
    front_mirrored->Blt(&rect, front_sheet, &rect, DDBLT_DDFX | DDBLT_WAIT, mirror_fx);
    back_mirrored->Blt(&rect, back_sheet, &rect, DDBLT_DDFX | DDBLT_WAIT, mirror_fx);
    frame_mirrored[frame] = true;
}

TRect NpcResource::GetFrameRect(int frame)
{
    TRect frame_rect;
    frame_rect.left = frame * frame_width;
    frame_rect.top = 0;
    frame_rect.right = frame_width + frame * frame_width;
    frame_rect.bottom = frame_height;
    return frame_rect;
}
