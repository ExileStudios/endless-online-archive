#include <vcl.h>
#pragma hdrstop

#include "Shopvalues.h"
#include "Pubfileheader.h"
#include "Shopvalue.h"

#pragma package(smart_init)

ShopValues::ShopValues(String prefix)
{
    file_prefix = prefix;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

ShopValues::~ShopValues()
{
}

// Loads the shops from serv_shops.epf: each shop's levels and class, what it trades and what it crafts.
void ShopValues::Load()
{
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        records_read = 0;
        record_count = 0;
        int handle;
        unsigned char name_length;
        int j;
        int file_length;
        char *buffer;
        int buy_price;
        file_name = "serv_shops.epf";
        file_path = file_prefix + file_name;
        try
        {
            handle = FileOpen(file_path.c_str(), fmOpenRead);
            file_length = FileSeek(handle, 0, 2);
            if (file_length < 0)
                return;
            FileSeek(handle, 0, 0);
            buffer = new char[file_length + 1];
            FileRead(handle, buffer, file_length);
            FileClose(handle);
            data = buffer;
            data.SetLength(file_length);
            delete[] buffer;
            if (data.Length() < 14)
                return;
            if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
                return;
            read_position = 4;
            PubfileHeader *header = new PubfileHeader(file_name);
            header->file_length = data.Length();
            header->checksum_high = ReadNumber(2);
            header->checksum_low = ReadNumber(2);
            header->record_count = ReadNumber(2);
            header->version = ReadByte();
            headers.insert(headers.end(), header);
            if (!records_read)
                record_count = header->record_count;
            do
            {
                int behavior_id = ReadNumber(2);
                name_length = ReadByte();
                if (name_length > 50)
                {
                    has_bad_record = true;
                    header->load_state = 3;
                }
                ShopValue *shop = new ShopValue(Count() + 1);
                shop->name = data.SubString(read_position, name_length);
                read_position += name_length;
                shop->behavior_id = behavior_id;
                shop->min_level = ReadNumber(2);
                shop->max_level = ReadNumber(2);
                shop->class_requirement = (unsigned char)ReadByte();
                short trade_count = ReadNumber(2);
                unsigned char craft_count = ReadByte();
                for (int i = 0; i < trade_count; i++)
                {
                    int read_item_id;
                    read_item_id = ReadNumber(2);
                    int item_id = read_item_id;
                    buy_price = ReadNumber(3);
                    int sell_price = ReadNumber(3);
                    unsigned char max_amount = ReadByte();
                    shop->AddTrade(item_id, buy_price, sell_price, max_amount);
                }
                for (j = 0; j < craft_count; j++)
                {
                    int craft_item_id = ReadNumber(2);
                    int ingredient_item_id_1 = ReadNumber(2);
                    unsigned char ingredient_amount_1 = ReadByte();
                    int ingredient_item_id_2 = ReadNumber(2);
                    unsigned char ingredient_amount_2 = ReadByte();
                    int ingredient_item_id_3 = ReadNumber(2);
                    unsigned char ingredient_amount_3 = ReadByte();
                    int ingredient_item_id_4 = ReadNumber(2);
                    shop->AddCraft(craft_item_id, ingredient_item_id_1, ingredient_amount_1, ingredient_item_id_2,
                        ingredient_amount_2, ingredient_item_id_3, ingredient_amount_3, ingredient_item_id_4,
                        (unsigned char)ReadByte());
                }
                records.insert(records.end(), shop);
                header->records_read++;
                records_read++;
                if (read_position - 1 == data.Length())
                {
                    if (header->load_state != 3)
                        header->load_state = 0;
                    break;
                }
            } while ((unsigned int)(read_position + 9) < data.Length());
            data = "";
            is_loaded = true;
        }
        catch (...)
        {
            is_loaded = false;
            return;
        }
    }
}

// Reads the shops again from the folder given.
DEFINE_LOAD_FROM(ShopValues)

// Deletes every shop with its trades and crafts.
void ShopValues::Clear()
{
    for (vector<ShopValue *>::iterator record = records.begin(); record != records.end(); record++)
    {
        (*record)->trades.clear();
        (*record)->crafts.clear();
        delete *record;
    }
    records.clear();
}

ShopValue *ShopValues::GetShop(int index)
{
    if (index < 0)
        return 0;
    if (records.size() <= index)
        return 0;
    return records[index];
}

int ShopValues::Count()
{
    return records.size();
}

char ShopValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *ShopValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(ShopValues)

void ShopValues::InsertShop(int index)
{
    ShopValue *shop = new ShopValue(index + 1);
    records.insert(records.begin() + index, shop);
}

void ShopValues::DeleteShop(int index)
{
    delete records[index];
    records.erase(records.begin() + index);
}

DEFINE_READ_BYTE(ShopValues)

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
DEFINE_READ_NUMBER(ShopValues)
