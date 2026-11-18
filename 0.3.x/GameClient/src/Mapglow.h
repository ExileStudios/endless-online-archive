#ifndef MapglowH
#define MapglowH

struct MapGlow
{
    unsigned char tile_x;
    unsigned char tile_y;
    unsigned char kind;
    char color_index;
    char strength;
    unsigned char size;
    char unused_bytes[1];
    unsigned char offset_x;
    unsigned char offset_y;
    char point_offsets[5][2];
    char point_jitter[5][2];
    char point_phases[6][2];
    char phase_base[2];

    ~MapGlow();
};

#endif
