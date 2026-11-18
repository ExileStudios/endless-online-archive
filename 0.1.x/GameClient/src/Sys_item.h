#ifndef Sys_itemH
#define Sys_itemH

// An item lying on the map: uid, record id, tile and amount, and the owner who holds it while protection_ticks runs.
struct MapItem
{
    int uid;
    int item_id;
    int x;
    int y;
    int amount;
    int owner_id;
    int protection_ticks;
    char out_of_range;
    int removal_countdown;
    char label_shown;

    MapItem();
    MapItem(int item_uid, int record_id, int tile_x, int tile_y, int item_amount);
    ~MapItem();
};

#endif
