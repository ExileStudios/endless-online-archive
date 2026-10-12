#include <vcl.h>
#pragma hdrstop

#include "Npcvalues.h"
#include "Npcdrop.h"
#include "Npcvalue.h"
#include "Shopvalues.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

NpcValues::NpcValues()
{
    encode_scratch = new char[8];
    record_index = -1;
    loaded = 0;
    drops_loaded = false;
    talk_loaded = false;
    file_id = 0;
    string_list = new TStringList();
    LoadNpcs();
    LoadDrops();
    LoadTalk();
}

NpcValues::~NpcValues()
{
}

// Reads pub/dtn001.enf and the files after it until every npc is loaded.
void NpcValues::LoadNpcs()
{
    int file;
    int count;
    int total;
    int file_handle;
    if (!loaded)
    {
        file = 1;
        count = 0;
        total = 1;
        do
        {
            String data;
            String path = "./pub/dtn";
            if (file < 10)
                path = path + "00" + IntToStr(file) + ".enf";
            else
                path = path + "0" + IntToStr(file) + ".enf";
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
                // a file without the ENF header ends the loading
                if (data[1] != 'E' || data[2] != 'N' || data[3] != 'F')
                    return;
                string_list->Add(data);
                if (file == 1)
                {
                    rid[0] = DecodeNumber(data.SubString(4, 2));
                    rid[1] = DecodeNumber(data.SubString(6, 2));
                    int parsed = DecodeNumber(data.SubString(8, 2));
                    total = parsed;
                    num_records = parsed;
                }
                data.Delete(1, 10);
                for (int i = 0; count < total && i < 900; i++)
                {
                    int namelen = DecodeNumber(data.SubString(1, 1)) + 1;
                    AddNpc(GetCount() + 1, data.SubString(2, namelen - 1), DecodeNumber(data.SubString(namelen + 1, 2)),
                        DecodeNumber(data.SubString(namelen + 3, 1)), DecodeNumber(data.SubString(namelen + 4, 2)),
                        DecodeNumber(data.SubString(namelen + 6, 2)), DecodeNumber(data.SubString(namelen + 8, 2)),
                        DecodeNumber(data.SubString(namelen + 10, 2)), DecodeNumber(data.SubString(namelen + 12, 3)),
                        DecodeNumber(data.SubString(namelen + 15, 2)), DecodeNumber(data.SubString(namelen + 17, 2)),
                        DecodeNumber(data.SubString(namelen + 19, 2)), DecodeNumber(data.SubString(namelen + 21, 2)),
                        DecodeNumber(data.SubString(namelen + 23, 2)), DecodeNumber(data.SubString(namelen + 25, 2)),
                        DecodeNumber(data.SubString(namelen + 27, 1)), DecodeNumber(data.SubString(namelen + 28, 2)),
                        DecodeNumber(data.SubString(namelen + 30, 2)), DecodeNumber(data.SubString(namelen + 32, 2)),
                        DecodeNumber(data.SubString(namelen + 34, 2)), DecodeNumber(data.SubString(namelen + 36, 1)),
                        DecodeNumber(data.SubString(namelen + 37, 3)));
                    count++;
                    data.Delete(1, namelen + 39);
                }
            }
            catch (...)
            {
                FileClose(file_handle);
                loaded = 0;
            }
            file++;
        } while (count < total);
        file_id = file - 1;
        loaded = 1;
    }
    return;
}

// Reads pub/dtd001.edf ( it starts EDF ): each npc's drops, 10 bytes a drop.
void NpcValues::LoadDrops()
{
    int file_handle;
    if (!drops_loaded)
    {
        String data;
        String path = "./pub/dtd001.edf";
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
            if (data[1] != 'E' || data[2] != 'D' || data[3] != 'F')
                return;
            data.Delete(1, 3);
            do
            {
                int npc_id = DecodeNumber(data.SubString(1, 2));
                int drops_count = DecodeNumber(data.SubString(3, 2));
                data.Delete(1, 4);
                for (int i = 0; i < drops_count; i++)
                {
                    AddDrop(npc_id, DecodeNumber(data.SubString(1, 2)), DecodeNumber(data.SubString(3, 3)),
                        DecodeNumber(data.SubString(6, 3)), DecodeNumber(data.SubString(9, 2)));
                    data.Delete(1, 10);
                }
            } while (data.Length() > 3);
            drops_loaded = true;
        }
        catch (...)
        {
            FileClose(file_handle);
            drops_loaded = false;
        }
    }
}

