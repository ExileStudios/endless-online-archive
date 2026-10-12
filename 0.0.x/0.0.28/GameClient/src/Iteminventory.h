#ifndef IteminventoryH
#define IteminventoryH

// One item in a grid of the inventory, chest or locker: its place, id and amount.
struct InventoryItem
{
    int grid_x;
    int grid_y;
    int item_id;
    unsigned int amount;
    char is_visible;
    char is_placed;

    InventoryItem(int id);
    ~InventoryItem();
};

#endif
