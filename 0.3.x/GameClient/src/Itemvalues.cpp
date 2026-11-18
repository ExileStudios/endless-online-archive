#include <vcl.h>
#pragma hdrstop

#include "Itemvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
#include "Packetproces.h"
#include <io.h>
#include <stdio.h>
#include "Pubfileheader.h"

#pragma package(smart_init)

ItemValues::ItemValues(String prefix, char keep_parts)
{
    file_prefix = prefix + "dat";
    file_parts = new TStringList();
    keep_file_parts = keep_parts;
    is_downloading = false;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
}

ItemValues::~ItemValues()
{
    delete file_parts;
}

// Reads the checksum and record count from the head of the first item file; a missing file is marked.
void ItemValues::ReadHeader()
{
    int file_handle;
    char *header_bytes;
    int file_length;
    String header_path = file_prefix + "001.eif";
    try
    {
        file_handle = FileOpen(header_path.c_str(), fmOpenRead);
        file_length = FileSeek(file_handle, 0, 2);
        if (file_length < 0)
        {
            header_read = true;
            return;
        }
        FileSeek(file_handle, 0, 0);
        header_bytes = new char[11];
        FileRead(file_handle, header_bytes, 10);
        FileClose(file_handle);
        data = header_bytes;
        data.SetLength(file_length);
        delete[] header_bytes;
        if (data.Length() < 10)
            return;
        if (data[1] != 'E' || data[2] != 'I' || data[3] != 'F')
            return;
        read_position = 4;
        checksum_high = DecodeNumber(2);
        checksum_low = DecodeNumber(2);
        record_count = DecodeNumber(2);
    }
    catch (...)
    {
        checksum_high = 0;
        checksum_low = 0;
        record_count = 0;
    }
}

// Writes one part of the item file as the server sent it.
void ItemValues::SaveFilePart(int part_number, String data)
{
    String lp_file_name = file_prefix;
    if (part_number < 10)
        lp_file_name = lp_file_name + "00" + IntToStr(part_number) + ".eif";
    else
        lp_file_name = lp_file_name + "0" + IntToStr(part_number) + ".eif";
    basic_ofstream<char, char_traits<char> > handle;
    handle.open(lp_file_name.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Loads the item records from the numbered .eif files, checking each file's mark and reading up to 900 records from
// each, until the count the headers give is read.
void ItemValues::Load()
{
    int file_handle;
    int part_length;
    short file_number;
    char *buffer;
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        ClearFileParts();
        file_number = 0;
        records_read = 0;
        records_expected = 0;
        // The file comes in parts numbered from 001, read until the record count of the header.
        do
        {
            file_number++;
            if (file_number < 10)
                file_name = "00" + IntToStr(file_number) + ".eif";
            else
                file_name = "0" + IntToStr(file_number) + ".eif";
            file_path = file_prefix + file_name;
            try
            {
                file_handle = FileOpen(file_path.c_str(), fmOpenRead);
                part_length = FileSeek(file_handle, 0, 2);
                if (part_length < 0)
                {
                    header_read = true;
                    return;
                }
                FileSeek(file_handle, 0, 0);
                buffer = new char[part_length + 1];
                FileRead(file_handle, buffer, part_length);
                FileClose(file_handle);
                data = buffer;
                data.SetLength(part_length);
                delete[] buffer;
                if (data.Length() < 14)
                    return;
                if (data[1] != 'E' || data[2] != 'I' || data[3] != 'F')
                    return;
                if (keep_file_parts)
                    file_parts->Add(data);
                read_position = 4;
                PubfileHeader *header = new PubfileHeader(file_name);
                header->file_length = data.Length();
                header->checksum_high = DecodeNumber(2);
                header->checksum_low = DecodeNumber(2);
                header->record_count = DecodeNumber(2);
                header->version = ReadByte();
                headers.insert(headers.end(), header);
                if (!records_read)
                    records_expected = header->record_count;
                int i;
                for (i = 0; records_read < records_expected && i < 900; i++)
                {
                    unsigned char name_length = ReadByte();
                    if (name_length > 50)
                    {
                        has_bad_record = true;
                        header->load_state = 3;
                    }
                    String item_name = data.SubString(read_position, name_length);
                    ItemValue *item = new ItemValue(Count() + 1);
                    item->name = item_name;
                    read_position += name_length;
                    item->graphic = DecodeNumber(2);
                    item->type = ReadByte();
                    item->subtype = ReadByte();
                    item->special = ReadByte();
                    item->hp = DecodeNumber(2);
                    item->tp = DecodeNumber(2);
                    item->min_damage = DecodeNumber(2);
                    item->max_damage = DecodeNumber(2);
                    item->accuracy = DecodeNumber(2);
                    item->evade = DecodeNumber(2);
                    item->armor = DecodeNumber(2);
                    item->return_damage = (unsigned char)ReadByte();
                    item->bonus_str = ReadByte();
                    item->bonus_int = ReadByte();
                    item->bonus_wis = ReadByte();
                    item->bonus_agi = ReadByte();
                    item->bonus_con = ReadByte();
                    item->bonus_cha = ReadByte();
                    item->light_resist = ReadByte();
                    item->dark_resist = ReadByte();
                    item->earth_resist = ReadByte();
                    item->air_resist = ReadByte();
                    item->water_resist = ReadByte();
                    item->fire_resist = ReadByte();
                    item->spec_value = DecodeNumber(3);
                    item->gender = ReadByte();
                    item->spec3 = ReadByte();
                    item->required_level = DecodeNumber(2);
                    item->required_class = DecodeNumber(2);
                    item->required_str = DecodeNumber(2);
                    item->required_int = DecodeNumber(2);
                    item->required_wis = DecodeNumber(2);
                    item->required_agi = DecodeNumber(2);
                    item->required_con = DecodeNumber(2);
                    item->required_cha = DecodeNumber(2);
                    item->element = ReadByte();
                    item->element_power = ReadByte();
                    item->weight = ReadByte();
                    item->spare_byte = ReadByte();
                    item->size = ReadByte();
                    records.insert(records.end(), item);
                    header->records_read++;
                    records_read++;
                    if (read_position - 1 == data.Length())
                    {
                        if (header->load_state != 3)
                            header->load_state = 0;
                        break;
                    }
                }
                data = "";
            }
            catch (...)
            {
                is_loaded = false;
                return;
            }
        } while (records_read < records_expected);
        is_loaded = true;
    }
}

// Deletes the item part files from 001 up to the first one missing, 100 at most.
void ItemValues::DeleteFiles()
{
    char file_found = true;
    String lp_file_name;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lp_file_name = file_prefix;
            if (i < 10)
                lp_file_name = lp_file_name + "00" + IntToStr(i) + ".eif";
            else
                lp_file_name = lp_file_name + "0" + IntToStr(i) + ".eif";
            if (!access(lp_file_name.c_str(), 0))
                remove(lp_file_name.c_str());
            else
                file_found = false;
        }
    }
    catch (...)
    {
    }
}

