#include <vcl.h>
#pragma hdrstop

#include "Learnvalues.h"
#include <Classes.hpp>
#include "Npcvalues.h"

#pragma package(smart_init)

LearnValues::LearnValues(String prefix)
{
    file_prefix = prefix;
    record_index = -1;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

LearnValues::~LearnValues()
{
}

// Reads skills.ini: each name key starts a skill master and each learn line adds a skill of fourteen numbers.
// Returns the masters read, or -1 when the file has no lines.
int LearnValues::ImportIni(int format, String path)
{
    int level_requirement;
    int price;
    int cha_requirement;
    int unused_value;
    int second_unused_value;
    int third_unused_value;
    TStringList *lines = new TStringList();
    try
    {
        lines->LoadFromFile(path);
    }
    catch (...)
    {
    }
    if (lines->Count <= 0)
        return -1;
    int entries_read = 0;
    int master_count = 0;
    for (int line_index = 0; lines->Count > line_index; line_index++)
    {
        String line = lines->Strings[line_index];
        if (line.Length() < 1)
            continue;
        if (line[1] == '#')
            continue;
        int equals_pos = line.Pos("=");
        if (equals_pos <= 0)
            continue;
        String key = line.SubString(1, equals_pos - 1);
        value_text = line.SubString(equals_pos + 1, line.Length() - equals_pos);
        int unused_count = 0;
        int name_pos = key.Pos(".name");
        if (name_pos > 0)
        {
            String id_part = key.SubString(1, name_pos - 1);
            int behavior_id = ParseNumber(id_part);
            while (value_text.Length() > 0)
            {
                if (value_text[1] != ' ')
                    break;
                value_text.Delete(1, 1);
            }
            while (value_text.Length() > 0)
            {
                if (value_text[value_text.Length()] != ' ')
                    break;
                value_text.Delete(value_text.Length(), 1);
            }
            // A new skill master serves every level up to 252 and every class.
            LearnValue master(Count() + 1);
            master.behavior_id = behavior_id;
            master.name = value_text;
            master.min_level = 0;
            master.max_level = 252;
            master.class_requirement = 0;
            records.insert(records.end(), master);
            records_read++;
            entries_read++;
            master_count++;
            continue;
        }
        int learn_pos = key.Pos(".learn");
        if (learn_pos > 0)
        {
            String learn_id_part = key.SubString(1, name_pos - 1);
            int learn_id[1];
            learn_id[0] = ParseNumber(learn_id_part);
            while (value_text.Length() > 0)
            {
                int skill_id = ReadNextNumber();
                price = ReadNextNumber();
                level_requirement = ReadNextNumber();
                int class_requirement = ReadNextNumber();
                int skill_requirement_1 = ReadNextNumber();
                int skill_requirement_2 = ReadNextNumber();
                int skill_requirement_3 = ReadNextNumber();
                int skill_requirement_4 = ReadNextNumber();
                int str_requirement = ReadNextNumber();
                int wis_requirement = ReadNextNumber();
                int int_requirement = ReadNextNumber();
                int agi_requirement = ReadNextNumber();
                int con_requirement = ReadNextNumber();
                cha_requirement = ReadNextNumber();
                if (skill_id == -1 || price == -1 || level_requirement == -1 || class_requirement == -1 ||
                    skill_requirement_1 == -1 || skill_requirement_2 == -1 || skill_requirement_3 == -1 ||
                    skill_requirement_4 == -1 || str_requirement == -1 || wis_requirement == -1 ||
                    int_requirement == -1 || agi_requirement == -1 || con_requirement == -1 || cha_requirement == -1)
                    break;
                records[master_count - 1].AddSkill(skill_id, level_requirement, class_requirement, price,
                    skill_requirement_1, skill_requirement_2, skill_requirement_3, skill_requirement_4, str_requirement,
                    wis_requirement, int_requirement, agi_requirement, con_requirement, cha_requirement);
            }
        }
    }
    return entries_read;
}

int CopyLearnItems(int first, int last, int dest)
{
    for (; first != last; dest = dest + 32)
    {
        *(LearnItemVal *)dest = *(LearnItemVal *)first;
        first += 32;
    }
    return dest;
}

// Takes the next number of a comma list off the value text; -1 when none is left.
DEFINE_READ_NEXT_NUMBER(LearnValues)

// The digits of the text as a number; 0 when it has none.
DEFINE_PARSE_NUMBER(LearnValues)

void Learnvalues_UnusedLiterals1()
{
    "";
    "serv_trainers.epf";
    "";
    "";
}

LearnValue *LearnValues::GetTrainer(int index)
{
    return &records[index];
}

int LearnValues::Count()
{
    return records.size();
}
