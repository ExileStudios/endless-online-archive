#ifndef ShopvalueH
#define ShopvalueH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

struct ShopCraftVal;
struct ShopItemVal;

#include "Shopcraft.h"
#include "Shopitem.h"

struct ShopValue;

// A shop: the items it trades and the items it crafts.
struct ShopValue
{
    int id;
    String name;
    short min_level;
    short max_level;
    short class_requirement;
    vector<ShopItemVal> trades;
    vector<ShopCraftVal> crafts;

    ShopValue();
    ShopValue(int id);
    ~ShopValue();
    void AddTrade(int item_id, int buy_price, int sell_price, int max_amount);
    void AddCraft(int item_id, int ingredient_item_id_1, int ingredient_amount_1, int ingredient_item_id_2,
        int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3, int ingredient_item_id_4,
        int ingredient_amount_4);
};


#endif
