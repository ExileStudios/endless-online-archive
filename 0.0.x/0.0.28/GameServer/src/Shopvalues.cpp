#include <vcl.h>
#pragma hdrstop

#include "Shopvalues.h"
#include "Shopcraft.h"
#include "Shopitem.h"
#include "Shopvalue.h"
#include "Constants.h"

#pragma package(smart_init)

ShopValues::ShopValues()
{
    record_index = -1;
    loaded = 0;
    LoadShops();
}

ShopValues::~ShopValues()
{
}

// Reads pub/dts001.esf ( it starts ESF ): each shop, then its trades ( 9 bytes ) and crafts ( 14 bytes ).
void ShopValues::LoadShops()
{
    int file_handle;
    if (!loaded)
    {
        Clear();
        String data;
        String path = "./pub/dts001.esf";
        try
        {
            int size;
            char *file_buffer;
            file_handle = FileOpen(path.c_str(), 0);
            size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            file_buffer = new char[size + 1];
            FileRead(file_handle, file_buffer, size);
            FileClose(file_handle);
            data = file_buffer;
            data.SetLength(size);
            delete[] file_buffer;
            if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
                return;
            data.Delete(1, 3);
            do
            {
                int behavior_id = DecodeNumber(data.SubString(1, 2));
                int namelen = DecodeNumber(data.SubString(3, 1));
                ShopValue shop(behavior_id);
                shop.name = data.SubString(4, namelen);
                shop.min_level = DecodeNumber(data.SubString(namelen + 4, 1));
                shop.max_level = DecodeNumber(data.SubString(namelen + 5, 1));
                shop.class_requirement = DecodeNumber(data.SubString(namelen + 6, 1));
                int trades_count = DecodeNumber(data.SubString(namelen + 7, 2));
                int crafts_count = DecodeNumber(data.SubString(namelen + 9, 1));
                data.Delete(1, namelen + 9);
                for (int i = 0; i < trades_count; i++)
                {
                    AddTrade(&shop, DecodeNumber(data.SubString(1, 2)), DecodeNumber(data.SubString(3, 3)),
                        DecodeNumber(data.SubString(6, 3)), DecodeNumber(data.SubString(9, 1)));
                    data.Delete(1, 9);
                }
                for (int i = 0; i < crafts_count; i++)
                {
                    AddCraft(&shop, DecodeNumber(data.SubString(1, 2)), DecodeNumber(data.SubString(3, 2)),
                        DecodeNumber(data.SubString(5, 1)), DecodeNumber(data.SubString(6, 2)),
                        DecodeNumber(data.SubString(8, 1)), DecodeNumber(data.SubString(9, 2)),
                        DecodeNumber(data.SubString(11, 1)), DecodeNumber(data.SubString(12, 2)),
                        DecodeNumber(data.SubString(14, 1)));
                    data.Delete(1, 14);
                }
                record_list.insert(record_list.end(), shop);
            } while (data.Length() > 9);
            loaded = 1;
        }
        catch (...)
        {
            FileClose(file_handle);
            loaded = 0;
        }
    }
}

void ShopValues::AddTrade(ShopValue *shop, int item_id, int buy_price, int sell_price, int max_amount)
{
    shop->AddTrade(item_id, buy_price, sell_price, max_amount);
}

void ShopValues::AddCraft(ShopValue *shop, int item_id, int ingredient_item_id_1, int ingredient_amount_1,
    int ingredient_item_id_2, int ingredient_amount_2, int ingredient_item_id_3, int ingredient_amount_3,
    int ingredient_item_id_4, int ingredient_amount_4)
{
    shop->AddCraft(item_id, ingredient_item_id_1, ingredient_amount_1, ingredient_item_id_2, ingredient_amount_2,
        ingredient_item_id_3, ingredient_amount_3, ingredient_item_id_4, ingredient_amount_4);
}

void ShopValues::Clear()
{
    for (vector<ShopValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        it->trades.clear();
        it->crafts.clear();
    }
    if (GetCount() >= 1)
    {
        record_list.clear();
        record_index = -1;
    }
}

ShopValue &Shopvalues_Slot(vector<ShopValue> &list, int index)
{
    return list[index];
}

// The first ingredient ( item and amount ) of a craft, or -1 and -1.
MapCoord ShopValues::GetCraftIngredient1(int shop_id, int craft_id)
{
    MapCoord result;
    result.item_id = -1;
    result.amount = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].crafts.size() > 0)
        {
            for (vector<ShopCraftVal>::iterator it = record_list[shop_id - 1].crafts.begin();
                it != record_list[shop_id - 1].crafts.end(); it++)
            {
                if (craft_id == it->id)
                {
                    result.item_id = it->ingredient_item_ids[0];
                    result.amount = it->ingredient_amounts[0];
                    break;
                }
            }
        }
    }
    return result;
}

