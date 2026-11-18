#ifndef Gfx_npcH
#define Gfx_npcH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

// The two sheets of one npc graphic, front view and back view: a row of equal frames each.
struct NpcResources
{
    TDx_DrawSurface *front_sheet;
    TDx_DrawSurface *back_sheet;
    TDDBltFX *fill_fx;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    int frame_width;
    int frame_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    bool sheets_created;

    NpcResources();
    ~NpcResources();
    bool CreateSheets(void *library, int resource_id, char has_idle_animation);
    int GetFrameWidth();
    int GetFrameHeight();
    bool LoadFrontFrame(int frame, void *library, int resource_id);
    bool LoadBackFrame(int frame, void *library, int resource_id);
    TRect GetFrameRect(int frame);
};

#endif
