#include <vcl.h>
#pragma hdrstop

#include "Classvalues.h"
#include "Packetcontrols.h"
#include <io.h>
#include <stdio.h>

#pragma package(smart_init)

ClassValues::ClassValues()
{
    file_prefix = "./pub/dat";
    unused_value = -1;
    is_loaded = false;
    unused_value = -1;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
}

ClassValues::~ClassValues()
{
}

// Reads the checksum and class count from the header of the first class file.
// With more than 250 classes the last part file is read too; the values are kept only when its header matches.
void ClassValues::ReadHeader()
{
    int file_handle;
    int file_size;
    char *buffer;
    int first_checksum_high;
    int first_checksum_low;
    int total_records;
    String file_text;
    String file_name = file_prefix + "001.ecf";
    try
    {
        file_handle = FileOpen(file_name.c_str(), fmOpenRead);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
        // "ECF".
        if (file_text[1] != 'E' || file_text[2] != 'C' || file_text[3] != 'F')
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
    if (total_records > 250)
    {
        file_text = "";
        file_name = file_prefix;
        int last_file_number = total_records / CLASS_RECORDS_PER_FILE + 1;
        if (last_file_number < 10)
            file_name = file_name + "00" + IntToStr(last_file_number) + ".ecf";
        else
            file_name = file_name + "0" + IntToStr(last_file_number) + ".ecf";
        try
        {
            file_handle = FileOpen(file_name.c_str(), fmOpenRead);
            file_size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            buffer = new char[file_size + 1];
            FileRead(file_handle, buffer, file_size);
            FileClose(file_handle);
            file_text = buffer;
            file_text.SetLength(file_size);
            delete[] buffer;
            if (file_text[1] != 'E' || file_text[2] != 'C' || file_text[3] != 'F')
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

// Reads every class part file into the list, once until the files change.
void ClassValues::Load()
{
    int file_size;
    char *buffer;
    if (!is_loaded)
    {
        int file_number = 1;
        int loaded_count = 0;
        int record_total = 1;
        do
        {
            String file_text;
            String file_path = file_prefix;
            if (file_number < 10)
                file_path = file_path + "00" + IntToStr(file_number) + ".ecf";
            else
                file_path = file_path + "0" + IntToStr(file_number) + ".ecf";
            int h_file;
            try
            {
                h_file = FileOpen(file_path.c_str(), fmOpenRead);
                file_size = FileSeek(h_file, 0, 2);
                FileSeek(h_file, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(h_file, buffer, file_size);
                FileClose(h_file);
                file_text = buffer;
                file_text.SetLength(file_size);
                delete[] buffer;
                if (file_text[1] != 'E' || file_text[2] != 'C' || file_text[3] != 'F')
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
                for (int i = 0; loaded_count < record_total && i < 250; i++)
                {
                    // Each class: a length byte, the name, then 14 bytes of numbers.
                    int name_length = DecodeNumber(file_text.SubString(1, 1));
                    AddClass(Count() + 1, DecodeNumber(file_text.SubString(name_length + 2, 1)),
                        file_text.SubString(2, name_length), DecodeNumber(file_text.SubString(name_length + 3, 1)),
                        DecodeNumber(file_text.SubString(name_length + 4, 2)),
                        DecodeNumber(file_text.SubString(name_length + 6, 2)),
                        DecodeNumber(file_text.SubString(name_length + 8, 2)),
                        DecodeNumber(file_text.SubString(name_length + 10, 2)),
                        DecodeNumber(file_text.SubString(name_length + 12, 2)),
                        DecodeNumber(file_text.SubString(name_length + 14, 2)));
                    loaded_count++;
                    file_text.Delete(1, name_length + 15);
                }
            }
            catch (...)
            {
                FileClose(h_file);
                is_loaded = false;
            }
            file_number++;
        } while (loaded_count < record_total);
        is_loaded = true;
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

void ClassValues::AddClass(int class_id, int parent_class_id, String name, int stat_group,
    short strength, short intelligence, short wisdom,
    short agility, short constitution, short charisma)
{
    ClassValue handle(class_id);
    handle.name = name;
    handle.parent_class_id = parent_class_id;
    handle.stat_group = stat_group;
    handle.strength = strength;
    handle.intelligence = intelligence;
    handle.wisdom = wisdom;
    handle.agility = agility;
    handle.constitution = constitution;
    handle.charisma = charisma;
    records.insert(records.end(), handle);
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

// True when ancestor_id is 0, or is class_id or one of its parents, following each class's parent up the file.
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

DEFINE_DECODE_NUMBER(ClassValues)
