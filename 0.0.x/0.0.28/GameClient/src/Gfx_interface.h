#ifndef Gfx_interfaceH
#define Gfx_interfaceH

enum ButtonState
{
    ButtonState_Normal = 0,
    ButtonState_Hover = 1
};

enum DamageDigitStyle
{
    DamageDigitStyle_Damage = 0,
    DamageDigitStyle_Heal = 1
};

enum SmallTabButton
{
    SmallTabButton_Friends = 0,
    SmallTabButton_Ignore = 1
};

enum InventoryButton
{
    InventoryButton_Left = 0,
    InventoryButton_Right = 1
};

enum StatusIcon
{
    StatusIcon_Weapon = 0,
    StatusIcon_Shield = 1,
    StatusIcon_SpellTarget = 2,
    StatusIcon_Pvp = 3,
    StatusIcon_Boss = 4,
    StatusIcon_Drain = 5
};

enum HudButton
{
    HudButton_Inventory = 0,
    HudButton_Minimap = 1,
    HudButton_Spells = 2,
    HudButton_Skills = 3,
    HudButton_Chat = 4,
    HudButton_Status = 5,
    HudButton_Players = 6,
    HudButton_Group = 7,
    HudButton_Unused = 8,
    HudButton_Settings = 9,
    HudButton_Help = 10
};

enum PanelBackground
{
    PanelBackground_Chat = 128,
    PanelBackground_Status = 134,
    PanelBackground_PlayerList = 136,
    PanelBackground_Group = 142,
    PanelBackground_Inventory = 144,
    PanelBackground_Settings = 147,
    PanelBackground_News = 148,
    PanelBackground_Skills = 162,
    PanelBackground_Help = 163
};

#define CORNER_BUTTON_HEIGHT 53
#define HEALTH_BAR_SCALE 0.36
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The game screen's interface: the HUD, the side panels (chat, inventory, spells, skills, status, player list,
// group, settings, help, news), their sheets and buttons, and the drag image of an item, spell or skill.
struct InterfaceGraphics
{
    TDx_DrawSurface *hud_background;
    TDx_DrawSurface *clock_background;
    TDx_DrawSurface *tooltip_background;
    TDx_DrawSurface *chat_mode_icons;
    TDx_DrawSurface *chat_input_background;
    TDx_DrawSurface *small_icons;
    TDx_DrawSurface *chat_tabs;
    TDx_DrawSurface *corner_button;
    TDx_DrawSurface *minimap_tiles;
    TDx_DrawSurface *group_icons;
    TDx_DrawSurface *status_icons;
    TDx_DrawSurface *hud_sheet;
    TDx_DrawSurface *tab_buttons;
    TDx_DrawSurface *list_icons;
    TDx_DrawSurface *control_sheet;
    TDx_DrawSurface *scroll_icons;
    TDx_DrawSurface *panel_background;
    TDx_DrawSurface *loading_icon;
    TDx_DrawSurface *hud_frame;
    TDx_DrawSurface *hud_left_frame;
    TDx_DrawSurface *hud_top_frame;
    TDx_DrawSurface *tile_cursor;
    TDx_DrawSurface *emote_sheet;
    TDx_DrawSurface *weapon_effects;
    TDx_DrawSurface *drag_save_surface;
    Windows::TRect hud_background_rect;
    Windows::TRect clock_rect;
    Windows::TRect tooltip_rect;
    Windows::TRect chat_input_rect;
    Windows::TRect hud_frame_rect;
    Windows::TRect hud_left_frame_rect;
    Windows::TRect hud_top_frame_rect;
    Windows::TRect panel_rect;
    Windows::TRect loading_icon_rect;
    Windows::TRect drag_rect;
    Windows::TRect book_scrollbar_rect;
    Windows::TRect status_icon_left_rect;
    Windows::TRect status_icon_right_rect;
    Windows::TRect status_bars_frame_rect;
    Windows::TRect health_bar_frame_rect;
    Windows::TRect health_bar_shadow_rect;
    Windows::TRect miss_label_rect;
    Windows::TRect bar_text_box_rect;
    bool minimap_visible;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    int last_chat_mode;
    TDDColorKey *color_key;
    int drag_width;
    int drag_height;

