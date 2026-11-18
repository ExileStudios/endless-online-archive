#ifndef Ctrl_crafterH
#define Ctrl_crafterH

#include <new>
#include <vector.h>

struct Tooltip;

#include "Itemcraft.h"

struct CrafterController
{
    vector<CraftIngredient> ingredients;
    int ingredient_count;
    char unused_bytes[8];
    bool can_craft;
    int max_crafts;
    short blink_counter;
    bool close_hovered;
    bool craft_hovered;
    Tooltip *tooltip;
    bool is_visible;
    int unused_value;
    int window_x;
    int window_y;
    String result_name;
    int result_item_id;
    char second_unused_bytes[4];
    int session_id;
    bool needs_layout;
    bool ingredient_hovered;
    bool is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    bool tooltip_dirty;

    CrafterController();
    ~CrafterController();
    void Update();
    void Open(String new_result_name, int new_result_item_id, int new_session_id);
    void Close();
    void AddIngredient(String name, int item_id, int amount);
    void Layout();
    void ClearHover();
    void ConfirmCraft();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void MouseUp();
};

#endif
