#ifndef Gfx_menuH
#define Gfx_menuH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The menu screens' surfaces: the backdrop, artwork, panels, buttons, the soft keypad and the boxes and forms drawn
// over them.
struct MenuGraphics
{
    TDx_DrawSurface *background;
    TDx_DrawSurface *artwork;
    TDx_DrawSurface *main_menu_buttons;
    TDx_DrawSurface *wide_buttons;
    TDx_DrawSurface *small_buttons;
    TDx_DrawSurface *lower_panel;
    TDx_DrawSurface *upper_panel;
    TDx_DrawSurface *key_sheet;
    TDx_DrawSurface *slot_frame;
    TDx_DrawSurface *label_sheet;
    TDx_DrawSurface *corner_button;
    TDx_DrawSurface *scroll_icons;
    TDx_DrawSurface *message_box_background;
    TDx_DrawSurface *create_character_background;
    TDx_DrawSurface *list_box_background;
    TDx_DrawSurface *progress_bar;
    TDx_DrawSurface *slot_icons;
    TDx_DrawSurface *animation_sheet;
    int background_id;
    int artwork_id;
    char pad_0x50[4];
    Windows::TRect background_rect;
    Windows::TRect message_box_rect;
    Windows::TRect create_character_rect;
    Windows::TRect list_box_rect;
    Windows::TRect up_key_rect;
    Windows::TRect down_key_rect;
    Windows::TRect left_key_rect;
    Windows::TRect right_key_rect;
    Windows::TRect lower_wide_key_rect;
    Windows::TRect upper_wide_key_rect;
    Windows::TRect small_key_rect;
    Windows::TRect message_icon_rect;
    int artwork_width;
    int artwork_height;
    TDDSurfaceDesc *background_desc;
    TDDSCaps *background_caps;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *progress_fill_fx;

    MenuGraphics();
    ~MenuGraphics();
    static Windows::TRect GetKeyRect(MenuGraphics *self, int column, int row);
    static Windows::TRect GetScrollIconRect(MenuGraphics *self, int icon);
    static Windows::TRect GetMainMenuButtonRect(MenuGraphics *self, int button, int state);
    static Windows::TRect GetWideButtonRect(MenuGraphics *self, int button, int state);
    static Windows::TRect GetSmallButtonRect(MenuGraphics *self, int button, int state);
    static Windows::TRect GetCornerButtonRect(MenuGraphics *self, int state);
    void LoadBackground(int library, int resource_id);
    void LoadScrollIcons(int library, int resource_id);
    void LoadLabelSheet(int library, int resource_id);
    void LoadUpperPanel(int library, int resource_id);
    void LoadKeySheet(int library, int resource_id);
    void LoadLowerPanel(int library, int resource_id);
    void LoadSlotFrame(int library, int resource_id);
    void LoadArtwork(int library, int resource_id);
    void LoadMainMenuButtons(int library, int resource_id);
    void LoadWideButtons(int library, int resource_id);
    void LoadSmallButtons(int library, int resource_id);
    void LoadCornerButton(int library, int resource_id);
    void LoadListBoxBackground(int library, int resource_id);
    void LoadMessageBoxBackground(int library, int resource_id);
    void LoadCreateCharacterBackground(int library, int resource_id);
    void LoadProgressBar(int library, int resource_id);
    void LoadSlotIcons(int library, int resource_id);
    void LoadAnimationSheet(int library, int resource_id);
    void DrawArtworkOnBackground(int x, int y);
    void DrawUpperPanelOnBackground(int x, int y);
    void DrawLowerPanelOnBackground(int x, int y);
    void DrawSlotFrameOnBackground(int x, int y);
    void DrawFormLabels(int origin_x, int origin_y);
    void DrawLoginText();
    void DrawCharacterSlotText();
    void DrawListBoxText();
    void DrawMessageBoxText();
    void DrawListBoxControls();
    void DrawMessageBoxControls();
    void DrawCreateCharacterControls();
    void DrawMainMenuButtons();
    void DrawAccountCreationButtons();
    void DrawKeyHighlights();
    void DrawConfirmButtons();
    void DrawCharacterSelect();
};

#endif
