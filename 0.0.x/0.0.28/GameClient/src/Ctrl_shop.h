#ifndef Ctrl_shopH
#define Ctrl_shopH

#include <vector.h>

#include "Itemcraft.h"
#include "Itemshop.h"

enum ShopPage
{
    ShopPage_Menu = 0,
    ShopPage_Buy = 1,
    ShopPage_Sell = 2,
    ShopPage_Craft = 3,
    ShopPage_Unknown4 = 4
};

// The shop window: the items to buy, to sell and to craft, the page shown with its scrolling list, and the item
// picked for the confirm or amount box.
struct ShopController
{
    vector<ShopItem> shown_items;
    vector<ShopItem> buy_items;
    vector<ShopItem> sell_items;
    vector<CraftItem> craft_items;
    char close_hovered;
    char back_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    String npc_name;
    int page;
    int npc_session_id;
    int selected_item_id;
    int selected_amount;
    int selected_price;
    char unused_cleared_flag;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    char scroll_changed;
    int unused_value;
    int top_row;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int scroll_repeat_delay;
    char pad_0xd4[4];

    ShopController();
    ~ShopController();
    void Open(String npc, int session_id);
    void Close();
    void AddCraftItem(int item_id, int first_ingredient_id, int first_ingredient_amount, int second_ingredient_id,
        int second_ingredient_amount, int third_ingredient_id, int third_ingredient_amount, int fourth_ingredient_id,
        int fourth_ingredient_amount);
    void AddTradeItem(int list_type, int item_id, int item_id_copy, int price, int amount);
    void Tick();
    int GetScrollThumbOffset();
    int GetTopRow();
    void ResetScroll();
    int CountSellableItems();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
};

#endif
