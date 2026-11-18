#include <vcl.h>
#pragma hdrstop

#include "Classvalues.h"
#include "Classvalue.h"
#include "Pubfileheader.h"

#pragma package(smart_init)

ClassValues::ClassValues(String folder, char keep)
{
    file_prefix = folder + "dat";
    kept_files = new TStringList();
    keep_files = keep;
    is_loaded = false;
    has_long_name = false;
    file_missing = false;
    checksum_high = 0;
    checksum_high = 0;
    header_record_count = 0;
}

ClassValues::~ClassValues()
{
}

void Classvalues_UnusedLiterals1()
{
    "001.ecf";
    "";
    "00";
    ".ecf";
    "0";
    ".ecf";
    "";
    "";
}

// Loads the classes from the numbered .ecf files, up to 900 from each, until the count the headers give is read.
void ClassValues::Load()
{
    int handle;
    int file_size;
    short part_number;
    char *buffer;
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        ClearKeptFiles();
        part_number = 0;
        records_read = 0;
        record_count = 0;
        do
        {
            part_number++;
            if (part_number < 10)
                part_file_name = "00" + IntToStr(part_number) + ".ecf";
            else
                part_file_name = "0" + IntToStr(part_number) + ".ecf";
            part_file_path = file_prefix + part_file_name;
            try
            {
                handle = FileOpen(part_file_path.c_str(), fmOpenRead);
                file_size = FileSeek(handle, 0, 2);
                if (file_size < 0)
                {
                    file_missing = true;
                    return;
                }
                FileSeek(handle, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(handle, buffer, file_size);
                FileClose(handle);
                file_text = buffer;
                file_text.SetLength(file_size);
                delete[] buffer;
                if (file_text.Length() < 14)
                    return;
                if (file_text[1] != 'E' || file_text[2] != 'C' || file_text[3] != 'F')
                    return;
                if (keep_files)
                    kept_files->Add(file_text);
                read_position = 4;
                PubfileHeader *header = new PubfileHeader(part_file_name);
                header->file_length = file_text.Length();
                header->checksum_high = ReadNumber(2);
                header->checksum_low = ReadNumber(2);
                header->record_count = ReadNumber(2);
                header->version = ReadByte();
                part_headers.insert(part_headers.end(), header);
                // The first part's header gives the count of records in all the parts.
                if (!records_read)
                    record_count = header->record_count;
                for (int part_record = 0; records_read < record_count && part_record < 900; part_record++)
                {
                    unsigned char name_length = ReadByte();
                    if (name_length > 50)
                    {
                        has_long_name = true;
                        header->load_state = 3;
                    }
                    String name = file_text.SubString(read_position, name_length);
                    ClassValue *record = new ClassValue(Count() + 1);
                    record->name = name;
                    read_position += name_length;
                    record->stat_group = ReadByte();
                    record->parent_class_id = ReadByte();
                    record->strength = ReadNumber(2);
                    record->intelligence = ReadNumber(2);
                    record->wisdom = ReadNumber(2);
                    record->agility = ReadNumber(2);
                    record->constitution = ReadNumber(2);
                    record->charisma = ReadNumber(2);
                    records.insert(records.end(), record);
                    header->records_read++;
                    records_read++;
                    // The end of the part's data also ends its records.
                    if (read_position - 1 == file_text.Length())
                    {
                        if (header->load_state != 3)
                            header->load_state = 0;
                        break;
                    }
                }
            }
            catch (...)
            {
                is_loaded = false;
                return;
            }
        } while (records_read < record_count);
        is_loaded = true;
    }
}

void Classvalues_UnusedLiterals2()
{
    "00";
    ".ecf";
    "0";
    ".ecf";
    "";
    "";
}

// Reads the class file again from the folder given: the prefix of its file names, then a fresh load.
void ClassValues::LoadFrom(String folder)
{
    file_prefix = folder + "dat";
    Clear();
    is_loaded = false;
    has_long_name = false;
    file_missing = false;
    Load();
}

// Deletes every class record.
void ClassValues::Clear()
{
    for (vector<ClassValue *>::iterator record = records.begin(); record != records.end(); record++)
        delete *record;
    records.clear();
}

ClassValue *ClassValues::GetRecord(int index)
{
    if (index < 0)
        index = 0;
    if (records.size() <= index)
        index = records.size() - 1;
    return records[index];
}

int ClassValues::Count()
{
    return records.size();
}

char ClassValues::HeaderCount()
{
    return part_headers.size();
}

PubfileHeader *ClassValues::GetHeader(unsigned char index)
{
    return part_headers[index];
}

void ClassValues::ClearHeaders()
{
    for (int i = 0; i < part_headers.size(); i++)
        delete part_headers[i];
    part_headers.clear();
}

void ClassValues::ClearKeptFiles()
{
    kept_files->Clear();
}

char ClassValues::ReadByte()
{
    unsigned char value;
    try
    {
        value = file_text[read_position];
        value--;
    }
    catch (...)
    {
        value = 0;
    }
    read_position++;
    if (value > 252)
        value = 0;
    return value;
}

int ClassValues::ReadNumber(unsigned char length)
{
    int digit;
    int number = 0;
    try
    {
        for (int i = 0; i < length; i++)
        {
            unsigned char byte = file_text[read_position + i];
            if (byte == 254)
                break;
            digit = byte;
            digit--;
            if (!i)
                number += digit;
            if (i == 1)
                number += 253 * digit;
            if (i == 2)
                number += 64009 * digit;
            if (i == 3)
                number += digit * 0xF71AE5;
        }
    }
    catch (...)
    {
        number = 0;
    }
    read_position += length;
    return number;
}