    InterfaceGraphics();
    ~InterfaceGraphics();
    Windows::TRect GetChatTabRect(int tab);
    Windows::TRect GetStatusBarFillRect(int bar, int segments);
    Windows::TRect GetListIconRect(int row, int column);
    Windows::TRect GetTabButtonRect(int tab, int state);
    Windows::TRect GetSettingMarkerRect(int state);
    Windows::TRect GetRaiseButtonRect(int state);
    Windows::TRect GetScrollIconRect(int icon);
    Windows::TRect GetChatModeIconRect(int mode);
    Windows::TRect GetSmallIconRect(int icon);
    Windows::TRect GetCornerButtonRect(int state);
    Windows::TRect GetListModeIconRect(int mode);
    Windows::TRect GetGroupIconRect(int icon);
    Windows::TRect GetSpellProgressRect(int progress);
    Windows::TRect GetSlotHighlightRect(int slot);
    Windows::TRect GetSpellSlotLabelRect(int slot);
    Windows::TRect GetSpellSlotFrameRect(int slot);
    Windows::TRect GetSkillSlotFrameRect(int slot);
    Windows::TRect GetInventoryButtonRect(int button, int state);
    Windows::TRect GetSmallTabButtonRect(int state, int tab);
    Windows::TRect GetInventoryWideButtonRect(ButtonState state);
    void SaveScreenshot();
    void LoadEmoteSheet(int library, int resource_id);
    void LoadWeaponEffects(int library, int resource_id);
    void LoadPanelBackground(int library, int resource_id);
    void LoadMapLoadingBox(int library, int resource_id);
    void LoadClockBackground(int library, int resource_id);
    void LoadGroupIcons(int library, int resource_id);
    void LoadTooltipBackground(int library, int resource_id);
    void LoadChatInputBackground(int library, int resource_id);
    void LoadSmallIcons(int library, int resource_id);
    void LoadHudSheet(int library, int resource_id);
    void LoadStatusIcons(int library, int resource_id);
    void LoadChatTabs(int library, int resource_id);
    void LoadListIcons(int library, int resource_id);
    void LoadCornerButton(int library, int resource_id);
    void LoadMinimapTiles(int library, int resource_id);
    void LoadChatModeIcons(int library, int resource_id);
    void LoadHudBackground(int library, int resource_id);
    void LoadTabButtons(int library, int resource_id);
    void LoadHudFrame(int library, int resource_id);
    void LoadHudLeftFrame(int library, int resource_id);
    void LoadHudTopFrame(int library, int resource_id);
    void LoadTileCursor(int library, int resource_id);
    void LoadControlSheet(int library, int resource_id);
    void LoadScrollIcons(int library, int resource_id);
    void DrawChatModeIcon();
    void DrawCornerButton();
    void DrawPlayerListScrollButtons();
    void DrawSpellScrollButtons();
    void DrawSkillScrollButtons();
    void DrawChatScrollButtons();
    void DrawTabButtons();
    void DrawPlayerList();
    void DrawPlayerListText();
    void DrawChatPanel();
    void DrawChatText();
    void DrawStatusValues();
    void DrawGroupScrollButtons();
    void DrawNewsScrollButtons();
    void DrawHelpScrollButtons();
    void DrawGroupList();
    void DrawGroupHealthBars();
    void DrawGroupText();
    void DrawSettingMarkers();
    void DrawSettingsText();
    void DrawHelpList();
    void DrawHelpText();
    void DrawNewsList();
    void DrawNewsText();
    void DrawInventoryButtons();
    void DrawSpellInfoText();
    void DrawSkillInfoText();
    void DrawSpellPanel();
    void DrawSkillPanel();
    void DrawInventoryGrid();
    void DrawInventoryText();
    void DrawDraggedItem();
    void DrawDraggedSpell();
    void DrawDraggedSkill();
    void DrawStatusBars();
    void DrawStatusBarText();
    void RestoreItemDragBackground();
    void RestoreSpellDragBackground();
    void RestoreSkillDragBackground();
    Windows::TRect GetMinimapEdgeRect(int edge);
    Windows::TRect GetWeaponEffectRect(int effect, int direction);
    Windows::TRect GetTooltipRect(int width, int height);
    Windows::TRect GetStatusIconRect(int icon, bool small_icon);
    Windows::TRect GetTileCursorRect(int frame);
    Windows::TRect GetEmoteFrameRect(int frame);
};

Windows::TRect Gfx_interface_GetDamageDigitRect(int interface_graphics, int style, int digit);
Windows::TRect Gfx_interface_GetHealthBarRect(int interface_graphics, int color, int length);

// Shared body: a class defines its GetCornerButtonRect with DEFINE_GET_CORNER_BUTTON_RECT(TheClass).
#define DEFINE_GET_CORNER_BUTTON_RECT(Class) \
Windows::TRect Class::GetCornerButtonRect(int state) \
{ \
    TRect corner_button_rect; \
    corner_button_rect.left = 0; \
    corner_button_rect.right = 50; \
    corner_button_rect.top = CORNER_BUTTON_HEIGHT * state; \
    corner_button_rect.bottom = 53 * state + 53; \
    return corner_button_rect; \
}

#endif
