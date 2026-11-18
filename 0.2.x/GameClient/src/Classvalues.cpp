#include <vcl.h>
#pragma hdrstop

#include "Classvalues.h"
#include "Packetcontrols.h"
#include <io.h>
#include <stdio.h>
#include "Pubfileheader.h"

#pragma package(smart_init)

ClassValues::ClassValues(String path)
{
    file_prefix = path + "dat";
    unused_value = -1;
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

// Reads the checksum and record count from the head of the first class file; a missing file is marked.
void ClassValues::ReadHeader()
{
    int handle;
    char *header_bytes;
    int file_length;
    String path = file_prefix + "001.ecf";
    try
    {
        handle = FileOpen(path.c_str(), fmOpenRead);
        file_length = FileSeek(handle, 0, 2);
        if (file_length < 0)
        {
            file_missing = true;
            return;
        }
        FileSeek(handle, 0, 0);
        header_bytes = new char[11];
        FileRead(handle, header_bytes, 10);
        FileClose(handle);
        file_text = header_bytes;
        file_text.SetLength(file_length);
        delete[] header_bytes;
        if (file_text.Length() < 10)
            return;
        // "ECF".
        if (file_text[1] != 'E' || file_text[2] != 'C' || file_text[3] != 'F')
            return;
        read_position = 4;
        checksum_high = DecodeNumber(2);
        checksum_low = DecodeNumber(2);
        header_record_count = DecodeNumber(2);
    }
    catch (...)
    {
        checksum_high = 0;
        checksum_low = 0;
        header_record_count = 0;
    }
}

// Writes one part of the class file as the server sent it.
void ClassValues::SaveFilePart(int part_number, String data)
{
    String lp_file_name = file_prefix;
    if (part_number < 10)
        lp_file_name = lp_file_name + "00" + IntToStr(part_number) + ".ecf";
    else
        lp_file_name = lp_file_name + "0" + IntToStr(part_number) + ".ecf";
    basic_ofstream<char, char_traits<char> > handle;
    handle.open(lp_file_name.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Loads the class records from the numbered .ecf files, checking each file's mark and reading up to 900 records from
// each, until the count the headers give is read.
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
                read_position = 4;
                PubfileHeader *header = new PubfileHeader(part_file_name);
                header->file_length = file_text.Length();
                header->checksum_high = DecodeNumber(2);
                header->checksum_low = DecodeNumber(2);
                header->record_count = DecodeNumber(2);
                header->version = ReadByte();
                part_headers.insert(part_headers.end(), header);
                if (!records_read)
                    record_count = header->record_count;
                int part_record;
                for (part_record = 0; records_read < record_count && part_record < 900; part_record++)
                {
                    unsigned char name_length = ReadByte();
                    if (name_length > 50)
                    {
                        has_long_name = true;
                        header->load_state = 3;
                    }
                    String name = file_text.SubString(read_position, name_length);
                    ClassValue record(Count() + 1);
                    record.name = name;
                    read_position += name_length;
                    record.parent_class_id = ReadByte();
                    record.stat_group = ReadByte();
                    record.strength = DecodeNumber(2);
                    record.intelligence = DecodeNumber(2);
                    record.wisdom = DecodeNumber(2);
                    record.agility = DecodeNumber(2);
                    record.constitution = DecodeNumber(2);
                    record.charisma = DecodeNumber(2);
                    records.insert(records.end(), record);
                    header->records_read++;
                    records_read++;
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

// Deletes the class part files from 001 up to the first one missing, 100 at most.
void ClassValues::DeleteFiles()
{
    char file_found = true;
    String lp_file_name;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lp_file_name = file_prefix;
            if (i < 10)
                lp_file_name = lp_file_name + "00" + IntToStr(i) + ".ecf";
            else
                lp_file_name = lp_file_name + "0" + IntToStr(i) + ".ecf";
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

DEFINE_CLEAR(ClassValues)

String ClassValues::GetName(int class_id)
{
    String name;
    for (vector<ClassValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (class_id == it->class_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

// True when the class is the ancestor or comes from it through its parent classes; any class for ancestor 0.
bool ClassValues::IsDescendantOf(int class_id, int ancestor_id)
{
    if (!ancestor_id)
        return true;
    for (; class_id > 0; class_id = records[class_id - 1].parent_class_id)
    {
        if (class_id == ancestor_id)
            return true;
    }
    return false;
}

unsigned int ClassValues::Count()
{
    return records.size();
}

// Deletes the part headers.
void ClassValues::ClearHeaders()
{
    for (int i = 0; i < part_headers.size(); i++)
        delete part_headers[i];
    part_headers.clear();
}

// Reads one byte at the read position, stored plus one; a value past 252 reads as 0.
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

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
int ClassValues::DecodeNumber(unsigned char encoded)
{
    int digit;
    int number = 0;
    try
    {
        for (int i = 0; i < encoded; i++)
        {
            unsigned char raw_byte = file_text[read_position + i];
            if (raw_byte == 254)
                break;
            digit = raw_byte;
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
    read_position += encoded;
    return number;
}
