#include <vcl.h>
#pragma hdrstop

#include "Innvalues.h"
#include "Innvalue.h"
#include "Constants.h"

#pragma package(smart_init)

InnValues::InnValues()
{
    record_index = -1;
    loaded = 0;
    LoadInns();
}

InnValues::~InnValues()
{
}

// Reads pub/din001.eid ( it starts EID ), one record an inn.
void InnValues::LoadInns()
{
    int file_handle;
    if (!loaded)
    {
        Clear();
        String data;
        String path = "./pub/din001.eid";
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
            if (data[1] != 'E' || data[2] != 'I' || data[3] != 'D')
                return;
            data.Delete(1, 3);
            do
            {
                int id = DecodeNumber(data.SubString(1, 2));
                int namelen = DecodeNumber(data.SubString(3, 1));
                InnValue record(id);
                record.name = data.SubString(4, namelen);
                record.spawn_map = DecodeNumber(data.SubString(namelen + 4, 2));
                record.spawn_x = DecodeNumber(data.SubString(namelen + 6, 1));
                record.spawn_y = DecodeNumber(data.SubString(namelen + 7, 1));
                record.sleep_map = DecodeNumber(data.SubString(namelen + 8, 2));
                record.sleep_x = DecodeNumber(data.SubString(namelen + 10, 1));
                record.sleep_y = DecodeNumber(data.SubString(namelen + 11, 1));
                record.alternate_spawn_level = DecodeNumber(data.SubString(namelen + 12, 1));
                record.alternate_spawn_map = DecodeNumber(data.SubString(namelen + 13, 2));
                record.alternate_spawn_x = DecodeNumber(data.SubString(namelen + 15, 1));
                record.alternate_spawn_y = DecodeNumber(data.SubString(namelen + 16, 1));
                for (int i = 0; i < INN_QUESTION_COUNT; i++)
                {
                    int qlen = DecodeNumber(data.SubString(namelen + 17, 1));
                    if (qlen > 0)
                        record.question[i] = data.SubString(namelen + 18, qlen);
                    namelen = namelen + qlen + 1;
                    int alen = DecodeNumber(data.SubString(namelen + 17, 1));
                    if (alen > 0)
                        record.answer[i] = data.SubString(namelen + 18, alen);
                    namelen = namelen + alen + 1;
                }
                data.Delete(1, namelen + 16);
                record_list.insert(record_list.end(), record);
            } while (data.Length() > 15);
            loaded = 1;
        }
        catch (...)
        {
            FileClose(file_handle);
            loaded = 0;
        }
    }
}

String InnValues::GetName(unsigned int index)
{
    if (record_list.size() <= index)
        return "";
    else
        return record_list[index].name;
}

int InnValues::GetSleepMap(unsigned int index)
{
    if (record_list.size() < index)
        return -1;
    return record_list[index].sleep_map;
}

int InnValues::GetSleepX(int index)
{
    return record_list[index].sleep_x;
}

int InnValues::GetSleepY(int index)
{
    return record_list[index].sleep_y;
}

// The map the inn's citizens spawn on; the alternate spawn once the player's level reaches alternate_spawn_level.
int InnValues::GetSpawnMap(unsigned int index, int level)
{
    int result = -1;
    if (record_list.size() > index)
    {
        result = record_list[index].spawn_map;
        if (record_list[index].alternate_spawn_level > 0 && record_list[index].alternate_spawn_level <= level)
            result = record_list[index].alternate_spawn_map;
    }
    return result;
}

int InnValues::GetSpawnX(int index, int level)
{
    int result = record_list[index].spawn_x;
    if (record_list[index].alternate_spawn_level > 0 && record_list[index].alternate_spawn_level <= level)
        result = record_list[index].alternate_spawn_x;
    return result;
}

int InnValues::GetSpawnY(int index, int level)
{
    int result = record_list[index].spawn_y;
    if (record_list[index].alternate_spawn_level > 0 && record_list[index].alternate_spawn_level <= level)
        result = record_list[index].alternate_spawn_y;
    return result;
}

// The inn's three questions, each after a break byte.
String InnValues::GetQuestion(unsigned int index)
{
    int i;
    if (record_list.size() <= index)
        return "";
    String result;
    for (i = 0; i < INN_QUESTION_COUNT; i++)
    {
        result.Insert((char)EO_BREAK_BYTE, result.Length() + 1);
        result.Insert(record_list[index].question[i], result.Length() + 1);
    }
    return result;
}

String InnValues::GetAnswer(unsigned int index, int number)
{
    if (record_list.size() <= index)
        return "";
    else
        return record_list[index].answer[number];
}

void InnValues::Clear()
{
    if (GetCount() >= 1)
    {
        record_list.clear();
        record_index = -1;
    }
}

unsigned int InnValues::GetCount()
{
    return record_list.size();
}

DEFINE_INN_DECODE_NUMBER(InnValues)