void Itemvalues_UnusedLiterals1()
{
    "dat";
}

// Deletes every item record.
void ItemValues::Clear()
{
    for (vector<ItemValue *>::iterator it = records.begin(); it != records.end(); it++)
        delete *it;
    records.clear();
}

// The item record at the index, kept within the list.
ItemValue *ItemValues::GetItem(int index)
{
    if (index < 0)
        index = 0;
    if (records.size() <= index)
        index = records.size() - 1;
    return records[index];
}

// The item's weight, at least 0; 0 when the id is not in the list.
int ItemValues::GetWeight(int item_id)
{
    int weight = 0;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
    {
        weight = 0;
        weight = records[item_id - 1]->weight;
        if (weight < 0)
            weight = 0;
    }
    return weight;
}

// The gender an item is made for, -1 when the id is not in the list.
int ItemValues::GetGender(int item_id)
{
    int gender = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
    {
        gender = 0;
        gender = records[item_id - 1]->gender;
        if (gender < 0)
            gender = 0;
    }
    return gender;
}

int ItemValues::GetGraphic(int item_id)
{
    int graphic = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        graphic = records[item_id - 1]->graphic;
    return graphic;
}

int ItemValues::GetSize(int item_id)
{
    int size = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        size = records[item_id - 1]->size;
    return size;
}

int ItemValues::GetSpecial(int item_id)
{
    int special = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        special = records[item_id - 1]->special;
    return special;
}

int ItemValues::GetType(int item_id)
{
    int type = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        type = records[item_id - 1]->type;
    return type;
}

int ItemValues::GetSubType(int item_id)
{
    int subtype = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        subtype = records[item_id - 1]->subtype;
    return subtype;
}

int ItemValues::GetClassRequirement(int item_id)
{
    int required_class = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        required_class = records[item_id - 1]->required_class;
    return required_class;
}

int ItemValues::GetSpecValue(int item_id)
{
    int spec_value = -1;
    if (item_id > 0 && (unsigned int)Count() >= item_id)
        spec_value = records[item_id - 1]->spec_value;
    return spec_value;
}

// The name of the key item with the key number; empty when there is none.
String ItemValues::GetKeyName(int key_id)
{
    String name;
    for (vector<ItemValue *>::iterator it = records.begin(); it != records.end(); it++)
    {
        if ((*it)->type == EifItemType_Key && key_id == (*it)->spec_value)
        {
            name = (*it)->name;
            break;
        }
    }
    return name;
}

String ItemValues::GetName(int item_id)
{
    String name;
    for (vector<ItemValue *>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (item_id == (*it)->item_id)
        {
            name = (*it)->name;
            break;
        }
    }
    return name;
}

int ItemValues::Count()
{
    return records.size();
}

DEFINE_CLEAR_HEADERS(ItemValues)

void ItemValues::ClearFileParts()
{
    file_parts->Clear();
}

DEFINE_READ_BYTE(ItemValues)

DEFINE_DECODE_NUMBER(ItemValues)
