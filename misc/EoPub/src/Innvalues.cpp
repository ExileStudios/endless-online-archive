#include <vcl.h>
#pragma hdrstop

#include "Innvalues.h"
#include "Pubfileheader.h"

#pragma package(smart_init)

InnQuizItem::InnQuizItem()
{
    question = "";
    answer = "";
}

InnQuizItem::~InnQuizItem()
{
}

InnValue::InnValue()
{
}

InnValue::InnValue(int record_id)
{
    id = record_id;
    behavior_id = record_id;
    sleep_cost = 0;
}

// Adds a question the innkeeper asks, with its answer.
void InnValue::AddQuestion(String question, String answer)
{
    InnQuizItem item;
    item.question = question;
    item.answer = answer;
    questions.insert(questions.end(), item);
}

InnValue::~InnValue()
{
    questions.clear();
}

InnValues::InnValues(String prefix)
{
    file_prefix = prefix;
    is_loaded = false;
    has_bad_record = false;
    header_read = false;
}

InnValues::~InnValues()
{
}

// Loads the inns from serv_inns.epf: each inn's cost, sleep and spawn places, and its questions.
void InnValues::Load()
{
    if (!is_loaded)
    {
        Clear();
        ClearHeaders();
        records_read = 0;
        record_count = 0;
        int handle;
        int file_length;
        char *buffer;
        file_name = "serv_inns.epf";
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
            if (data[1] != 'E' || data[2] != 'I' || data[3] != 'D')
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
                short behavior_id = ReadNumber(2);
                unsigned char name_length = ReadByte();
                if (name_length > 50)
                {
                    has_bad_record = true;
                    header->load_state = 3;
                }
                InnValue *inn = new InnValue(Count() + 1);
                inn->name = data.SubString(read_position, name_length);
                read_position += name_length;
                inn->behavior_id = behavior_id;
                inn->sleep_cost = ReadNumber(3);
                inn->sleep_map = ReadNumber(2);
                inn->sleep_x = ReadByte();
                inn->sleep_y = ReadByte();
                inn->spawn_map = ReadNumber(2);
                inn->spawn_x = ReadByte();
                inn->spawn_y = ReadByte();
                inn->alternate_spawn_level = ReadNumber(2);
                inn->alternate_spawn_map = ReadNumber(2);
                inn->alternate_spawn_x = ReadByte();
                inn->alternate_spawn_y = ReadByte();
                unsigned char question_count = ReadByte();
                // Each question comes with its answer; a question with no text is left out.
                for (int i = 0; i < question_count; i++)
                {
                    String question = "";
                    String answer = "";
                    unsigned char question_length = ReadByte();
                    if (question_length > 0)
                        question = data.SubString(read_position, question_length);
                    read_position += question_length;
                    unsigned char answer_length = ReadByte();
                    if (answer_length > 0)
                        answer = data.SubString(read_position, answer_length);
                    read_position += answer_length;
                    if (question.Length() > 0)
                        inn->AddQuestion(question, answer);
                }
                records.insert(records.end(), inn);
                header->records_read++;
                records_read++;
                if (read_position - 1 == data.Length())
                {
                    if (header->load_state != 3)
                        header->load_state = 0;
                    break;
                }
            } while ((unsigned int)(read_position + 15) < data.Length());
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

void Innvalues_UnusedLiterals1()
{
    "";
}

// Reads the inns again from the folder given.
DEFINE_LOAD_FROM(InnValues)

// Deletes every inn record.
void InnValues::Clear()
{
    for (vector<InnValue *>::iterator record = records.begin(); record != records.end(); record++)
        delete *record;
    records.clear();
}

InnValue *InnValues::GetInn(int index)
{
    if (index < 0)
        return 0;
    if (records.size() <= index)
        return 0;
    return records[index];
}

int InnValues::Count()
{
    return records.size();
}

char InnValues::HeaderCount()
{
    return headers.size();
}

PubfileHeader *InnValues::GetHeader(unsigned char index)
{
    return headers[index];
}

DEFINE_CLEAR_HEADERS(InnValues)

// Inserts an empty inn at the index, with the ID of that place.
void InnValues::InsertInn(int index)
{
    InnValue *inn = new InnValue(index + 1);
    records.insert(records.begin() + index, inn);
}

void InnValues::DeleteInn(int index)
{
    delete records[index];
    records.erase(records.begin() + index);
}

// Reads one byte at the read position, stored plus one; a value past 252 reads as 0.
DEFINE_READ_BYTE(InnValues)

// Reads a number of length bytes at the read position, each a base-253 digit plus one; a 254 ends it early.
DEFINE_READ_NUMBER(InnValues)
