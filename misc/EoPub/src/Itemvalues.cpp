#include <vcl.h>
#pragma hdrstop

#include "Itemvalues.h"
#include <fstream>
#include "Itemvalue.h"
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

void Itemvalues_UnusedLiterals1()
{
    std::basic_ofstream<char, std::char_traits<char> > object;
    "001.eif";
    "";
    "00";
    ".eif";
    "0";
    ".eif";
    "";
    "";
}

// Loads the items from the numbered .eif files, up to 900 from each, until the count the headers give is read.
void ItemValues::Load()
{
    int file_handle;
    int file_size;
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
                file_size = FileSeek(file_handle, 0, 2);
                if (file_size < 0)
                {
                    header_read = true;
                    return;
                }
                FileSeek(file_handle, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(file_handle, buffer, file_size);
                FileClose(file_handle);
                data = buffer;
                data.SetLength(file_size);
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
                header->checksum_high = ReadNumber(2);
                header->checksum_low = ReadNumber(2);
                header->record_count = ReadNumber(2);
                header->version = ReadByte();
                headers.insert(headers.end(), header);
                // The first part's header gives the count of records in all the parts.
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
                    item->graphic = ReadNumber(2);
                    item->type = ReadByte();
                    item->subtype = ReadByte();
                    item->special = ReadByte();
                    item->hp = ReadNumber(2);
                    item->tp = ReadNumber(2);
                    item->min_damage = ReadNumber(2);
                    item->max_damage = ReadNumber(2);
                    item->accuracy = ReadNumber(2);
                    item->evade = ReadNumber(2);
                    item->armor = ReadNumber(2);
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
                    item->spec_value = ReadNumber(3);
                    item->gender = ReadByte();
                    item->spec3 = ReadByte();
                    item->required_level = ReadNumber(2);
                    item->required_class = ReadNumber(2);
                    item->required_str = ReadNumber(2);
                    item->required_int = ReadNumber(2);
                    item->required_wis = ReadNumber(2);
                    item->required_agi = ReadNumber(2);
                    item->required_con = ReadNumber(2);
                    item->required_cha = ReadNumber(2);
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

void Itemvalues_UnusedLiterals2()
{
    "00";
    ".eif";
    "0";
    ".eif";
    "";
    "";
}

// Reads the item file again from the folder given: the prefix of its file names, then a fresh load.
void ItemValues::LoadFrom(String folder)
{
    file_prefix = folder + "dat";
    Clear();
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    Load();
}

// Deletes every item record.
void ItemValues::Clear()
{
    for (vector<ItemValue *>::iterator record = records.begin(); record != records.end(); record++)
        delete *record;
    records.clear();
}

ItemValue *ItemValues::GetItem(int index)
{
    if (index < 0)
        index = 0;
    if (records.size() <= index)
        index = records.size() - 1;
    return records[index];
}

int ItemValues::Count()
{
    return records.size();
}

char ItemValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *ItemValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(ItemValues)

void ItemValues::ClearFileParts()
{
    file_parts->Clear();
}

DEFINE_READ_BYTE(ItemValues)

DEFINE_READ_NUMBER(ItemValues)
