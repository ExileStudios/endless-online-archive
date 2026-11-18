#ifndef Ctrl_bossbarH
#define Ctrl_bossbarH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct BossBar
{
    TDx_DrawSurface *name_surface;
    TRect name_rect;
    bool needs_render;
    bool is_rendered;
    bool is_visible;
    int blink_frame;
    int rank_icons;
    String unused_text;
    int npc_index;
    int npc_id;
    int name_width;
    int health_percent;
    int health_color;
    bool has_health;
    bool is_blinking;
    int blink_counter;
    int damage_total;
    int shown_time;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *blt_fx;

    BossBar();
    ~BossBar();
    void Show(int new_npc_index, int new_npc_id);
    void Hide();
    void RenderName();
    TRect GetRankIconRect(int frame);
    void Update();
    bool IsVisible();
    void Draw(int x, int y, int offset_y);
};

#endif
