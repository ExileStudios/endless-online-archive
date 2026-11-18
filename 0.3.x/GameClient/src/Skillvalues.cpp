#include <vcl.h>
#pragma hdrstop

#include "Skillvalues.h"
#include "Classvalues.h"
#include "Packetcontrols.h"
#include <io.h>
#include <stdio.h>
#include "Pubfileheader.h"

#pragma package(smart_init)

SkillValues::SkillValues(String prefix, char keep_parts)
{
    file_prefix = prefix + "dsl";
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

SkillValues::~SkillValues()
{
}

// Reads the checksum and record count from the head of the first skill file; a missing file is marked.
void SkillValues::ReadHeader()
{
    int file_handle;
    char *header_bytes;
    int file_length;
    String header_file = file_prefix + "001.esf";
    try
    {
        file_handle = FileOpen(header_file.c_str(), fmOpenRead);
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
        if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
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

// Writes one part of the skill file as the server sent it.
void SkillValues::SaveFilePart(int part_number, String data)
{
    String lp_file_name = file_prefix;
    if (part_number < 10)
        lp_file_name = lp_file_name + "00" + IntToStr(part_number) + ".esf";
    else
        lp_file_name = lp_file_name + "0" + IntToStr(part_number) + ".esf";
    basic_ofstream<char, char_traits<char> > handle;
    handle.open(lp_file_name.c_str(), std::ios::binary);
    handle << data.c_str();
    handle.close();
}

// Loads the skill records from the numbered .esf files, checking each file's mark and reading up to 900 records from
// each, until the count the headers give is read.
void SkillValues::Load()
{
    int file_handle;
    int part_length;
    short part_number;
    char *buffer;
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        ClearFileParts();
        part_number = 0;
        records_read = 0;
        records_expected = 0;
        do
        {
            part_number++;
            if (part_number < 10)
                file_name = "00" + IntToStr(part_number) + ".esf";
            else
                file_name = "0" + IntToStr(part_number) + ".esf";
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
                if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
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
                    unsigned char shout_length;
                    shout_length = ReadByte();
                    if (name_length > 50)
                    {
                        has_bad_record = true;
                        header->load_state = 3;
                    }
                    if (shout_length > 50)
                    {
                        has_bad_record = true;
                        header->load_state = 3;
                    }
                    String skill_name = data.SubString(read_position, name_length);
                    SkillValue *skill = new SkillValue(Count() + 1);
                    skill->name = skill_name;
                    read_position += name_length;
                    String shout = data.SubString(read_position, shout_length);
                    skill->shout = shout;
                    read_position += shout_length;
                    skill->icon = DecodeNumber(2);
                    skill->graphic = DecodeNumber(2);
                    skill->tp_cost = DecodeNumber(2);
                    skill->sp_cost = DecodeNumber(2);
                    skill->cast_time = ReadByte();
                    skill->category = ReadByte();
                    skill->spare_byte = ReadByte();
                    skill->type = DecodeNumber(3);
                    skill->element = ReadByte();
                    skill->element_power = DecodeNumber(2);
                    skill->target_restriction = ReadByte();
                    skill->target_type = ReadByte();
                    skill->target_time = ReadByte();
                    skill->area = ReadByte();
                    skill->max_level = DecodeNumber(2);
                    skill->min_damage = DecodeNumber(2);
                    skill->max_damage = DecodeNumber(2);
                    skill->accuracy = DecodeNumber(2);
                    skill->evade = DecodeNumber(2);
                    skill->armor = DecodeNumber(2);
                    skill->return_damage = ReadByte();
                    skill->hp_heal = DecodeNumber(2);
                    skill->tp_heal = DecodeNumber(2);
                    skill->sp_heal = ReadByte();
                    skill->bonus_str = DecodeNumber(2);
                    skill->bonus_int = DecodeNumber(2);
                    skill->bonus_wis = DecodeNumber(2);
                    skill->bonus_agi = DecodeNumber(2);
                    skill->bonus_con = DecodeNumber(2);
                    skill->bonus_cha = DecodeNumber(2);
                    records.insert(records.end(), skill);
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

// Deletes the skill part files from 001 up to the first one missing, 100 at most.
void SkillValues::DeleteFiles()
{
    char file_found = true;
    String lp_file_name;
    try
    {
        for (int i = 1; i <= 100 && file_found; i++)
        {
            lp_file_name = file_prefix;
            if (i < 10)
                lp_file_name = lp_file_name + "00" + IntToStr(i) + ".esf";
            else
                lp_file_name = lp_file_name + "0" + IntToStr(i) + ".esf";
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

// Reads the skill file again from the folder given: the prefix of its file names, then a fresh load.
void SkillValues::LoadFrom(String folder)
{
    file_prefix = folder + "dsl";
    Clear();
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
    Load();
}

// Deletes every skill record.
void SkillValues::Clear()
{
    for (vector<SkillValue *>::iterator it = records.begin(); it != records.end(); it++)
        delete *it;
    records.clear();
}

// The skill with this name, in any case, or 0.
SkillValue *SkillValues::FindByName(String name)
{
    for (vector<SkillValue *>::iterator it = records.begin(); it != records.end(); it++)
    {
        if (LowerCase(name) == LowerCase((*it)->name))
            return *it;
    }
    return 0;
}

// The skill record at the index, kept within the list.
SkillValue *SkillValues::GetSkill(int index)
{
    if (index < 0)
        index = 0;
    if (records.size() <= index)
        index = records.size() - 1;
    return records[index];
}

int SkillValues::Count()
{
    return records.size();
}

// Deletes every skill record.
DEFINE_CLEAR_HEADERS(SkillValues)

void SkillValues::ClearFileParts()
{
    file_parts->Clear();
}

DEFINE_READ_BYTE(SkillValues)

DEFINE_DECODE_NUMBER(SkillValues)
