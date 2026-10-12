#ifndef Ctrl_chestH
#define Ctrl_chestH

#include <vector.h>

#include "Iteminventory.h"

// The chest window: the items in the chest at a map tile.
struct ChestController
{
    vector<InventoryItem> items;
    char close_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int reserved_value;
    int chest_x;
    int chest_y;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char pad_0x48[8];

    ChestController();
    ~ChestController();
    void AddItem(int item_id, int amount);
    void ClearItems();
    void Open(int reserved, int x, int y);
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