// The second ingredient ( item and amount ) of a craft, or -1 and -1.
MapCoord ShopValues::GetCraftIngredient2(int shop_id, int craft_id)
{
    MapCoord result;
    result.item_id = -1;
    result.amount = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].crafts.size() > 0)
        {
            for (vector<ShopCraftVal>::iterator it = record_list[shop_id - 1].crafts.begin();
                it != record_list[shop_id - 1].crafts.end(); it++)
            {
                if (craft_id == it->id)
                {
                    result.item_id = it->ingredient_item_ids[1];
                    result.amount = it->ingredient_amounts[1];
                    break;
                }
            }
        }
    }
    return result;
}

// The third ingredient ( item and amount ) of a craft, or -1 and -1.
MapCoord ShopValues::GetCraftIngredient3(int shop_id, int craft_id)
{
    MapCoord result;
    result.item_id = -1;
    result.amount = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].crafts.size() > 0)
        {
            for (vector<ShopCraftVal>::iterator it = record_list[shop_id - 1].crafts.begin();
                it != record_list[shop_id - 1].crafts.end(); it++)
            {
                if (craft_id == it->id)
                {
                    result.item_id = it->ingredient_item_ids[2];
                    result.amount = it->ingredient_amounts[2];
                    break;
                }
            }
        }
    }
    return result;
}

// The fourth ingredient ( item and amount ) of a craft, or -1 and -1.
MapCoord ShopValues::GetCraftIngredient4(int shop_id, int craft_id)
{
    MapCoord result;
    result.item_id = -1;
    result.amount = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].crafts.size() > 0)
        {
            for (vector<ShopCraftVal>::iterator it = record_list[shop_id - 1].crafts.begin();
                it != record_list[shop_id - 1].crafts.end(); it++)
            {
                if (craft_id == it->id)
                {
                    result.item_id = it->ingredient_item_ids[3];
                    result.amount = it->ingredient_amounts[3];
                    break;
                }
            }
        }
    }
    return result;
}

// What the shop asks for amount of the item, or -1 when it does not sell it.
int ShopValues::GetBuyPrice(int shop_id, int item_id, int amount)
{
    int result = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].trades.size() > 0)
        {
            for (vector<ShopItemVal>::iterator it = record_list[shop_id - 1].trades.begin();
                it != record_list[shop_id - 1].trades.end(); it++)
            {
                if (item_id == it->item_id && it->buy_price > 0)
                {
                    result = it->buy_price;
                    result = result * amount;
                    break;
                }
            }
        }
    }
    return result;
}

// What the shop pays for amount of the item, or -1 when it does not buy it.
int ShopValues::GetSellPrice(int shop_id, int item_id, int amount)
{
    int result = -1;
    if (shop_id > 0 && (int)record_list.size() >= shop_id)
    {
        if (record_list[shop_id - 1].trades.size() > 0)
        {
            for (vector<ShopItemVal>::iterator it = record_list[shop_id - 1].trades.begin();
                it != record_list[shop_id - 1].trades.end(); it++)
            {
                if (item_id == it->item_id && it->sell_price > 0)
                {
                    result = it->sell_price;
                    result = result * amount;
                    break;
                }
            }
        }
    }
    return result;
}

// The shop's trades and crafts, as the client gets them when it opens the shop; an id of 0 when there is none.
String ShopValues::BuildOpenData(int behavior_id)
{
    vector<ShopValue>::iterator i;
    vector<ShopItemVal>::iterator trade_iter;
    String result = "";
    for (i = record_list.begin(); i != record_list.end(); i++)
    {
        if (i->id != behavior_id)
            continue;
        result = EncodeNumber(i->id, 2);
        result.Insert(i->name, result.Length() + 1);
        result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
        if (i->trades.size() > 0)
        {
            for (trade_iter = i->trades.begin(); trade_iter != i->trades.end(); trade_iter++)
            {
                result.Insert(EncodeNumber(trade_iter->item_id, 2), result.Length() + 1);
                result.Insert(EncodeNumber(trade_iter->buy_price, 3), result.Length() + 1);
                result.Insert(EncodeNumber(trade_iter->sell_price, 3), result.Length() + 1);
                result.Insert(EncodeNumber(trade_iter->max_amount, 1), result.Length() + 1);
            }
        }
        result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
        if (i->crafts.size() > 0)
        {
            vector<ShopCraftVal>::iterator it;
            for (it = i->crafts.begin(); it != i->crafts.end(); it++)
            {
                result.Insert(EncodeNumber(it->id, 2), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_item_ids[0], 2), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_amounts[0], 1), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_item_ids[1], 2), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_amounts[1], 1), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_item_ids[2], 2), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_amounts[2], 1), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_item_ids[3], 2), result.Length() + 1);
                result.Insert(EncodeNumber(it->ingredient_amounts[3], 1), result.Length() + 1);
            }
        }
    }
    if (result == "")
        result = EncodeNumber(0, 2);
    return result;
}

unsigned int ShopValues::GetCount()
{
    return record_list.size();
}

DEFINE_LEARN_ENCODE_NUMBER(ShopValues)

// Reads a number of up to four bytes; a 254 ends it.
DEFINE_DECODE_NUMBER(ShopValues)
