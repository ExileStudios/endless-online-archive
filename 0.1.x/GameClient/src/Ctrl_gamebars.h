#ifndef Ctrl_gamebarsH
#define Ctrl_gamebarsH

// The strip at the top of the game screen: the stats and quest buttons and four bars.
// The health, mana, stamina and experience bars have 20 segments of 5 percent each.
struct GameBars
{
    bool show_health_text;
    int health_text_timer;
    bool show_mana_text;
    int mana_text_timer;
    bool show_stamina_text;
    int stamina_text_timer;
    bool show_experience_text;
    int experience_text_timer;
    int health_segments;
    int mana_segments;
    int stamina_segments;
    int experience_segments;
    int quest_request_cooldown;
    char is_dirty;

    GameBars();
    ~GameBars();
    void MouseDown(int x, int y, int button);
    void Tick();
    void SetHealthBar(int health, int max_health);
    void SetManaBar(int mana, int max_mana);
    void SetStaminaBar(int stamina, int max_stamina);
    void SetExperienceBar(int level_progress, int level_span);
    int GetHealthBarSegments();
    int GetManaBarSegments();
    int GetStaminaBarSegments();
    int GetExperienceBarSegments();
    void MarkDirty();
    bool TestAndClearDirty();
};

#endif
