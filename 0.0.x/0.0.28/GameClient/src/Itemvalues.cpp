#include <vcl.h>
#pragma hdrstop

#include "Itemvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
#include "Packetproces.h"
#include <fstream.h>
#include <io.h>
#include <stdio.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

ItemValues::ItemValues()
{
    file_prefix = "./pub/dat";
    unread_int_at_64 = -1;
    is_downloading = 0;
    is_loaded = 0;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
}

ItemValues::~ItemValues()
{
}

// Reads the checksum and item count from the header of the first item file.
// With more than 900 items the last part file is read too; the values are kept only when its header matches.
void ItemValues::ReadHeader()
{
    String file_text;
    int first_checksum_high;
    int first_checksum_low;
    int file_handle;
    DWORD file_size;
    char *buffer;
    int total_records;
    String file_name = file_prefix + "001.eif";
    try
    {
        file_handle = FileOpen(file_name.c_str(), 0);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
        // "EIF".
        if (file_text[1] != 'E' || file_text[2] != 'I' || file_text[3] != 'F')
            return;
        first_checksum_high = DecodeNumber(file_text.SubString(4, 2));
        first_checksum_low = DecodeNumber(file_text.SubString(6, 2));
        total_records = DecodeNumber(file_text.SubString(8, 2));
    }
    catch (...)
    {
        first_checksum_high = 0;
        first_checksum_low = 0;
        total_records = 0;
    }
    if (total_records > 900)
    {
        file_text = "";
        file_name = file_prefix;
        int last_file_number = total_records / PUB_RECORDS_PER_FILE + 1;
        if (last_file_number < 10)
            file_name = file_name + "00" + IntToStr(last_file_number) + ".eif";
        else
            file_name = file_name + "0" + IntToStr(last_file_number) + ".eif";
        try
        {
            file_handle = FileOpen(file_name.c_str(), 0);
            file_size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            buffer = new char[file_size + 1];
            FileRead(file_handle, buffer, file_size);
            FileClose(file_handle);
            file_text = buffer;
            file_text.SetLength(file_size);
            delete[] buffer;
            if (file_text[1] != 'E' || file_text[2] != 'I' || file_text[3] != 'F')
            {
                checksum_high = 0;
                checksum_low = 0;
                record_count = 0;
            }
            else
            {
                int last_checksum_high = DecodeNumber(file_text.SubString(4, 2));
                int last_checksum_low = DecodeNumber(file_text.SubString(6, 2));
                int last_total_records = DecodeNumber(file_text.SubString(8, 2));
                if (last_checksum_high == first_checksum_high && last_checksum_low == first_checksum_low &&
                    last_total_records == total_records)
                {
                    checksum_high = first_checksum_high;
                    checksum_low = first_checksum_low;
                    record_count = total_records;
                }
            }
        }
        catch (...)
        {
            checksum_high = 0;
            checksum_low = 0;
            record_count = 0;
        }
    }
    else
    {
        checksum_high = first_checksum_high;
        checksum_low = first_checksum_low;
        record_count = total_records;
    }
}

