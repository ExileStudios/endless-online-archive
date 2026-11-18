#ifndef Gfx_npcsH
#define Gfx_npcsH

#include <TDDBltFX.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#include "Sys_npc.h"

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
    char unused_bytes[8];
    TRect dest_rect;

    NpcGraphics();
    ~NpcGraphics();
    void DrawNpc(Npc npc, int draw_alpha, int x_offset);
    void BlitFrame(TRect screen_rect, TDx_DrawSurface *sheet, TRect source_rect, bool keyed);
    void DrawTranslucentBuffer(TRect screen_rect);
    TRect ClipToView(TRect rect);
    TRect GetClippedSourceRect(TRect rect);
    void ShowSpeechBalloon(String text, int npc_index, int style);
};

#endif
