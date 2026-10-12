#include <vcl.h>
#pragma hdrstop

#include "Skillvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
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

SkillValues::SkillValues()
{
    file_prefix = "./pub/dsl";
    is_downloading = 0;
    is_loaded = 0;
    unread_int_at_64 = -1;
    checksum_high = 0;
    checksum_high = 0;
    record_count = 0;
}

SkillValues::~SkillValues()
{
}

// Reads the checksum and skill count from the header of the first skill file.
// With more than 900 skills the last part file is read too; the values are kept only when its header matches.
void SkillValues::ReadHeader()
{
    String file_text;
    int first_checksum_high;
    int first_checksum_low;
    int file_handle;
    DWORD file_size;
    char *buffer;
    int total_records;
    String file_name = file_prefix + "001.esf";
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
        // "ESF".
        if (file_text[1] != 'E' || file_text[2] != 'S' || file_text[3] != 'F')
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
            file_name = file_name + "00" + IntToStr(last_file_number) + ".esf";
        else
            file_name = file_name + "0" + IntToStr(last_file_number) + ".esf";
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
            if (file_text[1] != 'E' || file_text[2] != 'S' || file_text[3] != 'F')
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

// Reads every skill part file into the list, once until the files change.
void SkillValues::Load()
{
    int file_number;
    int loaded_count;
    int record_total;
    int hFile;
    if (!is_loaded)
    {
        file_number = 1;
        loaded_count = 0;
        record_total = 1;
        do
        {
            String file_text;
            String file_path = file_prefix;
            if (file_number < 10)
                file_path = file_path + "00" + IntToStr(file_number) + ".esf";
            else
                file_path = file_path + "0" + IntToStr(file_number) + ".esf";
            try
            {
                DWORD file_size;
                char *buffer;
                hFile = FileOpen(file_path.c_str(), 0);
                file_size = FileSeek(hFile, 0, 2);
                FileSeek(hFile, 0, 0);
                buffer = new char[file_size + 1];
                FileRead(hFile, buffer, file_size);
                FileClose(hFile);
                file_text = buffer;
                file_text.SetLength(file_size);
                delete[] buffer;
                if (file_text[1] != 'E' || file_text[2] != 'S' || file_text[3] != 'F')
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
                    int name_length = DecodeNumber(file_text.SubString(1, 1));
                    int fields_start;
                    int shout_length = DecodeNumber(file_text.SubString(2, 1));
                    // Each skill: two length bytes, the name, the shout, then 51 bytes of numbers.
                    fields_start = name_length + shout_length + 2;
                    AddSkill(Count() + 1, file_text.SubString(3, name_length),
                        file_text.SubString(name_length + 3, shout_length),
                        DecodeNumber(file_text.SubString(fields_start + 1, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 3, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 5, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 7, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 9, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 10, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 11, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 12, 3)),
                        DecodeNumber(file_text.SubString(fields_start + 15, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 16, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 18, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 19, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 20, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 21, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 22, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 24, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 26, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 28, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 30, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 32, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 34, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 35, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 37, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 39, 1)),
                        DecodeNumber(file_text.SubString(fields_start + 40, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 42, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 44, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 46, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 48, 2)),
                        DecodeNumber(file_text.SubString(fields_start + 50, 2)));
                    loaded_count++;
                    file_text.Delete(1, fields_start + 51);
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

// Writes one part of the skill file as the server sent it.
void SkillValues::SaveFilePart(int part_number, String data)
{
    String lpFileName = file_prefix;
    if (part_number < 10)
        lpFileName = lpFileName + "00" + IntToStr(part_number) + ".esf";
    else
        lpFileName = lpFileName + "0" + IntToStr(part_number) + ".esf";
    std::ofstream handle;
    handle.open(lpFileName.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Deletes the skill part files from 001 up to the first one missing, 100 at most.
void SkillValues::DeleteFiles()
{
    char file_found = 1;
    String lpFileName;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lpFileName = file_prefix;
            if (i < 10)
                lpFileName = lpFileName + "00" + IntToStr(i) + ".esf";
            else
                lpFileName = lpFileName + "0" + IntToStr(i) + ".esf";
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

void SkillValues::AddSkill(int skill_id, String name, String shout, short icon, short graphic, short tp_cost,
    short sp_cost, short cast_time, short category, short skill_record_byte_at_13, short type,
    short skill_record_byte_at_17, short skill_record_word_at_18, short target_restriction, short target_type,
    short skill_record_byte_at_22, short skill_record_byte_at_23, short skill_record_word_at_24,
    short skill_record_word_at_26, short skill_record_word_at_28, short skill_record_word_at_30,
    short skill_record_word_at_32, short skill_record_word_at_34, short skill_record_byte_at_36,
    short skill_record_word_at_37, short skill_record_word_at_39, short skill_record_byte_at_41,
    short skill_record_word_at_42, short skill_record_word_at_44, short skill_record_word_at_46,
    short skill_record_word_at_48, short skill_record_word_at_50, short skill_record_word_at_52)
{
    SkillValue handle(skill_id);
    handle.name = name;
    handle.shout = shout;
    handle.icon = icon;
    handle.graphic = graphic;
    handle.skill_record_word_at_24 = skill_record_word_at_24;
    handle.tp_cost = tp_cost;
    handle.sp_cost = sp_cost;
    handle.cast_time = cast_time;
    handle.category = category;
    handle.skill_record_byte_at_13 = skill_record_byte_at_13;
    handle.type = type;
    handle.skill_record_byte_at_17 = skill_record_byte_at_17;
    handle.skill_record_word_at_18 = skill_record_word_at_18;
    handle.target_restriction = target_restriction;
    handle.target_type = target_type;
    handle.skill_record_byte_at_22 = skill_record_byte_at_22;
    handle.skill_record_byte_at_23 = skill_record_byte_at_23;
    handle.skill_record_word_at_26 = skill_record_word_at_26;
    handle.skill_record_word_at_28 = skill_record_word_at_28;
    handle.skill_record_word_at_30 = skill_record_word_at_30;
    handle.skill_record_word_at_32 = skill_record_word_at_32;
    handle.skill_record_word_at_34 = skill_record_word_at_34;
    handle.skill_record_byte_at_36 = skill_record_byte_at_36;
    handle.skill_record_word_at_37 = skill_record_word_at_37;
    handle.skill_record_word_at_39 = skill_record_word_at_39;
    handle.skill_record_byte_at_41 = skill_record_byte_at_41;
    handle.skill_record_word_at_42 = skill_record_word_at_42;
    handle.skill_record_word_at_44 = skill_record_word_at_44;
    handle.skill_record_word_at_46 = skill_record_word_at_46;
    handle.skill_record_word_at_48 = skill_record_word_at_48;
    handle.skill_record_word_at_50 = skill_record_word_at_50;
    handle.skill_record_word_at_52 = skill_record_word_at_52;
    records.insert(records.end(), handle);
}

DEFINE_CLEAR(SkillValues)

// The id of the skill with this name, in any case, or -1.
int SkillValues::FindByName(String name)
{
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (LowerCase(name) == LowerCase(it->name))
            return it->skill_id;
    }
    return -1;
}

int SkillValues::GetGraphic(int skill_id)
{
    int graphic = -1;
    vector<SkillValue>::iterator it;
    for (it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            graphic = it->graphic;
            break;
        }
    }
    return graphic;
}

int SkillValues::GetTargetRestriction(int skill_id)
{
    int target_restriction = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            target_restriction = it->target_restriction;
            break;
        }
    }
    return target_restriction;
}

int SkillValues::GetTargetType(int skill_id)
{
    int target_type = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            target_type = it->target_type;
            break;
        }
    }
    return target_type;
}

int SkillValues::GetCastTime(int skill_id)
{
    int cast_time = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            cast_time = it->cast_time;
            break;
        }
    }
    return cast_time;
}

int SkillValues::GetTpCost(int skill_id)
{
    int tp_cost = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            tp_cost = it->tp_cost;
            break;
        }
    }
    return tp_cost;
}

int SkillValues::GetSpCost(int skill_id)
{
    int sp_cost = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            sp_cost = it->sp_cost;
            break;
        }
    }
    return sp_cost;
}

int SkillValues::GetType(int skill_id)
{
    int type = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            type = it->type;
            break;
        }
    }
    return type;
}

int SkillValues::GetCategory(int skill_id)
{
    int category = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            category = it->category;
            break;
        }
    }
    return category;
}

int SkillValues::GetIcon(int skill_id)
{
    int icon = -1;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            icon = it->icon;
            break;
        }
    }
    return icon;
}

String SkillValues::GetName(int skill_id)
{
    String name;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

// The words shown over the caster when the skill is cast.
String SkillValues::GetShout(int skill_id)
{
    String shout;
    for (vector<SkillValue>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            shout = it->shout;
            break;
        }
    }
    return shout;
}

unsigned int SkillValues::Count()
{
    return records.size();
}

DEFINE_DECODE_NUMBER(SkillValues)
