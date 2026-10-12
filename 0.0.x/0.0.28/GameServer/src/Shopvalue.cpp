#include <vcl.h>
#pragma hdrstop

#include "Shopvalue.h"
#include "Shopcraft.h"
#include "Shopitem.h"

#pragma package(smart_init)

ShopValue::ShopValue()
{
}

ShopValue::ShopValue(int id)
{
    this->id = id;
    this->trades.clear();
    this->crafts.clear();
}

ShopValue::~ShopValue()
{
}

void ShopValue::AddTrade(int item_id, int buy_price, int sell_price, int max_amount)
{
    ShopItemVal trade(item_id);
    trade.buy_price = buy_price;
    trade.sell_price = sell_price;
    trade.max_amount = max_amount;
    trades.insert(trades.end(), trade);
}

void ShopValue::AddCraft(int item_id, int ingredient_item_id_1, int ingredient_amount_1, int ingredient_item_id_2,
    int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3, int ingredient_item_id_4,
    int ingredient_amount_4)
{
    ShopCraftVal craft(item_id);
    craft.ingredient_item_ids[0] = ingredient_item_id_1; craft.ingredient_amounts[0] = ingredient_amount_1;
    craft.ingredient_item_ids[1] = ingredient_item_id_2; craft.ingredient_amounts[1] = ingredient_amount_2;
    craft.ingredient_item_ids[2] = ingredient_item_id_3; craft.ingredient_amounts[2] = ingredient_amount_3;
    craft.ingredient_item_ids[3] = ingredient_item_id_4; craft.ingredient_amounts[3] = ingredient_amount_4;
    crafts.insert(crafts.end(), craft);
}
