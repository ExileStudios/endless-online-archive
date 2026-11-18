#include <vcl.h>
#pragma hdrstop

#include "Learnvalue.h"

#pragma package(smart_init)

LearnValue::LearnValue()
{
}

void Learnvalue_UnusedInstances1(std::vector<LearnItemVal> &items)
{
    items.clear();
}

LearnValue::LearnValue(int record_id)
{
    id = record_id;
    behavior_id = record_id;
    skills.clear();
}

LearnValue::~LearnValue()
{
}

// Sets the skill's price and needs when the master already teaches it, else adds the skill.
void LearnValue::AddSkill(int skill_id, int level_requirement, int needed_class, int price, int skill_requirement_1,
    int skill_requirement_2, int skill_requirement_3, int skill_requirement_4, int str_requirement, int wis_requirement,
    int int_requirement, int agi_requirement, int con_requirement, int cha_requirement)
{
    char found = false;
    for (vector<LearnItemVal>::iterator known = skills.begin(); known != skills.end(); known++)
    {
        if (skill_id == known->id)
        {
            known->level_requirement = level_requirement;
            found = true;
            known->class_requirement = needed_class;
            known->price = price;
            known->skill_requirement_1 = skill_requirement_1;
            known->skill_requirement_2 = skill_requirement_2;
            known->skill_requirement_3 = skill_requirement_3;
            known->skill_requirement_4 = skill_requirement_4;
            known->str_requirement = str_requirement;
            known->wis_requirement = wis_requirement;
            known->int_requirement = int_requirement;
            known->agi_requirement = agi_requirement;
            known->con_requirement = con_requirement;
            known->cha_requirement = cha_requirement;
        }
    }
    if (!found)
    {
        LearnItemVal skill(skill_id);
        skill.level_requirement = level_requirement;
        skill.class_requirement = needed_class;
        skill.price = price;
        skill.skill_requirement_1 = skill_requirement_1;
        skill.skill_requirement_2 = skill_requirement_2;
        skill.skill_requirement_3 = skill_requirement_3;
        skill.skill_requirement_4 = skill_requirement_4;
        skill.str_requirement = str_requirement;
        skill.wis_requirement = wis_requirement;
        skill.int_requirement = int_requirement;
        skill.agi_requirement = agi_requirement;
        skill.con_requirement = con_requirement;
        skill.cha_requirement = cha_requirement;
        skills.insert(skills.end(), skill);
    }
}
