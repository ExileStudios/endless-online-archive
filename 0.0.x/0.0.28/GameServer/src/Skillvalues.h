#ifndef SkillvaluesH
#define SkillvaluesH

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

struct MapCoord;
struct SkillValue;

#include "Skillvalue.h"

struct SkillValues;

// The spells, read from the pub/dsl*.esf files.
struct SkillValues
{
    int file_id;
    int num_records;
    int rid[2];
    char loaded;
    TStringList *string_list;
    void *encode_scratch;
    vector<SkillValue> record_list;
    int record_index;

    SkillValues();
    ~SkillValues();
    void LoadSpells();
    void AddRecord(int id, String name, String chant, short icon_id, short graphic_id, short tp_cost, short sp_cost,
        short cast_time, short nature, short esf_constant, short skill_type, short element, short element_power,
        short target_restrict, short target_type, short target_time, short skill_range_area, short max_skill_level,
        short min_damage, short max_damage, short accuracy, short evade, short armor, short return_damage,
        short hp_heal, short tp_heal, short sp_heal, short str, short intl, short wis, short agi, short con, short cha);
    MapCoord GetDamage(int skill_id);
    MapCoord GetElement(int skill_id);
    int GetTargetType(int skill_id);
    int GetSkillType(int skill_id);
    int GetTpCost(int skill_id);
    int GetHpHeal(int skill_id);
    int GetCastTime(int skill_id);
    int GetCount();
    int DecodeNumber(String value);
};


#endif
