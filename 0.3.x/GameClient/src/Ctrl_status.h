#ifndef Ctrl_statusH
#define Ctrl_statusH

#define GUILD_RANK_LOWEST 9

// The own character's numbers: health, mana, stamina, weight, level and experience, stat and skill points, the
// guild tag, and the experience totals the stats window shows.
struct StatusController
{
    String character_name;
    String character_second_text;
    String guild_rank_name;
    int class_id;
    int guild_rank;
    String guild_tag;
    String guild_name;
    int weight;
    int max_weight;
    short level;
    long experience_to_next_level;
    long next_level_experience;
    bool skip_next_exp_gain;
    int last_exp_gain;
    int best_exp_gain;
    int total_exp_gained;
    int health;
    int max_health;
    int mana;
    int max_mana;
    int stamina;
    int max_stamina;
    String stamina_check_text;
    String max_stamina_check_text;
    int unused_value;
    long experience;
    int gold;
    int usage_minutes;
    TDateTime usage_start_time;
    TDateTime ping_sent_time;
    int alignment;
    int skill_points;
    int stat_points;
    int train_confirm_ticks;
    int train_stat_index;
    int damage_min;
    int damage_max;
    int hitrate;
    int dodge;
    int armor;
    int stat_str;
    int stat_wis;
    int stat_int;
    int stat_agi;
    int stat_con;
    int stat_cha;
    bool is_low_health;
    unsigned char low_health_ticks;
    unsigned char flash_ticks;
    int flash_color;
    char is_dirty;
    int stamina_regen_ticks;
    int blink_phase;

    StatusController();
    ~StatusController();
    void Tick();
    bool IsLowHealth();
    char GetLowHealthAlpha();
    void FlashScreen(int color);
    unsigned char GetFlashAlpha();
    int GetFlashColor();
    void MouseDown(int x, int y, int button);
    int GetBlinkPhase();
    void SetHealth(int new_health, char show_change);
    void SetMana(int new_mana, char show_change);
    void SetStamina(int new_stamina, char show_change);
    void SetMaxHealth(int new_max_health);
    void SetMaxMana(int new_max_mana);
    void SetMaxStamina(int new_max_stamina);
    short GetHealthPercent();
    short GetManaPercent();
    String GetWeightText();
    void SetExperience(int new_experience);
    void SetLevel(int new_level);
    bool IsInGuild();
    bool TestAndClearDirty();
    void MarkDirty();
};

#endif
