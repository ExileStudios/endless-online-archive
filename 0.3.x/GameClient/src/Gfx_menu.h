#ifndef Gfx_menuH
#define Gfx_menuH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#define MENU_ARTWORK_X 229
#define MENU_ARTWORK_Y 70
#define MENU_BUTTON_PITCH 40
#define SIDE_ARTWORK_X 42
#define SIDE_ARTWORK_Y 140

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
    TDx_DrawSurface *checkbox_sheet;
    TDx_DrawSurface *form_sheet;
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
    TDx_DrawSurface *hair_styles;
    TDx_DrawSurface *hair_surface;
    int background_id;
    int artwork_id;
    char unused_bytes[4];
    TRect background_rect;
    TRect message_box_rect;
    TRect create_character_rect;
    TRect list_box_rect;
    TRect hair_styles_rect;
    TRect up_key_rect;
    TRect down_key_rect;
    TRect left_key_rect;
    TRect right_key_rect;
    TRect lower_wide_key_rect;
    TRect upper_wide_key_rect;
    TRect small_key_rect;
    TRect message_icon_rect;
    TRect cursor_rect;
    bool email_text_set;
    unsigned char shown_gender;
    unsigned short shown_skin;
    unsigned short shown_page;
    unsigned short shown_hair_color;
    int artwork_width;
    int artwork_height;
    TDDSurfaceDesc *background_desc;
    TDDSCaps *background_caps;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *progress_fill_fx;
    TDDBltFX *clear_fx;
    String email_text;
    bool email_scrolls;
    short email_scroll_length;
    short email_scroll_tick;

    MenuGraphics();
    ~MenuGraphics();
    TRect GetKeyRect(int column, int row);
    TRect GetScrollIconRect(int icon);
    TRect GetMainMenuButtonRect(int button, int state);
    TRect GetWideButtonRect(int button, int state);
    TRect GetSmallButtonRect(int button, int state);
    TRect GetCornerButtonRect(int state);
    TRect GetArrowRect(int arrow);
    TRect GetCheckboxRect(int checked);
    bool LoadSurface(TDx_DrawSurface *surface, int library, int resource_id);
    void LoadBackground(int library, int resource_id);
    void LoadScrollIcons(int library, int resource_id);
    void LoadLabelSheet(int library, int resource_id);
    void LoadUpperPanel(int library, int resource_id);
    void LoadKeySheet(int library, int resource_id);
    void LoadLowerPanel(int library, int resource_id);
    void LoadCheckboxSheet(int library, int resource_id);
    void LoadFormSheet(int library, int resource_id);
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
    void DrawSignupFormBackground(int x, int y, int style);
    void DrawPasswordResetBackground(int x, int y, int step);
    void DrawLoginText();
    void DrawCharacterSlotText();
    void DrawListBoxText();
    void DrawMessageBoxText();
    void DrawPasswordResetText();
    void SetEmailText(String email);
    void DrawSignupCodeText();
    void DrawListBoxControls();
    void DrawMessageBoxControls();
    void DrawChangePasswordText();
    void DrawCreateCharacterControls();
    bool RenderHairStyles(int gender, int skin, int hair_color, int page);
    void DrawMainMenuButtons();
    void DrawAccountCreationButtons();
    void DrawPasswordResetControls();
    void DrawSignupCodeControls();
    void DrawKeyHighlights();
    void DrawConfirmButtons();
    void DrawCharacterSelect();
};

#endif
