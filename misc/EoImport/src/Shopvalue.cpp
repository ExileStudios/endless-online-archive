#include <vcl.h>
#pragma hdrstop

#include "Shopvalue.h"

#pragma package(smart_init)

void Shopvalue_UnusedInstances1(std::vector<ShopItemVal> &trades, std::vector<ShopCraftVal> &crafts)
{
    trades.clear();
    crafts.clear();
}

ShopValue::ShopValue(int record_id)
{
    id = record_id;
    behavior_id = record_id;
    trades.clear();
    crafts.clear();
}

ShopValue::~ShopValue()
{
}

// Adds an item the shop trades, with its prices and the most traded at once.
void ShopValue::AddTrade(int item_id, int sell_price, int buy_price, int max_amount)
{
    ShopItemVal trade(item_id);
    trade.sell_price = sell_price;
    trade.buy_price = buy_price;
    trade.max_amount = max_amount;
    trades.insert(trades.end(), trade);
}

// Adds an item the shop crafts, with its four ingredients and their amounts.
void ShopValue::AddCraft(int item_id, int ingredient_item_id_1, int ingredient_amount_1, int ingredient_item_id_2,
    int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3, int ingredient_item_id_4,
    int ingredient_amount_4)
{
    ShopCraftVal craft(item_id);
    craft.ingredient_item_ids[0] = ingredient_item_id_1;
    craft.ingredient_amounts[0] = ingredient_amount_1;
    craft.ingredient_item_ids[1] = ingredient_item_id_2;
    craft.ingredient_amounts[1] = ingredient_amount_2;
    craft.ingredient_item_ids[2] = ingredient_item_id_3;
    craft.ingredient_amounts[2] = ingredient_amount_3;
    craft.ingredient_item_ids[3] = ingredient_item_id_4;
    craft.ingredient_amounts[3] = ingredient_amount_4;
    crafts.insert(crafts.end(), craft);
}
