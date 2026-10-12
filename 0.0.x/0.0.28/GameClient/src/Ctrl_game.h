#ifndef Ctrl_gameH
#define Ctrl_gameH

enum AmountBoxKind
{
    AmountBoxKind_DropItem = 1,
    AmountBoxKind_JunkItem = 2,
    AmountBoxKind_TradeAddItem = 4,
    AmountBoxKind_ChestAddItem = 5,
    AmountBoxKind_BankDeposit = 6,
    AmountBoxKind_BankWithdraw = 7,
    AmountBoxKind_ShopBuy = 8,
    AmountBoxKind_ShopSell = 9,
    AmountBoxKind_LockerAddItem = 10,
    AmountBoxKind_GuildDeposit = 11
};

enum ConfirmAction
{
    ConfirmAction_None = 0,
    ConfirmAction_ConnectionLost = 1,
    ConfirmAction_Logout = 2,
    ConfirmAction_AcceptPartyJoin = 3,
    ConfirmAction_AcceptPartyInvite = 4,
    ConfirmAction_EnableSound = 5,
    ConfirmAction_EnableMusic = 6,
    ConfirmAction_AcceptTrade = 7,
    ConfirmAction_TradeAgree = 8,
    ConfirmAction_BuyItem = 9,
    ConfirmAction_SellItem = 10,
    ConfirmAction_BuyHairstyle = 11,
    ConfirmAction_LeaveGuild = 12,
    ConfirmAction_CreateGuild = 13,
    ConfirmAction_DisbandGuild = 14,
    ConfirmAction_UseItem = 15,
    ConfirmAction_JukeboxRequest = 16,
    ConfirmAction_TrainStat = 17,
    ConfirmAction_TrainSpell = 18,
    ConfirmAction_TrainSkill = 19,
    ConfirmAction_AcceptWeddingRequest = 20,
    ConfirmAction_ConfirmWeddingVow = 21,
    ConfirmAction_LearnSkill = 22,
    ConfirmAction_SkillReset = 23,
    ConfirmAction_InnSleep = 24,
    ConfirmAction_BuyLockerUpgrade = 25
};

enum FormKind
{
    FormKind_GuildCreate = 1,
    FormKind_GuildJoin = 2,
    FormKind_GuildRankNames = 3,
    FormKind_GuildMemberRank = 4,
    FormKind_ReportPlayer = 5
};

enum InputBoxKind
{
    InputBoxKind_AddFriend = 0,
    InputBoxKind_AddIgnore = 1,
    InputBoxKind_GuildInfo = 2,
    InputBoxKind_GuildDescription = 3,
    InputBoxKind_RemoveGuildMember = 4,
    InputBoxKind_GuildMemberList = 5,
    InputBoxKind_WeddingPartner = 6,
    InputBoxKind_LawOption1 = 7,
    InputBoxKind_LawOption2 = 8,
    InputBoxKind_FindSkill = 9,
    InputBoxKind_HelpQuestion = 10,
    InputBoxKind_InnAnswer = 11
};

enum LargeBoxKind
{
    LargeBoxKind_CraftItem = 1,
    LargeBoxKind_AcceptGuildInvite = 2,
    LargeBoxKind_AcceptGuildJoinRequest = 3,
    LargeBoxKind_WeddingRequest = 4
};
#define MESSAGE_BOX_LINES 2
#define NPC_LIST_ROW_HEIGHT 37

enum DialogButtons
{
    DialogButtons_Cancel = 2,
    DialogButtons_OkCancel = 3
};
#define INPUT_BOX_LINES 2
#define INPUT_FORM_MAX_ROWS 25

#define INPUT_FORM_ROW_HEIGHT 23

