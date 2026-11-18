#include <vcl.h>
#pragma hdrstop

#include "Innvalues.h"
#include <Classes.hpp>
#include "Learnvalues.h"
#include "Npcvalues.h"

#pragma package(smart_init)

InnQuizItem::InnQuizItem()
{
    question = "";
    answer = "";
}

InnQuizItem::~InnQuizItem()
{
}

InnValue::InnValue(int record_id)
{
    id = record_id;
    behavior_id = record_id;
}

void Innvalues_UnusedInstances1(std::vector<InnQuizItem> &items, const InnQuizItem &item)
{
    items.insert(items.end(), item);
}

InnValue::~InnValue()
{
    questions.clear();
}

InnValues::InnValues(String prefix)
{
    file_prefix = prefix;
    record_index = -1;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

InnValues::~InnValues()
{
}

// Reads an inn .ini file: each name key starts an inn and the keys after it set its cost, places and questions.
// Returns the inns read, or -1 when the file has no lines.
int InnValues::ImportIni(int format, String path)
{
    int question_pos;
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
    int inn_count = 0;
    int answer_count = 0;
    for (int line_index = 0; lines->Count > line_index; line_index++)
    {
        String line = lines->Strings[line_index];
        if (line.Length() < 1)
            continue;
        if (line[1] == '#')
            continue;
        int key_pos = line.Pos("=");
        if (key_pos <= 0)
            continue;
        String key = line.SubString(1, key_pos - 1);
        value_text = line.SubString(key_pos + 1, line.Length() - key_pos);
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
            int inn_id = Count() + 1;
            InnValue inn(inn_id);
            inn.behavior_id = behavior_id;
            inn.name = value_text;
            inn.behavior_id = inn_id;
            inn.sleep_cost = 0;
            inn.sleep_map = 0;
            inn.sleep_x = 0;
            inn.sleep_y = 0;
            inn.alternate_spawn_level = 0;
            inn.alternate_spawn_map = 0;
            inn.alternate_spawn_x = 0;
            inn.alternate_spawn_y = 0;
            records.insert(records.end(), inn);
            inn_count++;
            records_read++;
            entries_read++;
            answer_count = 0;
            continue;
        }
        // Keys before the name of the first inn are skipped.
        if (!inn_count)
            continue;
        // Longer keys are tried before the shorter keys they begin with.
        key_pos = key.Pos(".innsleepcost");
        if (key_pos > 0)
        {
            records[inn_count - 1].sleep_cost = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".innsleep");
        if (key_pos > 0)
        {
            records[inn_count - 1].sleep_map = ReadNextNumber();
            records[inn_count - 1].sleep_x = ReadNextNumber();
            records[inn_count - 1].sleep_y = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".inncost");
        if (key_pos > 0)
        {
            records[inn_count - 1].sleep_cost = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".innkeeper");
        if (key_pos > 0)
        {
            records[inn_count - 1].behavior_id = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".hi_level_location");
        if (key_pos > 0)
        {
            records[inn_count - 1].alternate_spawn_map = ReadNextNumber();
            records[inn_count - 1].alternate_spawn_x = ReadNextNumber();
            records[inn_count - 1].alternate_spawn_y = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".hi_level");
        if (key_pos > 0)
        {
            records[inn_count - 1].alternate_spawn_level = ReadNextNumber();
            continue;
        }
        key_pos = key.Pos(".location");
        if (key_pos > 0)
        {
            records[inn_count - 1].spawn_map = ReadNextNumber();
            records[inn_count - 1].spawn_x = ReadNextNumber();
            records[inn_count - 1].spawn_y = ReadNextNumber();
            continue;
        }
        question_pos = key.Pos(".question");
        if (question_pos > 0)
        {
            InnQuizItem question;
            question.question = value_text;
            question.answer = "";
            records[inn_count - 1].questions.insert(records[inn_count - 1].questions.end(), question);
        }
        key_pos = key.Pos(".answer");
        if (key_pos > 0)
        {
            InnValue current_inn = records[inn_count - 1];
            if ((unsigned int)(((int)current_inn.questions.end() - (int)current_inn.questions.begin()) / 8) <=
                answer_count)
                continue;
            records[inn_count - 1].questions[answer_count].answer = value_text;
            answer_count++;
        }
    }
    return entries_read;
}

int CopyInnQuizItems(int first, int last, int dest)
{
    int to;
    int from;
    for (int source = first; source != last; *(String *)(to + 4) = *(String *)(from + 4))
    {
        from = source;
        to = dest;
        source += 8;
        dest += 8;
        *(String *)to = *(String *)from;
    }
    return dest;
}

// Takes the next number of a comma list off the value text; -1 when none is left.
DEFINE_READ_NEXT_NUMBER(InnValues)

// The digits of the text as a number; 0 when it has none.
DEFINE_PARSE_NUMBER(InnValues)

void Innvalues_UnusedLiterals1()
{
    "";
    "serv_inns.epf";
    "";
    "";
    "";
    "";
    "";
}

InnValue *InnValues::GetInn(int index)
{
    return &records[index];
}

int InnValues::Count()
{
    return records.size();
}
