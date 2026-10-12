#ifndef Gfx_npcH
#define Gfx_npcH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

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
    void CreateSheets(int library, int resource_id, char has_idle_animation);
    int GetFrameWidth();
    int GetFrameHeight();
    void LoadFrontFrame(int frame, int library, int resource_id);
    void LoadBackFrame(int frame, int library, int resource_id);
};

Windows::TRect Gfx_npc_GetFrameRect(NpcResources *self, int frame);

#endif
