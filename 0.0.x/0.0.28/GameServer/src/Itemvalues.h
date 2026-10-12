#ifndef ItemvaluesH
#define ItemvaluesH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

enum ElementKind
{
    ElementKind_None = 0,
    ElementKind_Light = 1,
    ElementKind_Dark = 2,
    ElementKind_Earth = 3,
    ElementKind_Wind = 4,
    ElementKind_Water = 5,
    ElementKind_Fire = 6
};

struct ItemValue;
struct MapCoord;

#include "Itemvalue.h"

struct ItemValues;

// The items, read from the pub/dat*.eif files.
struct ItemValues
{
    int file_id;
    int num_records;
    int rid[2];
    char loaded;
    TStringList *string_list;
    void *encode_scratch;
    vector<ItemValue *> record_list;
    int record_index;

    ItemValues();
    ~ItemValues();
    void LoadItems();
    void AddItem(int id, String name, int graphic_id, short type, short subtype, short special, short hp, short tp,
        short min_damage, short max_damage, short accuracy, short evade, short armor, short return_damage,
        short strength, short intelligence, short wisdom, short agility, short constitution, short charisma,
        short light_resistance, short dark_resistance, short earth_resistance, short air_resistance,
        short water_resistance, short fire_resistance, int spec1, short spec2, short spec3, short level_requirement,
        short class_requirement, short strength_requirement, short intelligence_requirement, short wisdom_requirement,
        short agility_requirement, short constitution_requirement, short charisma_requirement, short element,
        short element_damage, short weight, short weapon_target_area, short size);
    void Clear();
    ItemValue *GetByIndex(int index);
    int GetCount();
    int GetSpec1ForTypes(int item_id);
    MapCoord GetElement(int item_id);
    MapCoord GetSpecXY(int item_id);
    int GetSpec1(int item_id);
    int GetScrollMap(int item_id);
    int GetGender(int item_id);
    int GetType(int item_id);
    int GetSubtype(int item_id);
    int GetLevelRequirement(int item_id);
    int GetSpecial(int item_id);
    int GetWeight(int item_id);
    int GetHP(int item_id);
    int GetTP(int item_id);
    int DecodeNumber(String value);
};


#endif
