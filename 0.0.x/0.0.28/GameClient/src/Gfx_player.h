#ifndef Gfx_playerH
#define Gfx_playerH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The surfaces of one player: front and back sheets of eleven frames, the emote face strip and the name tag.
struct PlayerResources
{
    TDx_DrawSurface *front_sheet;
    TDx_DrawSurface *back_sheet;
    TDx_DrawSurface *emote_strip;
    TDx_DrawSurface *name_tag_surface;
    TDDBltFX *fill_fx;
    Windows::TRect sheet_rect;
    Windows::TRect emote_strip_rect;
    Windows::TRect name_tag_text_rect;
    Windows::TRect name_tag_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    PlayerResources();
    ~PlayerResources();
    void ClearSheets();
    void DrawFrontLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    void DrawBackLayer(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    void DrawFrontLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    void DrawBackLayerPixelKeyed(int frame_slot, int x_offset, int y_offset, int library, int resource_id);
    void RenderNameTag(String text, int color_index);
    Windows::TRect Gfx_player_GetFrameRect(int frame);
    Windows::TRect Gfx_player_GetEmoteFaceRect(int emote_index);
};

#endif
