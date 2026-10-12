#ifndef Sys_effectH
#define Sys_effectH

// A spell effect on a map tile: its frame, the loops left and which layers it draws.
struct MapEffect
{
    bool sound_pending;
    bool finished;
    int x;
    int y;
    int effect_id;
    int frame;
    int last_frame;
    int loops_left;
    char has_base_layer;
    char has_blend_layer;
    char has_solid_layer;

    MapEffect();
    MapEffect(int tile_x, int tile_y, int effect_number);
    ~MapEffect();
};

#endif
