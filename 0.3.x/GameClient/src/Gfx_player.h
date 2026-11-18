#ifndef Gfx_playerH
#define Gfx_playerH

enum PlayerFrame
{
    PlayerFrame_Stand = 0,
    PlayerFrame_Walk1 = 1,
    PlayerFrame_Walk2 = 2,
    PlayerFrame_Walk3 = 3,
    PlayerFrame_Walk4 = 4,
    PlayerFrame_Attack1 = 5,
    PlayerFrame_Attack2 = 6,
    PlayerFrame_SpellCast = 7,
    PlayerFrame_ChairSit = 8,
    PlayerFrame_FloorSit = 9,
    PlayerFrame_RangedAttack = 10
};

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#define PLAYER_FRAME_WIDTH 68

// The surfaces of one player: front and back sheets of eleven frames, the emote face strip and the name tag.
struct PlayerResources
{
    TDx_DrawSurface *front_sheet;
    TDx_DrawSurface *back_sheet;
    TDx_DrawSurface *emote_strip;
    TDx_DrawSurface *name_tag_surface;
    TDDBltFX *fill_fx;
    TRect sheet_rect;
    TRect emote_strip_rect;
    TRect name_tag_text_rect;
    TRect name_tag_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    PlayerResources();
    ~PlayerResources();
    void ClearSheets();
    TPoint DrawFrontLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    TPoint DrawBackLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    TPoint DrawFrontLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    TPoint DrawBackLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    char DrawFrontLayerFrom(int frame_slot, int x_offset, int y_offset, TDx_DrawSurface *source,
    TRect source_rect);
    char DrawBackLayerFrom(int frame_slot, int x_offset, int y_offset, TDx_DrawSurface *source, TRect source_rect);
    void RenderNameTag(String text, int color_index);
    TRect GetFrameRect(int frame);
    TRect GetEmoteFaceRect(int emote_index);
};

#endif
