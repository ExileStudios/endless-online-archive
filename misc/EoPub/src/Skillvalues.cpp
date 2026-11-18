#include <vcl.h>
#pragma hdrstop

#include "Skillvalues.h"
#include "Pubfileheader.h"
#include "Skillvalue.h"

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

void Skillvalues_UnusedLiterals1()
{
    "001.esf";
    "";
    "00";
    ".esf";
    "0";
    ".esf";
    "";
    "";
}

// Loads the skills from the numbered .esf files, up to 900 from each, until the count the headers give is read.
void SkillValues::Load()
{
    int file_handle;
    int file_size;
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
                if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
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
                    skill->icon = ReadNumber(2);
                    skill->graphic = ReadNumber(2);
                    skill->tp_cost = ReadNumber(2);
                    skill->sp_cost = ReadNumber(2);
                    skill->cast_time = ReadByte();
                    skill->category = ReadByte();
                    skill->spare_byte = ReadByte();
                    skill->type = ReadNumber(3);
                    skill->element = ReadByte();
                    skill->element_power = ReadNumber(2);
                    skill->target_restriction = ReadByte();
                    skill->target_type = ReadByte();
                    skill->target_time = ReadByte();
                    skill->area = ReadByte();
                    skill->max_level = ReadNumber(2);
                    skill->min_damage = ReadNumber(2);
                    skill->max_damage = ReadNumber(2);
                    skill->accuracy = ReadNumber(2);
                    skill->evade = ReadNumber(2);
                    skill->armor = ReadNumber(2);
                    skill->return_damage = ReadByte();
                    skill->hp_heal = ReadNumber(2);
                    skill->tp_heal = ReadNumber(2);
                    skill->sp_heal = ReadByte();
                    skill->bonus_str = ReadNumber(2);
                    skill->bonus_int = ReadNumber(2);
                    skill->bonus_wis = ReadNumber(2);
                    skill->bonus_agi = ReadNumber(2);
                    skill->bonus_con = ReadNumber(2);
                    skill->bonus_cha = ReadNumber(2);
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

void Skillvalues_UnusedLiterals2()
{
    "00";
    ".esf";
    "0";
    ".esf";
    "";
    "";
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
    for (vector<SkillValue *>::iterator record = records.begin(); record != records.end(); record++)
        delete *record;
    records.clear();
}

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

char SkillValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *SkillValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(SkillValues)

void SkillValues::ClearFileParts()
{
    file_parts->Clear();
}

DEFINE_NPC_READ_BYTE(SkillValues)

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
DEFINE_READ_NUMBER(SkillValues)
