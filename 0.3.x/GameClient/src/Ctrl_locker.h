#ifndef Ctrl_lockerH
#define Ctrl_lockerH

#include <vector.h>

struct Tooltip;

#include "Iteminventory.h"

#define LOCKER_VISIBLE_ROWS 5

// The locker window: the items in the locker at a map tile, in a scrolling list.
struct LockerController
{
    vector<InventoryItem> items;
    char close_hovered;
    Tooltip *tooltip;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    char is_visible;
    int hovered_slot;
    int hovered_col;
    int hovered_row;
    int window_x;
    int window_y;
    String title;
    int reserved_value;
    int locker_x;
    int locker_y;
    bool items_changed;
    char scroll_changed;
    int unused_value;
    int top_row;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    int scroll_repeat_delay;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char unused_bytes[4];

    LockerController();
    ~LockerController();
    void AddItem(int item_id, int amount);
    void UpdateItem(int item_id, int amount);
    void ScrollToItem(int item_id);
    void Open(String window_title, int reserved, int x, int y);
    void Close();
    void Tick();
    int GetScrollThumbOffset();
    int GetTopRow();
    void ClearItems();
    void ClearList();
    void ClearSeenFlags();
    void RemoveUnseenItems();
    void UpdateScrollThumb();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
    char CanStoreItem(int item_id);
    int GetItemAmount(int item_id);
};

#endif