// Reads pub/ttd001.etf ( it starts ETF ): each npc's talk rate and lines.
void NpcValues::LoadTalk()
{
    int file_handle;
    if (!talk_loaded)
    {
        String data;
        String path = "./pub/ttd001.etf";
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
            if (data[1] != 'E' || data[2] != 'T' || data[3] != 'F')
                return;
            data.Delete(1, 3);
            do
            {
                int npc_id = DecodeNumber(data.SubString(1, 2));
                int rate = DecodeNumber(data.SubString(3, 1));
                int messages_count = DecodeNumber(data.SubString(4, 1));
                data.Delete(1, 4);
                for (int i = 0; i < messages_count; i++)
                {
                    int message_length = DecodeNumber(data.SubString(1, 1));
                    SetTalk(npc_id, rate, data.SubString(2, message_length));
                    data.Delete(1, message_length + 1);
                }
            } while (data.Length() > 3);
            talk_loaded = true;
        }
        catch (...)
        {
            FileClose(file_handle);
            talk_loaded = false;
        }
    }
}

void NpcValues::AddDrop(int npc_id, int item_id, int min_amount, int max_amount, int rate)
{
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (npc_id == it->id)
        {
            it->AddDrop(item_id, min_amount, max_amount, rate);
            return;
        }
    }
}

void NpcValues::SetTalk(int npc_id, int rate, String message)
{
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (npc_id == it->id)
        {
            it->AddTalkMessage(rate, message);
            break;
        }
    }
}

// A random talk line of the npc, by its talk rate ( percent ), or "".
String NpcValues::RollTalk(int enf_id)
{
    int count;
    int roll;
    String result = "";
    if (enf_id > 0 && record_list.size() >= (unsigned int)enf_id)
    {
        if (record_list[enf_id - 1].has_talk)
        {
            count = (int)record_list[enf_id - 1].talk_lines.size();
            if (count > 0)
            {
                roll = random(100);
                if (record_list[enf_id - 1].talk_rate > roll)
                {
                    roll = random(count);
                    result = record_list[enf_id - 1].talk_lines[roll];
                }
            }
        }
    }
    return result;
}

// Rolls the npc's drop: a roll of 0 to 63999 against the summed drop rates; item 0 when nothing drops.
MapCoord NpcValues::GetDrop(int npc_id)
{
    MapCoord result;
    result.item_id = 0;
    result.amount = 0;
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (it->id != npc_id)
            continue;
        int roll = random(64000);
        int sum = 0;
        for (vector<NpcDropItem>::iterator drop = it->drops.begin(); drop != it->drops.end(); drop++)
        {
            sum += drop->rate;
            if (sum >= roll)
            {
                if (drop->min_amount > 0)
                {
                    int range = drop->max_amount - drop->min_amount;
                    result.item_id = drop->item_id;
                    if (range > 0)
                        result.amount = drop->min_amount + random(range + 1);
                    else
                        result.amount = drop->min_amount;
                }
                break;
            }
        }
    }
    return result;
}

void NpcValues::AddNpc(int id, String name, short graphic_id, short race, short boss, short child, short npc_type,
    short behavior_id, int hp, short tp, short min_damage, short max_damage, short accuracy, short evade, short armor,
    short return_damage, short element, short element_damage, short element_weakness, short element_weakness_damage,
    short level, int experience)
{
    NpcValue value(id);
    value.id = id;
    value.name = name;
    value.graphic_id = graphic_id;
    value.race = race;
    value.boss = boss;
    value.child = child;
    value.npc_type = npc_type;
    value.behavior_id = behavior_id;
    value.hp = hp;
    value.tp = tp;
    value.min_damage = min_damage;
    value.max_damage = max_damage;
    value.accuracy = accuracy;
    value.evade = evade;
    value.armor = armor;
    value.return_damage = return_damage;
    value.element = element;
    value.element_damage = element_damage;
    value.element_weakness = element_weakness;
    value.element_weakness_damage = element_weakness_damage;
    value.level = level;
    value.experience = experience;
    if (value.hp < 1)
        value.hp = 1;
    record_list.insert(record_list.end(), value);
}

void Npcvalues_Clear(vector<NpcDropItem> &list)
{
    list.clear();
}

NpcValue NpcValues::GetNpc(int id)
{
    NpcValue result;
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (it->id == id)
            result = *it;
    }
    return result;
}

int NpcValues::GetExp(int npc_id)
{
    int result = 0;
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (npc_id == it->id)
        {
            result = it->experience;
            break;
        }
    }
    return result;
}

int NpcValues::GetMaxHp(int npc_id)
{
    int result = 1;
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (npc_id == it->id)
        {
            result = it->hp;
            break;
        }
    }
    return result;
}

// The npc's type and behavior id, or NpcType_Invalid and -1.
MapCoord NpcValues::GetType(int enf_id)
{
    MapCoord result;
    result.type = NpcType_Invalid;
    result.behavior_id = -1;
    for (vector<NpcValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (enf_id == it->id)
        {
            result.type = it->npc_type;
            result.behavior_id = it->behavior_id;
            break;
        }
    }
    return result;
}

int NpcValues::GetCount()
{
    return record_list.size();
}

// Reads a number of up to four bytes; a 254 ends it.
DEFINE_DECODE_NUMBER(NpcValues)
