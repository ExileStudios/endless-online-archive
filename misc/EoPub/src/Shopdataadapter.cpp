#include <vcl.h>
#pragma hdrstop

#include "Shopdataadapter.h"
#include "UnitMain.h"
#include "Shopvalue.h"
#include "Shopvalues.h"
#include <vector.h>

#pragma package(smart_init)

ShopDataAdapter::ShopDataAdapter()
{
}

ShopDataAdapter::~ShopDataAdapter()
{
}

// How many trades of all the shops are of the item.
int ShopDataAdapter::ItemTradeCount(int item_id)
{
    ShopValue *shop;
    int count = 0;
    for (int i = 0; i < (unsigned int)FormMain->shops_tab->shop_values->Count(); i++)
    {
        shop = FormMain->shops_tab->shop_values->GetShop(i);
        for (int j = 0; j < shop->trades.size(); j++)
        {
            if (shop->trades[j].item_id == item_id)
                count++;
        }
    }
    return count;
}

// How many crafts of all the shops make the item.
int ShopDataAdapter::ItemCraftCount(int item_id)
{
    ShopValue *shop;
    int count = 0;
    for (int i = 0; i < (unsigned int)FormMain->shops_tab->shop_values->Count(); i++)
    {
        shop = FormMain->shops_tab->shop_values->GetShop(i);
        for (int j = 0; j < shop->crafts.size(); j++)
        {
            if (shop->crafts[j].id == item_id)
                count++;
        }
    }
    return count;
}

// Deletes the shop at the index and moves the IDs of the shops after it down by one.
void ShopDataAdapter::DeleteAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->shops_tab->shop_values->Count() <= index)
        return;
    FormMain->shops_tab->shop_values->DeleteShop(index);
    while ((unsigned int)FormMain->shops_tab->shop_values->Count() > index)
    {
        FormMain->shops_tab->shop_values->GetShop(index)->id--;
        index++;
    }
}

// Inserts a shop at the index and moves the IDs of the shops after it up by one.
void ShopDataAdapter::InsertAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->shops_tab->shop_values->Count() <= index)
        return;
    FormMain->shops_tab->shop_values->InsertShop(index);
    while ((unsigned int)FormMain->shops_tab->shop_values->Count() > index)
    {
        FormMain->shops_tab->shop_values->GetShop(index)->id++;
        index++;
    }
}

// Copies the crafts and trades of the shop with the first ID onto the shop with the second.
void ShopDataAdapter::CopyLists(String source_id, String target_id)
{
    if (source_id.Length() < 1)
        return;
    if (target_id.Length() < 1)
        return;
    ShopValue *source = FormMain->shops_tab->shop_values->GetShop(StrToInt(source_id) - 1);
    ShopValue *target = FormMain->shops_tab->shop_values->GetShop(StrToInt(target_id) - 1);
    target->crafts.clear();
    target->trades.clear();
    for (int i = 0; i < source->crafts.size(); i++)
    {
        ShopCraftVal craft = source->crafts[i];
        ShopCraftVal craft_copy(i);
        craft_copy = craft;
        target->crafts.insert(target->crafts.end(), craft_copy);
    }
    for (int j = 0; j < source->trades.size(); j++)
    {
        ShopItemVal trade = source->trades[j];
        ShopItemVal trade_copy(j);
        trade_copy = trade;
        target->trades.insert(target->trades.end(), trade_copy);
    }
}
