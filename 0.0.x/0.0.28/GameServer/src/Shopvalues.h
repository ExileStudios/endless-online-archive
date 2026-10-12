#ifndef ShopvaluesH
#define ShopvaluesH

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

struct ShopValue;

#include "Shopvalue.h"

struct MapCoord;
struct ShopValues;

// A pair of numbers; the union names the pair for each use ( x and y, item and amount, min and max damage, .. ).
struct MapCoord
{
    union
    {
        struct
        {
            int x;
            int y;
        };
        struct
        {
            int element;
            int element_damage;
        };
        struct
        {
            int spec2;
            int spec3;
        };
        struct
        {
            int type;
            int behavior_id;
        };
        struct
        {
            int item_id;
            int amount;
        };
        struct
        {
            int min_damage;
            int max_damage;
        };
        struct
        {
            int id;
            int element_power;
        };
    };

    MapCoord() {}
};

// The shops, read from pub/dts001.esf.
struct ShopValues
{
    char loaded;
    vector<ShopValue> record_list;
    int record_index;

    ShopValues();
    ~ShopValues();
    void LoadShops();
    void AddTrade(ShopValue *shop, int item_id, int buy_price, int sell_price, int max_amount);
    void AddCraft(ShopValue *shop, int item_id, int ingredient_item_id_1, int ingredient_amount_1,
        int ingredient_item_id_2, int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3,
        int ingredient_item_id_4, int ingredient_amount_4);
    void Clear();
    MapCoord GetCraftIngredient1(int shop_id, int craft_id);
    MapCoord GetCraftIngredient2(int shop_id, int craft_id);
    MapCoord GetCraftIngredient3(int shop_id, int craft_id);
    MapCoord GetCraftIngredient4(int shop_id, int craft_id);
    int GetBuyPrice(int shop_id, int item_id, int amount);
    int GetSellPrice(int shop_id, int item_id, int amount);
    String BuildOpenData(int behavior_id);
    unsigned int GetCount();
    String EncodeNumber(unsigned int value, int width);
    int DecodeNumber(String value);
};


#endif
