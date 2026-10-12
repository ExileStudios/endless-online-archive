#ifndef Sys_npcH
#define Sys_npcH

struct Balloon;
struct NpcResources;

// One npc on the map: record id, graphic, position and the state of each animation, with a queue of balloons.
struct Npc
{
    unsigned int index;
    int npc_id;
    int graphic;
    int idle_frame;
    char dying;
    int death_countdown;
    char out_of_range;
    int removal_countdown;
    char graphics_pending;
    char reload_requested;
    int load_stage;
    char label_shown;
    String name;
    int hidden;
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
    unsigned int x;
    unsigned int y;
    int direction;
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
    int frame_width;
    int frame_height;
    short balloon_timer;
    short balloon_height;
    short balloon_width;
    char balloon_shown;
    NpcResources *resources;
    Balloon *balloon;
    String queued_balloon_lines[6][5];
    int queued_balloon_widths[5];
    int queued_balloon_heights[5];
    int queued_balloon_tinted[5];
    int queued_balloon_count;

    Npc();
    Npc(int map_index, int record_id, int graphic_id, String display_name, int tile_x, int tile_y, int facing);
    ~Npc();
    void CreateResources();
    static Windows::TPoint GetAnimatedPosition(Npc *self, int screen_x, int screen_y);
    void ShowBalloon(String *lines, int width, int height, int tinted);
    void DrawBalloon();
    void CloseBalloon();
    void FreeResources();
};

#endif
