#ifndef Gfx_npcH
#define Gfx_npcH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct NpcResource
{
    TDx_DrawSurface *front_sheet;
    TDx_DrawSurface *back_sheet;
    TDx_DrawSurface *front_mirrored;
    TDx_DrawSurface *back_mirrored;
    TDDBltFX *fill_fx;
    TDDBltFX *mirror_fx;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    int frame_width;
    int frame_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    char frame_mirrored[8];
    bool sheets_created;

    NpcResource();
    ~NpcResource();
    TPoint CreateSheets(int library, int resource_id);
    char LoadFrontFrame(int frame, int library, int resource_id);
    char LoadBackFrame(int frame, int library, int resource_id);
    bool LoadFrame(TDx_DrawSurface *sheet, int frame, int library, int resource_id);
    void MirrorFrame(int frame);
    TRect GetFrameRect(int frame);
};

#endif
