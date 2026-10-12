#ifndef Gfx_spritesH
#define Gfx_spritesH

#define HIDDEN_VIEW_ADMIN_LEVEL 3
class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

struct EmoteBuffer;
struct Player;

// The player body sheets (stand, walk, attack, spell cast, chair sit, floor sit, ranged attack, face) and the code
// that draws players on the map with their emote face, health bar and damage number, and their labels and balloons.
struct SpriteGraphics
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
    int height_offset;
    int player_load_stage;
    Windows::TRect frame_rect;
    Windows::TRect emote_rect;

    SpriteGraphics();
    ~SpriteGraphics();
    void LoadStandSheet(int library, int resource_id);
    void LoadWalkSheet(int library, int resource_id);
    void LoadAttackSheet(int library, int resource_id);
    void LoadSpellCastSheet(int library, int resource_id);
    void LoadChairSitSheet(int library, int resource_id);
    void LoadFloorSitSheet(int library, int resource_id);
    void LoadRangedAttackSheet(int library, int resource_id);
    void LoadFaceSheet(int library, int resource_id);
    void DrawPlayerAt(Player player, int hair_style, int hair_color, int x, int y);
    void DrawPlayer(Player player, int draw_buffered, int x_offset);
    void DrawPreviewPlayers(int preview);
    void BlitFrame(Windows::TRect dest_rect, TDx_DrawSurface *sheet, Windows::TRect source_rect, bool clip, bool keyed);
    void DrawTranslucentBuffer(Windows::TRect dest_rect);
    void DrawNameLabel();
    static Windows::TRect ClipToView(SpriteGraphics *self, Windows::TRect rect);
    static Windows::TRect GetClippedSourceRect(SpriteGraphics *self, Windows::TRect rect);
    void ShowNameLabel(String text, int color_index);
    void ShowSpeechBalloon(String text, int player_id, int style);
    void CreateEmoteBuffer();
    void FreeEmoteBuffer();
    void DrawLayerToEmoteBuffer(int x_offset, int y_offset, int library, int resource_id);
    void DrawLayerToEmoteBufferPixelKeyed(int x_offset, int y_offset, int library, int resource_id);
};

Windows::TRect Gfx_sprites_GetStandFrameRect(SpriteGraphics *self, int gender, int direction, int skin);
Windows::TRect Gfx_sprites_GetWalkFrameRect(SpriteGraphics *self, int gender, int direction, int skin, int frame);
Windows::TRect Gfx_sprites_GetAttackFrameRect(SpriteGraphics *self, int gender, int direction, int skin, int frame);
Windows::TRect Gfx_sprites_GetSpellCastCellRect(SpriteGraphics *self, int gender, int direction, int skin);
Windows::TRect Gfx_sprites_GetChairSitFrameRect(SpriteGraphics *self, int gender, int direction, int skin);
Windows::TRect Gfx_sprites_GetFloorSitFrameRect(SpriteGraphics *self, int gender, int direction, int skin);
Windows::TRect Gfx_sprites_GetRangedAttackFrameRect(SpriteGraphics *self, int gender, int direction, int skin);
Windows::TRect Gfx_sprites_GetFaceStripRect(SpriteGraphics *self, int gender, int skin);

#endif
