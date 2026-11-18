#include <vcl.h>
#pragma hdrstop

#include "Learnvalue.h"
#include "Learnitem.h"

#pragma package(smart_init)

LearnValue::LearnValue()
{
    skills.clear();
}

LearnValue::LearnValue(int id)
{
    this->id = id;
    this->skills.clear();
}

LearnValue::~LearnValue()
{
}

// Adds a skill to the master, or replaces the requirements of one it already teaches.
void LearnValue::AddSkill(int skill_id, int level_requirement, int class_requirement, int price,
    int skill_requirement_1, int skill_requirement_2, int skill_requirement_3, int skill_requirement_4,
    int str_requirement, int int_requirement, int wis_requirement, int agi_requirement, int con_requirement,
    int cha_requirement)
{
    char found = false;
    for (vector<LearnItemVal>::iterator known = skills.begin(); known != skills.end(); known++)
    {
        if (skill_id != known->id)
            continue;
        found = true;
        known->level_requirement = level_requirement;
        known->class_requirement = class_requirement;
        known->price = price;
        known->skill_requirement_1 = skill_requirement_1;
        known->skill_requirement_2 = skill_requirement_2;
        known->skill_requirement_3 = skill_requirement_3;
        known->skill_requirement_4 = skill_requirement_4;
        known->str_requirement = str_requirement;
        known->int_requirement = int_requirement;
        known->wis_requirement = wis_requirement;
        known->agi_requirement = agi_requirement;
        known->con_requirement = con_requirement;
        known->cha_requirement = cha_requirement;
    }
    if (!found)
    {
        LearnItemVal value(skill_id);
        value.level_requirement = level_requirement;
        value.class_requirement = class_requirement;
        value.price = price;
        value.skill_requirement_1 = skill_requirement_1;
        value.skill_requirement_2 = skill_requirement_2;
        value.skill_requirement_3 = skill_requirement_3;
        value.skill_requirement_4 = skill_requirement_4;
        value.str_requirement = str_requirement;
        value.int_requirement = int_requirement;
        value.wis_requirement = wis_requirement;
        value.agi_requirement = agi_requirement;
        value.con_requirement = con_requirement;
        value.cha_requirement = cha_requirement;
        skills.insert(skills.end(), value);
    }
}
