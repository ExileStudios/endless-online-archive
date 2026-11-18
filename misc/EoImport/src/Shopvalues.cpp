#include <vcl.h>
#pragma hdrstop

#include "Shopvalues.h"
#include <Classes.hpp>
#include "Learnvalues.h"
#include "Npcvalues.h"

#pragma package(smart_init)

ShopValues::ShopValues(String prefix)
{
    file_prefix = prefix;
    record_index = -1;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

ShopValues::~ShopValues()
{
}

// Reads shops.ini: each name key starts a shop, trade lines add items by threes and craft lines by nines.
// Returns the shops read, or -1 when the file has no lines.
int ShopValues::ImportIni(int format, String path)
{
    int ingredient_amount_1;
    int buy_price;
    int ingredient_item_id_1;
    int sell_price;
    int ingredient_amount_4;
    int unused_value;
    int second_unused_value;
    int third_unused_value;
    TStringList *lines = new TStringList();
    try
    {
        lines->LoadFromFile(path);
    }
    catch (...)
    {
    }
    if (lines->Count <= 0)
        return -1;
    int entries_read = 0;
    int shop_count = 0;
    for (int line_index = 0; lines->Count > line_index; line_index++)
    {
        String line = lines->Strings[line_index];
        if (line.Length() < 1)
            continue;
        if (line[1] == '#')
            continue;
        int equals_pos = line.Pos("=");
        if (equals_pos <= 0)
            continue;
        String key = line.SubString(1, equals_pos - 1);
        value_text = line.SubString(equals_pos + 1, line.Length() - equals_pos);
        int unused_count = 0;
        int name_pos = key.Pos(".name");
        if (name_pos > 0)
        {
            String id_part = key.SubString(1, name_pos - 1);
            int behavior_id = ParseNumber(id_part);
            while (value_text.Length() > 0)
            {
                if (value_text[1] != ' ')
                    break;
                value_text.Delete(1, 1);
            }
            while (value_text.Length() > 0)
            {
                if (value_text[value_text.Length()] != ' ')
                    break;
                value_text.Delete(value_text.Length(), 1);
            }
            // A new shop serves every level up to 252 and every class.
            ShopValue shop(Count() + 1);
            shop.behavior_id = behavior_id;
            shop.name = value_text;
            shop.min_level = 0;
            shop.max_level = 252;
            shop.class_requirement = 0;
            records.insert(records.end(), shop);
            records_read++;
            entries_read++;
            shop_count++;
            continue;
        }
        int trade_pos = key.Pos(".trade");
        if (trade_pos > 0)
        {
            String trade_id_part = key.SubString(1, trade_pos - 1);
            int trade_shop_id[1];
            trade_shop_id[0] = ParseNumber(trade_id_part);
            int max_amount = 4;
            while (value_text.Length() > 0)
            {
                int item_id = ReadNextNumber();
                buy_price = ReadNextNumber();
                sell_price = ReadNextNumber();
                if (item_id == -1 || sell_price == -1 || buy_price == -1)
                    break;
                records[shop_count - 1].AddTrade(item_id, sell_price, buy_price, max_amount);
            }
            continue;
        }
        int craft_pos = key.Pos(".craft");
        if (craft_pos > 0)
        {
            String craft_id_part = key.SubString(1, craft_pos - 1);
            int craft_shop_id[1];
            craft_shop_id[0] = ParseNumber(craft_id_part);
            while (value_text.Length() > 0)
            {
                int craft_item_id = ReadNextNumber();
                ingredient_item_id_1 = ReadNextNumber();
                ingredient_amount_1 = ReadNextNumber();
                int ingredient_item_id_2 = ReadNextNumber();
                int ingredient_amount_2 = ReadNextNumber();
                int ingredient_item_id_3 = ReadNextNumber();
                int ingredient_amount_3 = ReadNextNumber();
                int ingredient_item_id_4 = ReadNextNumber();
                ingredient_amount_4 = ReadNextNumber();
                if (craft_item_id == -1 || ingredient_item_id_1 == -1 || ingredient_amount_1 == -1 ||
                    ingredient_item_id_2 == -1 || ingredient_amount_2 == -1 || ingredient_item_id_3 == -1 ||
                    ingredient_amount_3 == -1 || ingredient_item_id_4 == -1 || ingredient_amount_4 == -1)
                    break;
                records[shop_count - 1].AddCraft(craft_item_id, ingredient_item_id_1, ingredient_amount_1,
                    ingredient_item_id_2, ingredient_amount_2, ingredient_item_id_3, ingredient_amount_3,
                    ingredient_item_id_4, ingredient_amount_4);
            }
            continue;
        }
    }
    return entries_read;
}

int CopyShopItems(int first, int last, int dest)
{
    for (; first != last; dest = dest + 16)
    {
        *(NpcDropItem *)dest = *(NpcDropItem *)first;
        first += 16;
    }
    return dest;
}

// Takes the next number of a comma list off the value text; -1 when none is left.
DEFINE_READ_NEXT_NUMBER(ShopValues)

// The digits of the text as a number; 0 when it has none.
DEFINE_PARSE_NUMBER(ShopValues)

void Shopvalues_UnusedLiterals1()
{
    "";
    "serv_shops.epf";
    "";
    "";
}

ShopValue *ShopValues::GetShop(int index)
{
    return &records[index];
}

int ShopValues::Count()
{
    return records.size();
}
