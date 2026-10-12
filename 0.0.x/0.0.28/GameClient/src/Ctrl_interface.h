#ifndef Ctrl_interfaceH
#define Ctrl_interfaceH

enum StatusCategory
{
    StatusCategory_Button = 1,
    StatusCategory_Action = 2,
    StatusCategory_Warning = 3,
    StatusCategory_Information = 4,
    StatusCategory_Item = 5,
    StatusCategory_Npc = 6,
    StatusCategory_Spell = 7,
    StatusCategory_Skill = 8
};

struct ChatController;
struct GameBars;
struct GroupController;
struct HelpController;
struct InventoryController;
struct NewslistController;
struct PlayerlistController;
struct SettingsController;
struct SkillController;
struct SpellController;
struct StatusController;

// The game screen's interface: the panel buttons, the status line, the screen shake and the panel controllers.
struct InterfaceController
{
    bool inventory_button_hover;
    bool minimap_button_hover;
    bool spells_button_hover;
    bool skills_button_hover;
    bool chat_button_hover;
    bool status_button_hover;
    bool players_button_hover;
    bool group_button_hover;
    bool third_right_button_hover;
    bool settings_button_hover;
    bool help_button_hover;
    bool friends_button_hover;
    bool ignore_button_hover;
    bool unread_flag_at_13;
    bool unread_flag_at_14;
    bool corner_button_hover;
    bool gold_drop_warning_pending;
    bool welcome_notice_pending;
    bool player_list_button_ready;
    ChatController *chat;
    StatusController *status;
    PlayerlistController *player_list;
    GroupController *group;
    SettingsController *settings;
    NewslistController *news;
    HelpController *help;
    InventoryController *inventory;
    SpellController *spells;
    SkillController *skills;
    GameBars *game_bars;
    bool is_shaking;
    int shake_offset;
    bool timed_text_visible;
    int timed_text_duration;
    String timed_text;
    int shake_duration;
    int shake_amplitude;
    int trade_requester_id;
    bool full_redraw_pending;
    int full_redraw_countdown;
    bool button_hover_dirty;
    bool corner_hover_dirty;
    bool is_invalidated;
    String status_message;
    int status_message_timer;
    int status_message_state;
    int screenshot_cooldown;
    int player_list_cooldown;

    InterfaceController();
    ~InterfaceController();
    void Logout();
    void RequestFullRedraw();
    void Invalidate();
    void OnMouseMove(int mouse_x, int mouse_y);
    bool UpdateCornerHover(int mouse_x, int mouse_y);
    bool UpdateButtonHover(int mouse_x, int mouse_y);
    bool TakeInvalidated();
    bool IsFullRedrawPending();
    void ClearFullRedraw();
    bool TakeButtonHoverDirty();
    bool TakeCornerHoverDirty();
    void ShowStatusMessage(int category, String text);
    int TakeStatusChange();
    String GetStatusMessage();
    bool IsScreenshotCooldown();
    void StartScreenshotCooldown();
    void StartPlayerListCooldown();
    void OnTimer();
    void UpdateScreenEffects();
    void ShowTimedText(String text, int duration);
    void StartScreenShake(int strength);
    void SetTradeRequesterId(int player_id);
    int GetTradeRequesterId();
};

#endif
