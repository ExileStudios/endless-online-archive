#ifndef MapdoorH
#define MapdoorH

// A door on the map: its tile, key and open state.
struct MapDoor
{
    unsigned short x;
    unsigned short y;
    unsigned short key;
    char open;
    int closed_graphic;
    int close_timer;

    MapDoor(int tile_x, int tile_y, int door_key);
    ~MapDoor();
};

#endif