// Writes one part of the item file as the server sent it.
void ItemValues::SaveFilePart(int part_number, String data)
{
    String lpFileName = file_prefix;
    if (part_number < 10)
        lpFileName = lpFileName + "00" + IntToStr(part_number) + ".eif";
    else
        lpFileName = lpFileName + "0" + IntToStr(part_number) + ".eif";
    std::ofstream handle;
    handle.open(lpFileName.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Reads every item part file into the list, once until the files change.
void ItemValues::Load()
{
    if (!is_loaded)
    {
        Clear();
        int file_number = 1;
        int loaded_count = 0;
        int record_total = 1;
        int hFile;
        do
        {
            String file_name;
            String file_text;
            file_name.Insert(file_prefix, 0);
            if (file_number < 10)
                file_name = file_name + "00" + IntToStr(file_number) + ".eif";
            else
                file_name = file_name + "0" + IntToStr(file_number) + ".eif";
            try
            {
                DWORD file_size;
                char *buffer;
                hFile = FileOpen(file_name.c_str(), 0);
                file_size = FileSeek(hFile, 0, 2);
                FileSeek(hFile, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(hFile, buffer, file_size);
                FileClose(hFile);
                file_text = buffer;
                file_text.SetLength(file_size);
                delete[] buffer;
                if (file_text[1] != 'E' || file_text[2] != 'I' || file_text[3] != 'F')
                    return;
                if (file_number == 1)
                {
                    int header_checksum_high = DecodeNumber(file_text.SubString(4, 2));
                    int header_checksum_low = DecodeNumber(file_text.SubString(6, 2));
                    int header_count = DecodeNumber(file_text.SubString(8, 2));
                    int header_byte_at = DecodeNumber(file_text.SubString(10, 1));
                    record_total = header_count;
                }
                file_text.Delete(1, 10);
                for (int i = 0; loaded_count < record_total && i < 900; i++)
                {
                    // Each item: a length byte, the name, then 58 bytes of numbers.
                    int name_end = DecodeNumber(file_text.SubString(1, 1)) + 1;
                    AddItem(Count() + 1, file_text.SubString(2, name_end - 1),
                        DecodeNumber(file_text.SubString(name_end + 1, 2)),
                        DecodeNumber(file_text.SubString(name_end + 3, 1)),
                        DecodeNumber(file_text.SubString(name_end + 4, 1)),
                        DecodeNumber(file_text.SubString(name_end + 5, 1)),
                        DecodeNumber(file_text.SubString(name_end + 6, 2)),
                        DecodeNumber(file_text.SubString(name_end + 8, 2)),
                        DecodeNumber(file_text.SubString(name_end + 10, 2)),
                        DecodeNumber(file_text.SubString(name_end + 12, 2)),
                        DecodeNumber(file_text.SubString(name_end + 14, 2)),
                        DecodeNumber(file_text.SubString(name_end + 16, 2)),
                        DecodeNumber(file_text.SubString(name_end + 18, 2)),
                        DecodeNumber(file_text.SubString(name_end + 20, 1)),
                        DecodeNumber(file_text.SubString(name_end + 21, 1)),
                        DecodeNumber(file_text.SubString(name_end + 22, 1)),
                        DecodeNumber(file_text.SubString(name_end + 23, 1)),
                        DecodeNumber(file_text.SubString(name_end + 24, 1)),
                        DecodeNumber(file_text.SubString(name_end + 25, 1)),
                        DecodeNumber(file_text.SubString(name_end + 26, 1)),
                        DecodeNumber(file_text.SubString(name_end + 27, 1)),
                        DecodeNumber(file_text.SubString(name_end + 28, 1)),
                        DecodeNumber(file_text.SubString(name_end + 29, 1)),
                        DecodeNumber(file_text.SubString(name_end + 30, 1)),
                        DecodeNumber(file_text.SubString(name_end + 31, 1)),
                        DecodeNumber(file_text.SubString(name_end + 32, 1)),
                        DecodeNumber(file_text.SubString(name_end + 33, 3)),
                        DecodeNumber(file_text.SubString(name_end + 36, 1)),
                        DecodeNumber(file_text.SubString(name_end + 37, 1)),
                        DecodeNumber(file_text.SubString(name_end + 38, 2)),
                        DecodeNumber(file_text.SubString(name_end + 40, 2)),
                        DecodeNumber(file_text.SubString(name_end + 41, 2)),
                        DecodeNumber(file_text.SubString(name_end + 44, 2)),
                        DecodeNumber(file_text.SubString(name_end + 46, 2)),
                        DecodeNumber(file_text.SubString(name_end + 48, 2)),
                        DecodeNumber(file_text.SubString(name_end + 50, 2)),
                        DecodeNumber(file_text.SubString(name_end + 52, 2)),
                        DecodeNumber(file_text.SubString(name_end + 54, 1)),
                        DecodeNumber(file_text.SubString(name_end + 55, 1)),
                        DecodeNumber(file_text.SubString(name_end + 56, 1)),
                        DecodeNumber(file_text.SubString(name_end + 57, 1)),
                        DecodeNumber(file_text.SubString(name_end + 58, 1)));
                    loaded_count++;
                    file_text.Delete(1, name_end + 58);
                }
            }
            catch (...)
            {
                FileClose(hFile);
                is_loaded = 0;
            }
            file_number++;
        } while (loaded_count < record_total);
        is_loaded = 1;
    }
}

// Deletes the item part files from 001 up to the first one missing, 100 at most.
void ItemValues::DeleteFiles()
{
    char file_found = 1;
    String lpFileName;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lpFileName = file_prefix;
            if (i < 10)
                lpFileName = lpFileName + "00" + IntToStr(i) + ".eif";
            else
                lpFileName = lpFileName + "0" + IntToStr(i) + ".eif";
            if (!access(lpFileName.c_str(), 0))
                remove(lpFileName.c_str());
            else
                file_found = 0;
        }
    }
    catch (...)
    {
    }
}

void ItemValues::AddItem(int item_id, String name, short graphic, short type, short subtype, short special, short item_record_word_at_6, short item_record_word_at_8, short item_record_word_at_10,
    short item_record_word_at_12, short item_record_word_at_14, short item_record_word_at_16, short item_record_word_at_18, short item_record_byte_at_20, short item_record_byte_at_21, short item_record_byte_at_22, short item_record_byte_at_23, short item_record_byte_at_24, short item_record_byte_at_25,
    short item_record_byte_at_26, short item_record_byte_at_27, short item_record_byte_at_28, short item_record_byte_at_29, short item_record_byte_at_30, short item_record_byte_at_31, short item_record_byte_at_32, int spec_value, short gender, short item_record_byte_at_37,
    short required_level, short required_class, short required_str, short required_int, short required_wis, short required_agi, short required_con, short required_cha, short item_record_byte_at_54, short item_record_byte_at_55,
    short weight, short item_record_byte_at_57, short size)
{
    ItemValue handle(item_id);
    handle.name = name;
    handle.graphic = graphic;
    handle.type = type;
    handle.subtype = subtype;
    handle.special = special;
    handle.item_record_word_at_6 = item_record_word_at_6;
    handle.item_record_word_at_8 = item_record_word_at_8;
    handle.item_record_word_at_10 = item_record_word_at_10;
    handle.item_record_word_at_12 = item_record_word_at_12;
    handle.item_record_word_at_14 = item_record_word_at_14;
    handle.item_record_word_at_16 = item_record_word_at_16;
    handle.item_record_word_at_18 = item_record_word_at_18;
    handle.item_record_byte_at_20 = item_record_byte_at_20;
    handle.item_record_byte_at_21 = item_record_byte_at_21;
    handle.item_record_byte_at_22 = item_record_byte_at_22;
    handle.item_record_byte_at_23 = item_record_byte_at_23;
    handle.item_record_byte_at_24 = item_record_byte_at_24;
    handle.item_record_byte_at_25 = item_record_byte_at_25;
    handle.item_record_byte_at_26 = item_record_byte_at_26;
    handle.item_record_byte_at_27 = item_record_byte_at_27;
    handle.item_record_byte_at_28 = item_record_byte_at_28;
    handle.item_record_byte_at_29 = item_record_byte_at_29;
    handle.item_record_byte_at_30 = item_record_byte_at_30;
    handle.item_record_byte_at_31 = item_record_byte_at_31;
    handle.item_record_byte_at_32 = item_record_byte_at_32;
    handle.spec_value = spec_value;
    handle.gender = gender;
    handle.item_record_byte_at_37 = item_record_byte_at_37;
    handle.required_level = required_level;
    handle.required_class = required_class;
    handle.required_str = required_str;
    handle.required_int = required_int;
    handle.required_wis = required_wis;
    handle.required_agi = required_agi;
    handle.required_con = required_con;
    handle.required_cha = required_cha;
    handle.size = size;
    handle.item_record_byte_at_57 = item_record_byte_at_57;
    handle.weight = weight;
    handle.item_record_byte_at_54 = item_record_byte_at_54;
    handle.item_record_byte_at_55 = item_record_byte_at_55;
    records.insert(records.end(), handle);
}

DEFINE_CLEAR(ItemValues)

// The record of an item id; ids start at 1.
ItemValue ItemValues::GetItem(int item_id)
{
    return records[item_id - 1];
}

// The weight of an item, 0 when the id is not in the list.
int ItemValues::GetWeight(int item_id)
{
    int weight = 0;
    if (item_id > 0 && Count() > (unsigned int)item_id)
    {
        weight = records[item_id - 1].weight;
        if (weight < 0)
            weight = 0;
    }
    return weight;
}

// The gender an item is made for, -1 when the id is not in the list.
int ItemValues::GetGender(int item_id)
{
    int gender = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
    {
        gender = records[item_id - 1].gender;
        if (gender < 0)
            gender = 0;
    }
    return gender;
}

int ItemValues::GetGraphic(int item_id)
{
    int graphic = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        graphic = records[item_id - 1].graphic;
    return graphic;
}

int ItemValues::GetSize(int item_id)
{
    int size = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        size = records[item_id - 1].size;
    return size;
}

int ItemValues::GetSpecial(int item_id)
{
    int special = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        special = records[item_id - 1].special;
    return special;
}

int ItemValues::GetType(int item_id)
{
    int type = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        type = records[item_id - 1].type;
    return type;
}

int ItemValues::GetSubType(int item_id)
{
    int subtype = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        subtype = records[item_id - 1].subtype;
    return subtype;
}

int ItemValues::GetClassRequirement(int item_id)
{
    int required_class = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        required_class = records[item_id - 1].required_class;
    return required_class;
}

int ItemValues::GetSpecValue(int item_id)
{
    int spec_value = -1;
    if (item_id > 0 && Count() > (unsigned int)item_id)
        spec_value = records[item_id - 1].spec_value;
    return spec_value;
}

// The name of the key item (type 9) that opens lock key_id, or "" when there is none.
String ItemValues::GetKeyName(int key_id)
{
    String name;
    for (vector<ItemValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (it->type == 9 && key_id == it->spec_value)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

String ItemValues::GetName(int item_id)
{
    String name;
    for (vector<ItemValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (item_id == it->item_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

unsigned int ItemValues::Count()
{
    return records.size();
}

DEFINE_DECODE_NUMBER(ItemValues)