// The game screen's boxes (message, large message, input box, form, amount box), the player menu, and the own
// character's actions: walking, sitting, attacking, emotes and using tiles.
struct GameController
{
    int restricted_map_id;
    int welcome_value_a;
    int welcome_value_b;
    int welcome_value_c;
    bool form_ok_hovered;
    bool form_cancel_hovered;
    int form_row_count;
    int form_kind;
    String form_title;
    String form_caption;
    String form_row_labels[INPUT_FORM_MAX_ROWS];
    String form_row_texts[INPUT_FORM_MAX_ROWS];
    short form_row_char_filters[INPUT_FORM_MAX_ROWS];
    short form_row_max_lengths[INPUT_FORM_MAX_ROWS];
    short form_row_case_modes[INPUT_FORM_MAX_ROWS];
    unsigned char form_row_password_flags[INPUT_FORM_MAX_ROWS];
    bool form_visible;
    int form_x;
    int form_y;
    bool form_scroll_up_hovered;
    bool form_scroll_down_hovered;
    int form_top_row;
    int form_thumb_offset;
    bool form_thumb_grabbed;
    int form_thumb_grab_offset;
    bool form_scroll_up_held;
    bool form_scroll_down_held;
    int form_scroll_repeat_delay;
    bool input_box_ok_hovered;
    bool input_box_cancel_hovered;
    int input_box_kind;
    String input_box_lines[INPUT_BOX_LINES];
    bool input_box_visible;
    int input_box_x;
    int input_box_y;
    bool large_box_ok_hovered;
    bool large_box_cancel_hovered;
    String large_box_title;
    int large_box_buttons;
    int large_box_kind;
    String large_box_lines[7];
    String large_box_text;
    bool large_box_visible;
    int large_box_x;
    int large_box_y;
    int amount_slider_offset;
    int amount_max;
    int amount_box_opened;
    bool amount_slider_grabbed;
    int amount_slider_grab_offset;
    int amount_item_id;
    int amount_box_reserved;
    int amount_box_kind;
    int amount_box_x;
    int amount_box_y;
    bool amount_box_visible;
    int drop_x;
    int drop_y;
    bool amount_ok_hovered;
    bool amount_cancel_hovered;
    bool amount_slider_hovered;
    bool message_ok_hovered;
    bool message_cancel_hovered;
    bool message_visible;
    int message_x;
    int message_y;
    String message_title;
    String message_lines[MESSAGE_BOX_LINES];
    int message_buttons;
    int message_kind;
    bool player_menu_visible;
    int player_menu_x;
    int player_menu_y;
    String player_menu_name;
    int player_menu_target_id;
    int player_menu_hovered_option;
    int idle_ticks;
    int attack_count;
    TTimeStamp last_action_time;
    char ghost_walk;
    int spot_countdown;
    int spot_x;
    int spot_y;
    int blocking_player_id;
    int last_blocking_player_id;
    int blocked_ticks;
    int blocked_direction;
    int blocked_x;
    int blocked_y;
    int message_drag_mode;
    bool message_dragging;
    int message_drag_offset_x;
    int message_drag_offset_y;
    bool large_box_dragging;
    int large_box_drag_offset_x;
    int large_box_drag_offset_y;
    bool amount_box_dragging;
    int amount_box_drag_offset_x;
    int amount_box_drag_offset_y;
    bool input_box_dragging;
    int input_box_drag_offset_x;
    int input_box_drag_offset_y;
    bool form_dragging;
    int form_drag_offset_x;
    int form_drag_offset_y;
    char unused_flag;

    GameController();
    ~GameController();
    void CenterViewOnPlayer(int player_id);
    bool StandUp();
    bool ToggleSit(int x, int y);
    bool SitOnChair(int x, int y);
    void CastUntargetedSpell(int spell_id);
    void Attack(char animation_only);
    void PlayEmote(int emote_id);
    void Walk(int direction);
    void OpenPlayerMenu(int x, int y, int target_id, String player_name);
    void ClosePlayerMenu();
    void CloseInputBox();
    void CloseInputForm();
    void ShowLargeMessageBox(int buttons, int kind, String title, String text);
    void ShowMessageBox(int buttons, int kind, String title, String text);
    void OpenInputForm(int kind, int row_count, int char_filter, int max_length, String title, String caption);
    void OpenInputBox(int kind, String prompt, int char_filter, int max_length);
    void OpenAmountBoxAtTile(int kind, int max_amount, int item_id, int reserved, int x, int y);
    void OpenAmountBox(int kind, int max_amount, int item_id, int reserved);
    void MouseDown(int x, int y);
    void MouseMove(int x, int y);
    void AddInputFormRow(String label, String text, short char_filter, short max_length, short case_mode,
        char password);
    void ShowInputFormRows(int first_row);
    void SaveInputFormRows();
    int GetInputFormThumbOffset();
    int GetInputFormTopRow();
    void InputFormScrollUp();
    void InputFormScrollDown();
    void InputFormDragThumb(int y);
    char TrySitOnChair(int x, int y, int tile_spec);
    bool TryOpenLocker(int x, int y, int tile_spec);
    bool TryOpenBoard(int x, int y, int tile_spec);
    bool TryOpenJukebox(int x, int y, int tile_spec);
    bool TryOpenChest(int x, int y, int tile_spec);
    bool TryOpenDoor(int x, int y, int tile_spec);
    void Tick();
    void MouseUp();
    void ResetIdleTimer();
    int GetExperienceForLevel(int level);
    void StartSpotCountdown(int x, int y);
    void SetGhostWalk(char enabled);
    void ToggleGhostWalk();
    bool IsActionDelayElapsed();
};

#endif
