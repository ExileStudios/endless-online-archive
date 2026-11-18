#ifndef Sys_npcH
#define Sys_npcH

struct Balloon;

// One npc on the map: record id, graphic, position and the state of each animation, with a queue of balloons.
struct Npc
{
    int index;
    int bar_index;
    int npc_id;
    int graphic;
    bool hidden;
    unsigned char idle_frame;
    char dying;
    bool killed_by_hit;
    short death_countdown;
    char out_of_range;
    char graphics_pending;
    int load_stage;
    char reload_requested;
    char label_shown;
    String name;
    char redraw;
    char walk_start;
    char walking;
    int walk_direction;
    int walk_frame;
    char attack_start;
    char attacking;
    int attack_direction;
    int attack_frame;
    bool alt_attack_start;
    bool alt_attacking;
    int alt_attack_direction;
    int alt_attack_frame;
    char effect_start;
    char effect_running;
    int effect_id;
    int effect_frame;
    int effect_last_frame;
    int effect_loops_left;
    char effect_base_layer;
    char effect_blend_layer;
    char effect_solid_layer;
    int walk_target_x;
    int walk_target_y;
    int walk_facing;
    int walk_target_direction;
    int x;
    int y;
    int direction;
    int frame_width;
    int frame_height;
    char health_bar_shown;
    char health_bar_start;
    char health_bar_kind;
    int health_bar_timer;
    unsigned char health_bar_width;
    unsigned char second_bar_width;
    unsigned char third_bar_width;
    char damage_shown;
    char damage_start;
    int damage_digits;
    int damage_timer;
    int damage_amount;
    char damage_heal;
    short balloon_timer;
    char balloon_shown;
    Balloon *balloon;
    String queued_balloon_lines[6];
    int queued_balloon_styles[6];
    int queued_balloon_count;

    Npc(int map_index);
    Npc(int map_index, int record_id, int graphic_id, String display_name, int tile_x, int tile_y, int facing);
    ~Npc();
    TPoint GetAnimatedPosition(int screen_x, int screen_y);
    void ShowBalloon(String speaker, String text, int style);
    void ShowNextBalloon();
    void DrawBalloon();
    void FreeBalloon();
};

#endif
