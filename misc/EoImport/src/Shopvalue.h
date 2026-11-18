#ifndef ShopvalueH
#define ShopvalueH

#include <new>
#include <vector.h>

#include "Shopcraft.h"
#include "Shopitem.h"

// One shop: its name, the levels and class it serves, what it trades and what it crafts.
struct ShopValue
{
    int id;
    int behavior_id;
    String name;
    short min_level;
    short max_level;
    short class_requirement;
    vector<ShopItemVal> trades;
    vector<ShopCraftVal> crafts;

    ShopValue(int record_id);
    ~ShopValue();
    void AddTrade(int item_id, int sell_price, int buy_price, int max_amount);
    void AddCraft(int item_id, int ingredient_item_id_1, int ingredient_amount_1, int ingredient_item_id_2,
    int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3, int ingredient_item_id_4,
    int ingredient_amount_4);
};

#endif
