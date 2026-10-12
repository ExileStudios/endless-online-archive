#ifndef Gfx_npcsH
#define Gfx_npcsH

class TDDBltFX;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

struct Npc;

// Draws the npcs in the map view: the frame of each npc's action, its health bar, damage number and balloon text.
struct NpcGraphics
{
    int view_left;
    int view_top;
    int view_right;
    int view_bottom;
    TDDBltFX *blt_fx;
    TDx_DrawSurface *translucent_buffer;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    int clip_left;
    int clip_top;
    int clip_right;
    int clip_bottom;
    char is_mirrored;
    bool is_translucent;
    int alpha;
    int buffer_width;
    int buffer_height;
    char pad_0x40[8];
    Windows::TRect dest_rect;

    NpcGraphics();
    ~NpcGraphics();
    void DrawNpc(Npc npc, int draw_alpha, int x_offset);
    void BlitFrame(Windows::TRect screen_rect, TDx_DrawSurface *sheet, Windows::TRect source_rect, bool keyed);
    void DrawTranslucentBuffer(Windows::TRect screen_rect);
    static Windows::TRect ClipToView(NpcGraphics *self, Windows::TRect rect);
    static Windows::TRect GetClippedSourceRect(NpcGraphics *self, Windows::TRect rect);
    void ShowSpeechBalloon(String text, int npc_index, int style);
};

#endif
