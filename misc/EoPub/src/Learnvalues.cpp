#include <vcl.h>
#pragma hdrstop

#include "Learnvalues.h"
#include "Learnvalue.h"
#include "Pubfileheader.h"

#pragma package(smart_init)

LearnValues::LearnValues(String prefix)
{
    file_prefix = prefix;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

LearnValues::~LearnValues()
{
}

// Loads the skill masters from serv_trainers.epf: each one's levels, class and the skills it teaches.
void LearnValues::Load()
{
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        records_read = 0;
        record_count = 0;
        int handle;
        unsigned char name_length;
        int file_length;
        char *buffer;
        int i;
        file_name = "serv_trainers.epf";
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
            if (data[1] != 'E' || data[2] != 'T' || data[3] != 'F')
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
                LearnValue *trainer = new LearnValue(Count() + 1);
                trainer->name = data.SubString(read_position, name_length);
                read_position += name_length;
                trainer->behavior_id = behavior_id;
                trainer->min_level = ReadNumber(2);
                trainer->max_level = ReadNumber(2);
                trainer->class_requirement = (unsigned char)ReadByte();
                int skill_count = ReadNumber(2);
                // Each skill taught: its ID, needed level and class, price, skills and stats.
                for (i = 0; i < skill_count; i++)
                {
                    unsigned short read_skill_id = (short)ReadNumber(2);
                    short skill_id = read_skill_id;
                    name_length = ReadNumber(2);
                    unsigned char needed_class = ReadByte();
                    int price = ReadNumber(3);
                    unsigned short skill_requirement_1 = ReadNumber(2);
                    unsigned short skill_requirement_2 = ReadNumber(2);
                    unsigned short skill_requirement_3 = ReadNumber(2);
                    unsigned short skill_requirement_4 = ReadNumber(2);
                    unsigned short str_requirement = ReadNumber(2);
                    unsigned short wis_requirement = ReadNumber(2);
                    unsigned short int_requirement = ReadNumber(2);
                    unsigned short agi_requirement = ReadNumber(2);
                    unsigned short con_requirement = ReadNumber(2);
                    unsigned short cha_requirement = ReadNumber(2);
                    trainer->AddSkill(read_skill_id, name_length, needed_class, price, skill_requirement_1,
                        skill_requirement_2,
                        skill_requirement_3, skill_requirement_4, str_requirement, wis_requirement, int_requirement,
                        agi_requirement, con_requirement, cha_requirement);
                }
                records.insert(records.end(), trainer);
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

// Reads the skill masters again from the folder given.
DEFINE_LOAD_FROM(LearnValues)

// Deletes every skill master with its skills.
void LearnValues::Clear()
{
    for (vector<LearnValue *>::iterator record = records.begin(); record != records.end(); record++)
    {
        (*record)->skills.clear();
        delete *record;
    }
    records.clear();
}

// The skill master at the index; none outside the list.
LearnValue *LearnValues::Insert(int index)
{
    if (index < 0)
        return 0;
    if (records.size() <= index)
        return 0;
    return records[index];
}

int LearnValues::Count()
{
    return records.size();
}

char LearnValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *LearnValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(LearnValues)

void LearnValues::InsertTrainer(int index)
{
    LearnValue *trainer = new LearnValue(index + 1);
    records.insert(records.begin() + index, trainer);
}

void LearnValues::DeleteTrainer(int index)
{
    delete records[index];
    records.erase(records.begin() + index);
}

DEFINE_READ_BYTE(LearnValues)

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
DEFINE_READ_NUMBER(LearnValues)
