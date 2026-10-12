#ifndef Sys_itemsH
#define Sys_itemsH

#include <vector.h>

#include "Sys_item.h"

// The items on the map, and a second list that keeps protected items while their protection runs.
struct MapItems
{
    char hover_label_shown;
    char list_changed;
    char pad_0x2[6];
    vector<MapItem> items;
    vector<MapItem> protected_items;
    int current_index;
    int mouse_x;
    int mouse_y;
    char pad_0x54[4];

    MapItems();
    ~MapItems();
    void UpdateHoverLabel();
    void RemoveDistantItems(int center_x, int center_y);
    void SortByPosition();
    void AddItem(int uid, int item_id, int x, int y, int amount);
    void AddProtectedItem(int uid, int item_id, int x, int y, int amount, int owner_id, int protection_ticks);
    void RemoveItem(int uid);
    MapItem GetItem(int index);
    int GetProtectionOwnerAt(int x, int y);
    int GetProtectionOwner(int uid);
    bool HasItem(int uid);
    bool HasItemAt(int x, int y);
    int GetItemId(int uid);
    int FindItemAt(int x, int y);
    void ClearAll();
    void ClearKeepingProtection();
    unsigned int GetCount();
    bool IsInViewRange(int center_x, int center_y, int x, int y);
    void SetMousePosition(int cursor_x, int cursor_y);
};

#endif
