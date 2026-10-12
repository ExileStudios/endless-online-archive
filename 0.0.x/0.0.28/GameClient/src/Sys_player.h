#ifndef Sys_playerH
#define Sys_playerH

enum HatGraphic
{
    HatGraphic_DragonMask = 15,
    HatGraphic_BlackHood = 16,
    HatGraphic_ChainmailHat = 21,
    HatGraphic_HornedHat = 25,
    HatGraphic_MerchantHat = 26,
    HatGraphic_Helmy = 28,
    HatGraphic_EloffHelmet = 30,
    HatGraphic_AirHat = 31,
    HatGraphic_FrogHead = 32,
    HatGraphic_Pilotte = 33,
    HatGraphic_Beruta = 34,
    HatGraphic_PirateHat = 35,
    HatGraphic_LotusHelmet = 36,
    HatGraphic_KittyHat = 37,
    HatGraphic_HulaHulaHat = 38,
    HatGraphic_GobHelm = 40,
    HatGraphic_HornedGobHelm = 41,
    HatGraphic_HelmetDarkness = 44,
    HatGraphic_FladHat = 46,
    HatGraphic_CookHat = 47,
    HatGraphic_Glasses = 48,
    HatGraphic_MedicCap = 50
};

enum ShieldGraphic
{
    ShieldGraphic_GoodWings = 10,
    ShieldGraphic_Bag = 11,
    ShieldGraphic_NormalArrows = 14,
    ShieldGraphic_FrostArrows = 15,
    ShieldGraphic_FireArrows = 16,
    ShieldGraphic_GoodForceWings = 18,
    ShieldGraphic_FireForceWings = 19
};

enum FacingDirection
{
    FacingDirection_Down = 0,
    FacingDirection_Left = 1,
    FacingDirection_Up = 2,
    FacingDirection_Right = 3
};

struct Balloon;
struct PlayerResources;

// One player on the map: position, looks, equipment and the state of every animation.
struct Player
{
    char out_of_range;
    int removal_countdown;
    char graphics_pending;
    char reload_requested;
    int load_progress;
    bool sheets_cleared;
    bool walk_sheets_loaded;
    bool walk_tail_sheets_loaded;
    bool stand_sheet_loaded;
    bool sit_sheets_loaded;
    bool attack_sheets_loaded;
    bool extra_sheets_loaded;
    bool cast_sheets_loaded;
    bool emote_sheets_loaded;
    char hover_label_shown;
    String name;
    String secondary_name;
    bool text_label_shown;
    char text_label_blinking;
    int text_label_timer;
    String text_before_guild_tag;
    String guild_tag;
    String text_after_guild_tag;
    char dying;
    int death_countdown;
    bool hiding;
    int hide_countdown;
    bool unhiding;
    unsigned char unhide_countdown;
    int record_value_a;
    int record_value_b;
    int character_id;
    int display_number;
    bool level_up_pending;
    int player_id;
    int record_value_d;
    int slot_state;
    char redraw;
    bool emote_start;
    char emoting;
    int emote_id;
    int emote_first_frame;
    int emote_step;
    int emote_frame;
    int emote_frame_count;
    char walk_start;
    char walking;
    int walk_direction;
    int walk_frame;
    bool jump_start;
    bool jumping;
    int jump_direction;
    int jump_frame;
    char attack_start;
    char attacking;
    int attack_direction;
    int attack_frame;
    char silent_attack;
    bool cast_start;
    char casting;
    int cast_direction;
    int cast_frame;
    char effect_start;
    int pending_effect;
    char effect_running;
    int effect_id;
    int effect_frame;
    int effect_last_frame;
    int effect_loops_left;
    char effect_base_layer;
    char effect_blend_layer;
    char effect_solid_layer;
    bool sitting_on_chair;
    int chair_direction;
    bool sitting_on_floor;
    int floor_direction;
    bool chair_sit_start;
    bool floor_sit_start;
    bool stand_up_start;
    char recenter_view;
    bool pose_flag;
    int target_x;
    int target_y;
    int target_direction;
    int walk_target_direction;
    unsigned int x;
    unsigned int y;
    int position_extra;
    int direction;
    int equip_slot_a;
    int equip_slot_b;
    int equip_slot_c;
    int equip_slot_d;
    int boots;
    int equip_after_boots_a;
    int equip_after_boots_b;
    int armor;
    int equip_after_armor;
    int equip_before_hat;
    int hat;
    int shield;
    int weapon;
    int equip_after_weapon_a;
    int equip_after_weapon_b;
    int equip_after_weapon_c;
    int equip_after_weapon_d;
    int equip_after_weapon_e;
    int equip_after_weapon_f;
    int skin;
    int hair_style;
    int hair_color;
    int gender;
    char hidden;
    char health_bar_shown;
    char health_bar_start;
    int health_bar_timer;
    int health_bar_width;
    char damage_shown;
    char damage_start;
    int damage_digits;
    int damage_timer;
    int damage_amount;
    char damage_heal;
    short balloon_timer;
    short balloon_height;
    short balloon_width;
    char balloon_shown;
    short badge_level;
    PlayerResources *resources;
    Balloon *balloon;

    Player();
    Player(int tile_x, int tile_y, String player_name, int player_gender, int player_skin, int player_hair_style,
        int player_hair_color, char start_pending);
    ~Player();
    void ShowBalloon(String *lines, int width, int height, int tinted);
    Windows::TPoint GetAnimatedPosition(int screen_x, int screen_y);
    void DrawEmote();
    void DrawBalloon();
    void DrawHoverLabel();
    void DrawTextLabel();
    void CloseBalloon();
    void CreateResources();
    void FreeResources();
};

#endif
