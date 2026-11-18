#ifndef Gfx_interfaceH
#define Gfx_interfaceH

enum HealthBarFill
{
    HealthBarFill_Green = 0,
    HealthBarFill_Yellow = 1,
    HealthBarFill_Red = 2,
    HealthBarFill_Second = 4,
    HealthBarFill_Third = 5
};

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

enum StatusIcon
{
    StatusIcon_Weapon = 0,
    StatusIcon_Shield = 1,
    StatusIcon_SpellTarget = 2,
    StatusIcon_Pvp = 3,
    StatusIcon_Boss = 4,
    StatusIcon_Drain = 5
};

enum InventoryButton
{
    InventoryButton_Left = 0,
    InventoryButton_Right = 1
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

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

#define DRAG_BACKUP_WIDTH 50
#define DRAG_ICON_BACKUP_HEIGHT 50
#define INFO_NAME_MAX_WIDTH 68
#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 640
#define SLOT_FRAME_PITCH 49

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

// The game screen's interface: the HUD, the side panels (chat, inventory, spells, skills, status, player list,
// group, settings, help, news), their sheets and buttons, and the drag image of an item, spell or skill.
struct InterfaceGraphics
{
    int theme_0_colors[7];
    int theme_1_colors[7];
    TDx_DrawSurface *hud_background;
    TDx_DrawSurface *clock_background;
    TDx_DrawSurface *tooltip_background;
    TDx_DrawSurface *chat_panel_surface;
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
    TDx_DrawSurface *element_icons;
    TDx_DrawSurface *popup_icons;
    TRect hud_background_rect;
    TRect clock_rect;
    TRect tooltip_rect;
    TRect chat_input_rect;
    TRect hud_frame_rect;
    TRect hud_left_frame_rect;
    TRect hud_top_frame_rect;
    TRect panel_rect;
    TRect loading_icon_rect;
    TRect drag_rect;
    TRect book_scrollbar_rect;
    TRect status_icon_left_rect;
    TRect status_icon_right_rect;
    TRect status_bars_frame_rect;
    TRect health_bar_frame_rect;
    TRect health_bar_shadow_rect;
    TRect miss_label_rect;
    TRect bar_text_box_rect;
    TRect scroll_up_arrow_rect;
    TRect scroll_down_arrow_rect;
    unsigned char minimap_visible;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    int last_chat_mode;
    TDDColorKey *color_key;
    int drag_width;
    int drag_height;

    InterfaceGraphics();
    ~InterfaceGraphics();
    int GetThemeColor(int theme, int shade);
    TRect GetChatTabRect(int tab);
    TRect GetDamageDigitRect(int style, int digit);
    TRect GetHealthBarFillRect(int color, int length);
    TRect GetStatusBarFillRect(int bar, int segments);
    TRect GetListIconRect(int row, int column);
    TRect GetTabButtonRect(int tab, int state);
    TRect GetSettingMarkerRect(int state);
    TRect GetRaiseButtonRect(int state);
    TRect GetScrollIconRect(int icon);
    TRect GetChatModeIconRect(int mode);
    TRect GetSmallIconRect(int icon);
    TRect GetCornerButtonRect(int tab);
    TRect GetMinimapTileRect(int row, int edge);
    TRect GetListIconInnerRect(int row, int column);
    TRect GetListIconMarkRect(int row, int column);
    TRect GetMinimapMarkRect(int row, int column);
    TRect GetStatusIconLowRect(int row, int column);
    TRect GetMarkerRect(int unused, int index);
    TRect GetElementIconRect(int element);
    TRect GetWeaknessIconRect(int weakness);
    TRect GetPopupFrameRect();
    TRect GetBarRowRect(int row);
    TRect GetPercentBarRect(int percent, int row);
    TRect GetCellRect(int effect, int direction);
    TRect GetListModeIconRect(int mode);
    TRect GetBackRect(int width, int height);
    TRect GetStatusIconRect(int icon, bool small_icon);
    TRect GetWideCellRect(int frame);
    TRect GetGroupIconRect(int icon);
    TRect GetSpellProgressRect(int progress);
    TRect GetSlotHighlightRect(int slot);
    TRect GetSpellSlotLabelRect(int slot);
    TRect GetSpellSlotFrameRect(int slot);
    TRect GetSkillSlotFrameRect(int slot);
    TRect GetInventoryButtonRect(int button, int state);
    TRect GetSmallTabButtonRect(int state, int tab);
    TRect GetInventoryWideButtonRect(ButtonState state);
    void SaveScreenshot();
    void LoadSurface(TDx_DrawSurface *surface, int library, int resource_id);
    bool LoadResourceSurface(TDx_DrawSurface *emote_sheet, int library, int resource_id, int fallback_width,
    int fallback_height);
    void LoadEmoteSheet(int library, int resource_id);
    void LoadWeaponEffects(int library, int resource_id);
    void LoadPanelBackground(int library, int resource_id);
    void LoadMapLoadingBox(int library, int resource_id);
    void LoadClockBackground(int library, int resource_id);
    void LoadGroupIcons(int library, int resource_id);
    void LoadTooltipBackground(int library, int resource_id);
    void ClearChatPanel();
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
    void LoadElementIcons(int library, int resource_id);
    void LoadPopupIcons(int library, int resource_id);
    TRect GetWideIconRect(int frame);
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
    void DrawMinimap();
    void DrawFullMinimap();
    void DrawStatusBars();
    void DrawStatusBarText();
    void RestoreItemDragBackground();
    void RestoreSpellDragBackground();
    void RestoreSkillDragBackground();
};


// Shared body: a class defines its GetCornerButtonRect with DEFINE_GET_CORNER_BUTTON_RECT(TheClass).
#define DEFINE_GET_CORNER_BUTTON_RECT(Class) \
TRect Class::GetCornerButtonRect(int state) \
{ \
    TRect corner_button_rect; \
    corner_button_rect.left = 0; \
    corner_button_rect.right = 50; \
    corner_button_rect.top = 53 * state; \
    corner_button_rect.bottom = 53 * state + 53; \
    return corner_button_rect; \
}

#endif
