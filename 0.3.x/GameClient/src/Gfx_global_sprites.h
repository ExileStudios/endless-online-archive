#ifndef Gfx_global_spritesH
#define Gfx_global_spritesH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct EmoteBuffer;

#include "Sys_player.h"

#define HIDDEN_VIEW_ADMIN_LEVEL 3

struct SpriteGlobalGraphics
{
    TDx_DrawSurface *stand_sheet;
    TDx_DrawSurface *walk_sheet;
    TDx_DrawSurface *attack_sheet;
    TDx_DrawSurface *spell_cast_sheet;
    TDx_DrawSurface *chair_sit_sheet;
    TDx_DrawSurface *floor_sit_sheet;
    TDx_DrawSurface *ranged_attack_sheet;
    TDx_DrawSurface *face_sheet;
    TDx_DrawSurface *translucent_buffer;
    TDx_DrawSurface *kind_sheet;
    EmoteBuffer *emote_buffer;
    bool emote_buffer_created;
    int view_left;
    int view_top;
    int view_right;
    int view_bottom;
    TDDBltFX *blt_fx;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    int clip_left;
    int clip_top;
    int clip_right;
    int clip_bottom;
    char is_mirrored;
    char use_frame_buffer;
    bool emote_pass;
    bool emote_lowered;
    int direction;
    int draw_gender;
    int draw_progress;
    TRect player_rect;
    TRect emote_rect;

    SpriteGlobalGraphics();
    ~SpriteGlobalGraphics();
    TRect GetStandFrameRect(int gender, int facing, int skin);
    TRect GetWalkFrameRect(int gender, int facing, int skin, int frame);
    TRect GetAttackFrameRect(int gender, int facing, int skin, int frame);
    TRect GetSpellCastCellRect(int gender, int facing, int skin);
    TRect GetChairSitFrameRect(int gender, int facing, int skin);
    TRect GetFloorSitFrameRect(int gender, int facing, int skin);
    TRect GetRangedAttackFrameRect(int gender, int facing, int skin);
    TRect GetFaceStripRect(int gender, int skin);
    TRect GetKindFrameRect(int unused, int column, int kind);
    TRect GetHeadRect(int gender, int skin);
    TRect GetHeadAndNeckRect(int gender, int skin);
    void LoadStandSheet(int library, int resource_id);
    void LoadWalkSheet(int library, int resource_id);
    void LoadAttackSheet(int library, int resource_id);
    void LoadSpellCastSheet(int library, int resource_id);
    void LoadChairSitSheet(int library, int resource_id);
    void LoadFloorSitSheet(int library, int resource_id);
    void LoadRangedAttackSheet(int library, int resource_id);
    void LoadFaceSheet(int library, int resource_id);
    void LoadKindSheet(int library, int resource_id);
    bool LoadSheet(TDx_DrawSurface *sheet, int library, int resource_id);
    bool DrawLayerToEmoteBuffer(int x_offset, int y_offset, int library, int resource_id);
    bool DrawLayerToEmoteBufferPixelKeyed(int x_offset, int y_offset, int library, int resource_id);
    void DrawPlayerAt(Player player, int hair_style, int hair_color, int x, int y);
    void DrawPlayer(Player *player, char draw_buffered, int x_offset);
    void DrawPreviewPlayers(int preview);
    void BlitFrame(TRect dest_rect, TDx_DrawSurface *sheet, TRect source_rect, bool clip,
    bool translucent, Player *player);
    void DrawTranslucentBuffer(TRect dest_rect);
    void DrawNameLabel();
    TRect ComputeClip(TRect dest_rect);
    TRect ApplyClip(TRect source_rect);
    void ShowNameLabel(String text, int color_index, int element);
    void CreateEmoteBuffer();
    void FreeEmoteBuffer();
    bool DrawHealthBar(int x, int y, Player *player);
};

#endif
