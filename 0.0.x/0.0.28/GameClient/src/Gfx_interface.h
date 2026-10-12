#ifndef Gfx_interfaceH
#define Gfx_interfaceH

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
    static Windows::TRect GetChatTabRect(InterfaceGraphics *self, int tab);
    static Windows::TRect GetStatusBarFillRect(InterfaceGraphics *self, int bar, int segments);
    static Windows::TRect GetListIconRect(InterfaceGraphics *self, int row, int column);
    static Windows::TRect GetTabButtonRect(InterfaceGraphics *self, int tab, int state);
    static Windows::TRect GetSettingMarkerRect(InterfaceGraphics *self, int state);
    static Windows::TRect GetRaiseButtonRect(InterfaceGraphics *self, int state);
    static Windows::TRect GetScrollIconRect(InterfaceGraphics *self, int icon);
    static Windows::TRect GetChatModeIconRect(InterfaceGraphics *self, int mode);
    static Windows::TRect GetSmallIconRect(InterfaceGraphics *self, int icon);
    static Windows::TRect GetCornerButtonRect(InterfaceGraphics *self, int state);
    static Windows::TRect GetListModeIconRect(InterfaceGraphics *self, int mode);
    static Windows::TRect GetGroupIconRect(InterfaceGraphics *self, int icon);
    static Windows::TRect GetSpellProgressRect(InterfaceGraphics *self, int progress);
    static Windows::TRect GetSlotHighlightRect(InterfaceGraphics *self, int slot);
    static Windows::TRect GetSpellSlotLabelRect(InterfaceGraphics *self, int slot);
    static Windows::TRect GetSpellSlotFrameRect(InterfaceGraphics *self, int slot);
    static Windows::TRect GetSkillSlotFrameRect(InterfaceGraphics *self, int slot);
    static Windows::TRect GetInventoryButtonRect(InterfaceGraphics *self, int button, int state);
    static Windows::TRect GetSmallTabButtonRect(InterfaceGraphics *self, int tab, int state);
    static Windows::TRect GetInventoryWideButtonRect(InterfaceGraphics *self, int button);
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
};

Windows::TRect Gfx_interface_GetDamageDigitRect(int interface_graphics, int style, int digit);
Windows::TRect Gfx_interface_GetHealthBarRect(int interface_graphics, int color, int length);
Windows::TRect Gfx_interface_GetMinimapEdgeRect(InterfaceGraphics *self, int edge);
Windows::TRect Gfx_interface_GetWeaponEffectRect(InterfaceGraphics *self, int effect, int direction);
Windows::TRect Gfx_interface_GetTooltipRect(InterfaceGraphics *self, int width, int height);
Windows::TRect Gfx_interface_GetStatusIconRect(InterfaceGraphics *self, int icon, bool small_icon);
Windows::TRect Gfx_interface_GetTileCursorRect(InterfaceGraphics *self, int frame);
Windows::TRect Gfx_interface_GetEmoteFrameRect(InterfaceGraphics *self, int frame);

// Shared body: a class defines its GetCornerButtonRect with DEFINE_GET_CORNER_BUTTON_RECT(TheClass).
#define DEFINE_GET_CORNER_BUTTON_RECT(Class) \
Windows::TRect Class::GetCornerButtonRect(Class *self, int state) \
{ \
    TRect corner_button_rect; \
    corner_button_rect.left = 0; \
    corner_button_rect.right = 50; \
    corner_button_rect.top = CORNER_BUTTON_HEIGHT * state; \
    corner_button_rect.bottom = 53 * state + 53; \
    return corner_button_rect; \
}

#endif
