#ifndef SkillvalueH
#define SkillvalueH

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



struct SkillValue;

// A spell from the pub spell file: its costs, cast time, target and damage.
struct SkillValue
{
    int id;
    String name;
    String chant;
    short max_skill_level;
    short icon_id;
    short graphic_id;
    short tp_cost;
    short sp_cost;
    short cast_time;
    short nature;
    short esf_constant;
    int skill_type;
    short element;
    short element_power;
    short target_restrict;
    short target_type;
    short target_time;
    short skill_range_area;
    short min_damage;
    short max_damage;
    short accuracy;
    short evade;
    short armor;
    short return_damage;
    short hp_heal;
    short tp_heal;
    short sp_heal;
    short str;
    short intl;
    short wis;
    short agi;
    short con;
    short cha;

    SkillValue(int id);
    ~SkillValue();
};


#endif
