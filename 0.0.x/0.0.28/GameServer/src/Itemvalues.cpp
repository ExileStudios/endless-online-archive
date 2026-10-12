#include <vcl.h>
#pragma hdrstop

#include "Itemvalues.h"
#include "Itemvalue.h"
#include "Shopvalues.h"
#include "Constants.h"

#pragma package(smart_init)

ItemValues::ItemValues()
{
    encode_scratch = new char[8];
    record_index = -1;
    loaded = 0;
    rid[0] = -1;
    rid[1] = -1;
    file_id = 0;
    string_list = new TStringList();
    LoadItems();
}

ItemValues::~ItemValues()
{
}

// Reads pub/dat001.eif and the files after it until every item is loaded.
void ItemValues::LoadItems()
{
    int file;
    int count;
    int total;
    int file_handle;
    if (!loaded)
    {
        string_list->Clear();
        Clear();
        file = 1;
        count = 0;
        total = 1;
        do
        {
            String path;
            String data;
            path.Insert("./pub/", 0);
            path.Insert("dat", path.Length() + 1);
            if (file < 10)
                path = path + "00" + IntToStr(file) + ".eif";
            else
                path = path + "0" + IntToStr(file) + ".eif";
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
                // a file without the EIF header ends the loading
                if (data[1] != 'E' || data[2] != 'I' || data[3] != 'F')
                    return;
                string_list->Add(data);
                // .. the first file's header holds the number of items
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
                    int namelen = DecodeNumber(data.SubString(1, 1)) + 1;
                    AddItem(GetCount() + 1, data.SubString(2, namelen - 1),
                        DecodeNumber(data.SubString(namelen + 1, 2)), DecodeNumber(data.SubString(namelen + 3, 1)),
                        DecodeNumber(data.SubString(namelen + 4, 1)), DecodeNumber(data.SubString(namelen + 5, 1)),
                        DecodeNumber(data.SubString(namelen + 6, 2)), DecodeNumber(data.SubString(namelen + 8, 2)),
                        DecodeNumber(data.SubString(namelen + 10, 2)), DecodeNumber(data.SubString(namelen + 12, 2)),
                        DecodeNumber(data.SubString(namelen + 14, 2)), DecodeNumber(data.SubString(namelen + 16, 2)),
                        DecodeNumber(data.SubString(namelen + 18, 2)), DecodeNumber(data.SubString(namelen + 20, 1)),
                        DecodeNumber(data.SubString(namelen + 21, 1)), DecodeNumber(data.SubString(namelen + 22, 1)),
                        DecodeNumber(data.SubString(namelen + 23, 1)), DecodeNumber(data.SubString(namelen + 24, 1)),
                        DecodeNumber(data.SubString(namelen + 25, 1)), DecodeNumber(data.SubString(namelen + 26, 1)),
                        DecodeNumber(data.SubString(namelen + 27, 1)), DecodeNumber(data.SubString(namelen + 28, 1)),
                        DecodeNumber(data.SubString(namelen + 29, 1)), DecodeNumber(data.SubString(namelen + 30, 1)),
                        DecodeNumber(data.SubString(namelen + 31, 1)), DecodeNumber(data.SubString(namelen + 32, 1)),
                        DecodeNumber(data.SubString(namelen + 33, 3)), DecodeNumber(data.SubString(namelen + 36, 1)),
                        DecodeNumber(data.SubString(namelen + 37, 1)), DecodeNumber(data.SubString(namelen + 38, 2)),
                        DecodeNumber(data.SubString(namelen + 40, 2)), DecodeNumber(data.SubString(namelen + 41, 2)),
                        DecodeNumber(data.SubString(namelen + 44, 2)), DecodeNumber(data.SubString(namelen + 46, 2)),
                        DecodeNumber(data.SubString(namelen + 48, 2)), DecodeNumber(data.SubString(namelen + 50, 2)),
                        DecodeNumber(data.SubString(namelen + 52, 2)), DecodeNumber(data.SubString(namelen + 54, 1)),
                        DecodeNumber(data.SubString(namelen + 55, 1)), DecodeNumber(data.SubString(namelen + 56, 1)),
                        DecodeNumber(data.SubString(namelen + 57, 1)), DecodeNumber(data.SubString(namelen + 58, 1)));
                    count++;
                    data.Delete(1, namelen + 58);
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

void ItemValues::AddItem(int id, String name, int graphic_id, short type, short subtype, short special, short hp,
    short tp, short min_damage, short max_damage, short accuracy, short evade, short armor, short return_damage,
    short strength, short intelligence, short wisdom, short agility, short constitution, short charisma,
    short light_resistance, short dark_resistance, short earth_resistance, short air_resistance, short water_resistance,
    short fire_resistance, int spec1, short spec2, short spec3, short level_requirement, short class_requirement,
    short strength_requirement, short intelligence_requirement, short wisdom_requirement, short agility_requirement,
    short constitution_requirement, short charisma_requirement, short element, short element_damage, short weight,
    short weapon_target_area, short size)
{
    ItemValue *value = new ItemValue(id);
    value->type = type;
    value->subtype = subtype;
    value->special = special;
    value->hp = hp;
    value->tp = tp;
    value->min_damage = min_damage;
    value->max_damage = max_damage;
    value->accuracy = accuracy;
    value->evade = evade;
    value->armor = armor;
    value->return_damage = return_damage;
    value->strength = strength;
    value->intelligence = intelligence;
    value->wisdom = wisdom;
    value->agility = agility;
    value->constitution = constitution;
    value->charisma = charisma;
    value->light_resistance = light_resistance;
    value->dark_resistance = dark_resistance;
    value->earth_resistance = earth_resistance;
    value->air_resistance = air_resistance;
    value->water_resistance = water_resistance;
    value->fire_resistance = fire_resistance;
    value->spec1 = spec1;
    value->spec2 = spec2;
    value->spec3 = spec3;
    value->level_requirement = level_requirement;
    value->class_requirement = class_requirement;
    value->strength_requirement = strength_requirement;
    value->intelligence_requirement = intelligence_requirement;
    value->wisdom_requirement = wisdom_requirement;
    value->agility_requirement = agility_requirement;
    value->constitution_requirement = constitution_requirement;
    value->charisma_requirement = charisma_requirement;
    value->size = size;
    value->weapon_target_area = weapon_target_area;
    value->weight = weight;
    value->element = element;
    value->element_damage = element_damage;
    record_list.insert(record_list.end(), value);
}

void ItemValues::Clear()
{
    for (vector<ItemValue *>::iterator it = record_list.begin(); it != record_list.end(); )
    {
        ItemValue *value = *it;
        it = record_list.erase(it);
        delete value;
    }
    record_index = -1;
}

ItemValue * &Itemvalues_Slot(vector<ItemValue *> &list, int index)
{
    return list[index];
}

// The item at index; one out of range gives the first item.
ItemValue * ItemValues::GetByIndex(int index)
{
    if (index < 0 || record_list.size() - 1 < (unsigned int)index)
        index = 0;
    return record_list[index];
}

int ItemValues::GetCount()
{
    return record_list.size();
}

// An equipment item's spec1 ( weapon to bracer ), else 0.
int ItemValues::GetSpec1ForTypes(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id && record_list[item_id - 1]->type >= EifItemType_Weapon &&
        record_list[item_id - 1]->type <= EifItemType_Bracer)
    {
        result = record_list[item_id - 1]->spec1;
        if (result < 0)
            result = 0;
    }
    return result;
}

// A weapon's element and element damage.
MapCoord ItemValues::GetElement(int item_id)
{
    MapCoord result;
    result.element = ElementKind_None;
    result.element_damage = 0;
    if (item_id > 0 && GetCount() > item_id && record_list[item_id - 1]->type == EifItemType_Weapon)
    {
        result.element = record_list[item_id - 1]->element;
        result.element_damage = record_list[item_id - 1]->element_damage;
    }
    return result;
}

// A teleport scroll's x and y ( spec2 and spec3 ), else -1 and -1.
MapCoord ItemValues::GetSpecXY(int item_id)
{
    MapCoord result;
    result.spec2 = -1;
    result.spec3 = -1;
    if (item_id > 0 && GetCount() > item_id && record_list[item_id - 1]->type == EifItemType_Teleport)
    {
        result.spec2 = record_list[item_id - 1]->spec2;
        result.spec3 = record_list[item_id - 1]->spec3;
    }
    return result;
}

int ItemValues::GetSpec1(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->spec1;
        if (result < 0)
            result = 0;
    }
    return result;
}

// A teleport scroll's map ( spec1 ), else 0.
int ItemValues::GetScrollMap(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id && record_list[item_id - 1]->type == EifItemType_Teleport)
    {
        result = record_list[item_id - 1]->spec1;
        if (result < 0)
            result = 0;
    }
    return result;
}

// The gender an armor is for ( spec2 ), else -1.
int ItemValues::GetGender(int item_id)
{
    int result = -1;
    if (item_id > 0 && GetCount() > item_id && record_list[item_id - 1]->type == EifItemType_Armor)
    {
        result = record_list[item_id - 1]->spec2;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetType(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->type;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetSubtype(int item_id)
{
    if (item_id > 0 && GetCount() > item_id)
        return record_list[item_id - 1]->subtype;
    else
        return 0;
}

int ItemValues::GetLevelRequirement(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->level_requirement;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetSpecial(int item_id)
{
    int result = -1;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->special;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetWeight(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->weight;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetHP(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->hp;
        if (result < 0)
            result = 0;
    }
    return result;
}

int ItemValues::GetTP(int item_id)
{
    int result = 0;
    if (item_id > 0 && GetCount() > item_id)
    {
        result = record_list[item_id - 1]->tp;
        if (result < 0)
            result = 0;
    }
    return result;
}

// Reads a number of up to four bytes; a 254 ends it.
DEFINE_DECODE_NUMBER(ItemValues)
