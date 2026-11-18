#ifndef MapsignH
#define MapsignH

// A sign on the map: its tile and text.
struct MapSign
{
    unsigned short x;
    unsigned short y;
    String text;

    MapSign(int tile_x, int tile_y, String sign_text);
    ~MapSign();
};

#endif
