#include <vcl.h>
#pragma hdrstop

#include "Learnvalues.h"
#include "Learnitem.h"
#include "Learnvalue.h"
#include "Constants.h"

#pragma package(smart_init)

LearnValues::LearnValues()
{
    record_index = -1;
    loaded = 0;
    LoadSkillMasters();
}

LearnValues::~LearnValues()
{
}

// Reads pub/dsm001.emf ( it starts EMF ): each master, then the skills it teaches, 28 bytes a skill.
void LearnValues::LoadSkillMasters()
{
    int file_handle;
    if (!loaded)
    {
        Clear();
        String data;
        String path = "./pub/dsm001.emf";
        try
        {
            int size;
            char *file_buffer;
            file_handle = FileOpen(path.c_str(), 0);
            size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            file_buffer = new char[size + 1];
            FileRead(file_handle, file_buffer, size);
            FileClose(file_handle);
            data = file_buffer;
            data.SetLength(size);
            delete[] file_buffer;
            if (data[1] != 'E' || data[2] != 'M' || data[3] != 'F')
                return;
            data.Delete(1, 3);
            do
            {
                int id = DecodeNumber(data.SubString(1, 2));
                int namelen = DecodeNumber(data.SubString(3, 1));
                LearnValue record(id);
                record.name = data.SubString(4, namelen);
                record.min_level = DecodeNumber(data.SubString(namelen + 4, 1));
                record.max_level = DecodeNumber(data.SubString(namelen + 5, 1));
                record.class_requirement = DecodeNumber(data.SubString(namelen + 6, 1));
                int skill_count = DecodeNumber(data.SubString(namelen + 7, 2));
                data.Delete(1, namelen + 8);
                for (int j = 0; j < skill_count; j++)
                {
                    AddSkill(&record, DecodeNumber(data.SubString(1, 2)), DecodeNumber(data.SubString(3, 1)),
                        DecodeNumber(data.SubString(4, 1)), DecodeNumber(data.SubString(5, 4)),
                        DecodeNumber(data.SubString(9, 2)), DecodeNumber(data.SubString(11, 2)),
                        DecodeNumber(data.SubString(13, 2)), DecodeNumber(data.SubString(15, 2)),
                        DecodeNumber(data.SubString(17, 2)), DecodeNumber(data.SubString(19, 2)),
                        DecodeNumber(data.SubString(21, 2)), DecodeNumber(data.SubString(23, 2)),
                        DecodeNumber(data.SubString(25, 2)), DecodeNumber(data.SubString(27, 2)));
                    data.Delete(1, 28);
                }
                record_list.insert(record_list.end(), record);
            } while (data.Length() > 9);
            loaded = 1;
        }
        catch (...)
        {
            FileClose(file_handle);
            loaded = 0;
        }
    }
}

void LearnValues::AddSkill(LearnValue *record, int skill_id, int level_requirement, int class_requirement, int price,
    int skill_requirement_1, int skill_requirement_2, int skill_requirement_3, int skill_requirement_4,
    int str_requirement, int int_requirement, int wis_requirement, int agi_requirement, int con_requirement,
    int cha_requirement)
{
    record->AddSkill(skill_id, level_requirement, class_requirement, price, skill_requirement_1, skill_requirement_2,
        skill_requirement_3, skill_requirement_4, str_requirement, int_requirement, wis_requirement, agi_requirement,
        con_requirement, cha_requirement);
}

void LearnValues::Clear()
{
    for (vector<LearnValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
        it->skills.clear();
    if (GetCount() >= 1)
    {
        record_list.clear();
        record_index = -1;
    }
}

// True when the master ( by its id ) teaches the skill.
char LearnValues::HasSkill(int master_id, int skill_id)
{
    char result = 0;
    if (master_id > 0 && (int)record_list.size() >= master_id)
    {
        if (record_list[master_id - 1].skills.size() > 0)
        {
            for (vector<LearnItemVal>::iterator it = record_list[master_id - 1].skills.begin();
                it != record_list[master_id - 1].skills.end(); it++)
            {
                if (skill_id == it->id)
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    return result;
}

// The master's entry for the skill: its price and what it needs.
LearnItemVal LearnValues::GetSkill(int master_id, int skill_id)
{
    LearnItemVal result;
    if (master_id > 0 && (int)record_list.size() >= master_id)
    {
        if (record_list[master_id - 1].skills.size() > 0)
        {
            for (vector<LearnItemVal>::iterator it = record_list[master_id - 1].skills.begin();
                it != record_list[master_id - 1].skills.end(); it++)
            {
                if (skill_id == it->id)
                {
                    result = *it;
                    break;
                }
            }
        }
    }
    return result;
}

void Learnvalues_UnusedLiterals1()
{
    "EMF";
    "dsm001.emf";
}

// The master's skill list, as the client gets it when it opens the master; an id of 0 when there is none.
String LearnValues::BuildOpenData(int behavior_id)
{
    vector<LearnValue>::iterator i;
    String data = "";
    for (i = record_list.begin(); i != record_list.end(); i++)
    {
        if (i->id != behavior_id)
            continue;
        data = EncodeNumber(i->id, 2);
        data.Insert(i->name, data.Length() + 1);
        data.Insert((char)EO_BREAK_BYTE, data.Length() + 1);
        if (i->skills.size() > 0)
        {
            vector<LearnItemVal>::iterator it;
            for (it = i->skills.begin(); it != i->skills.end(); it++)
            {
                data.Insert(EncodeNumber(it->id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->level_requirement, 1), data.Length() + 1);
                data.Insert(EncodeNumber(it->class_requirement, 1), data.Length() + 1);
                data.Insert(EncodeNumber(it->price, 4), data.Length() + 1);
                data.Insert(EncodeNumber(it->skill_requirement_1, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->skill_requirement_2, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->skill_requirement_3, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->skill_requirement_4, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->str_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->int_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->wis_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->agi_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->con_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(it->cha_requirement, 2), data.Length() + 1);
            }
        }
    }
    if (data == "")
        data = EncodeNumber(0, 2);
    return data;
}

unsigned int LearnValues::GetCount()
{
    return record_list.size();
}

DEFINE_LEARN_ENCODE_NUMBER(LearnValues)

DEFINE_INN_DECODE_NUMBER(LearnValues)
