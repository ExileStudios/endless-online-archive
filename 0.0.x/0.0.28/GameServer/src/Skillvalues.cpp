#include <vcl.h>
#pragma hdrstop

#include "Skillvalues.h"
#include "Shopvalues.h"
#include "Skillvalue.h"
#include "Constants.h"

#pragma package(smart_init)

SkillValues::SkillValues()
{
    encode_scratch = new char[8];
    record_index = -1;
    loaded = 0;
    file_id = 0;
    string_list = new TStringList();
    LoadSpells();
}

SkillValues::~SkillValues()
{
}

// Reads pub/dsl001.esf and the files after it until every spell is loaded.
void SkillValues::LoadSpells()
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
            String path = "./pub/dsl";
            if (file < 10)
                path = path + "00" + IntToStr(file) + ".esf";
            else
                path = path + "0" + IntToStr(file) + ".esf";
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
                // a file without the ESF header ends the loading
                if (data[1] != 'E' || data[2] != 'S' || data[3] != 'F')
                    return;
                string_list->Add(data);
                // .. the first file's header holds the number of spells
                if (file == 1)
                {
                    rid[0] = DecodeNumber(data.SubString(4, 2));
                    rid[1] = DecodeNumber(data.SubString(6, 2));
                    int parsed = DecodeNumber(data.SubString(8, 2));
                    int version = DecodeNumber(data.SubString(10, 1));
                    total = parsed;
                    num_records = parsed;
                }
                data.Delete(1, 10);
                for (int j = 0; count < total && j < 900; j++)
                {
                    int namelen = DecodeNumber(data.SubString(1, 1));
                    int chantlen = DecodeNumber(data.SubString(2, 1));
                    int base = namelen + chantlen + 2;
                    AddRecord(GetCount() + 1, data.SubString(3, namelen), data.SubString(namelen + 3, chantlen),
                        DecodeNumber(data.SubString(base + 1, 2)), DecodeNumber(data.SubString(base + 3, 2)),
                        DecodeNumber(data.SubString(base + 5, 2)), DecodeNumber(data.SubString(base + 7, 2)),
                        DecodeNumber(data.SubString(base + 9, 1)), DecodeNumber(data.SubString(base + 10, 1)),
                        DecodeNumber(data.SubString(base + 11, 1)), DecodeNumber(data.SubString(base + 12, 3)),
                        DecodeNumber(data.SubString(base + 15, 1)), DecodeNumber(data.SubString(base + 16, 2)),
                        DecodeNumber(data.SubString(base + 18, 1)), DecodeNumber(data.SubString(base + 19, 1)),
                        DecodeNumber(data.SubString(base + 20, 1)), DecodeNumber(data.SubString(base + 21, 1)),
                        DecodeNumber(data.SubString(base + 22, 2)), DecodeNumber(data.SubString(base + 24, 2)),
                        DecodeNumber(data.SubString(base + 26, 2)), DecodeNumber(data.SubString(base + 28, 2)),
                        DecodeNumber(data.SubString(base + 30, 2)), DecodeNumber(data.SubString(base + 32, 2)),
                        DecodeNumber(data.SubString(base + 34, 1)), DecodeNumber(data.SubString(base + 35, 2)),
                        DecodeNumber(data.SubString(base + 37, 2)), DecodeNumber(data.SubString(base + 39, 1)),
                        DecodeNumber(data.SubString(base + 40, 2)), DecodeNumber(data.SubString(base + 42, 2)),
                        DecodeNumber(data.SubString(base + 44, 2)), DecodeNumber(data.SubString(base + 46, 2)),
                        DecodeNumber(data.SubString(base + 48, 2)), DecodeNumber(data.SubString(base + 50, 2)));
                    count = count + 1;
                    data.Delete(1, base + 51);
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

void SkillValues::AddRecord(int id, String name, String chant, short icon_id, short graphic_id, short tp_cost,
    short sp_cost, short cast_time, short nature, short esf_constant, short skill_type, short element,
    short element_power, short target_restrict, short target_type, short target_time, short skill_range_area,
    short max_skill_level, short min_damage, short max_damage, short accuracy, short evade, short armor,
    short return_damage, short hp_heal, short tp_heal, short sp_heal, short str, short intl, short wis, short agi,
    short con, short cha)
{
    SkillValue record(id);
    record.name = name;
    record.chant = chant;
    record.icon_id = icon_id;
    record.graphic_id = graphic_id;
    record.max_skill_level = max_skill_level;
    record.tp_cost = tp_cost;
    record.sp_cost = sp_cost;
    record.cast_time = cast_time;
    record.nature = nature;
    record.esf_constant = esf_constant;
    record.skill_type = skill_type;
    record.element = element;
    record.element_power = element_power;
    record.target_restrict = target_restrict;
    record.target_type = target_type;
    record.target_time = target_time;
    record.skill_range_area = skill_range_area;
    record.min_damage = min_damage;
    record.max_damage = max_damage;
    record.accuracy = accuracy;
    record.evade = evade;
    record.armor = armor;
    record.return_damage = return_damage;
    record.hp_heal = hp_heal;
    record.tp_heal = tp_heal;
    record.sp_heal = sp_heal;
    record.str = str;
    record.intl = intl;
    record.wis = wis;
    record.agi = agi;
    record.con = con;
    record.cha = cha;
    record_list.insert(record_list.end(), record);
}

SkillValue &Skillvalues_Slot(vector<SkillValue> &list, int index)
{
    return list[index];
}

// The spell's min and max damage.
MapCoord SkillValues::GetDamage(int skill_id)
{
    MapCoord result;
    result.min_damage = 0;
    result.max_damage = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result.min_damage = record_list[skill_id - 1].min_damage;
        result.max_damage = record_list[skill_id - 1].max_damage;
    }
    return result;
}

// The spell's element and its power.
MapCoord SkillValues::GetElement(int skill_id)
{
    MapCoord result;
    result.x = 0;
    result.y = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result.x = record_list[skill_id - 1].element;
        result.y = record_list[skill_id - 1].element_power;
    }
    return result;
}

int SkillValues::GetTargetType(int skill_id)
{
    int result = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result = record_list[skill_id - 1].target_type;
        if (result < 0)
            result = 0;
    }
    return result;
}

int SkillValues::GetSkillType(int skill_id)
{
    int result = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result = record_list[skill_id - 1].skill_type;
        if (result < 0)
            result = 0;
    }
    return result;
}

int SkillValues::GetTpCost(int skill_id)
{
    int result = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result = record_list[skill_id - 1].tp_cost;
        if (result < 0)
            result = 0;
    }
    return result;
}

int SkillValues::GetHpHeal(int skill_id)
{
    int result = 0;
    if (skill_id > 0 && GetCount() > skill_id)
    {
        result = record_list[skill_id - 1].hp_heal;
        if (result < 0)
            result = 0;
    }
    return result;
}

// The spell's cast time, or -1 for an unknown spell.
int SkillValues::GetCastTime(int skill_id)
{
    int result = -1;
    for (vector<SkillValue>::iterator it = record_list.begin(); it != record_list.end(); it++)
    {
        if (skill_id == it->id)
        {
            result = it->cast_time;
            break;
        }
    }
    return result;
}

int SkillValues::GetCount()
{
    return record_list.size();
}

// Reads a number of up to four bytes; a 254 ends it.
DEFINE_DECODE_NUMBER(SkillValues)
