#ifndef LearnvalueH
#define LearnvalueH

#include <new>
#include <vector.h>

#include "Learnitem.h"

// One skill master: its name, the levels and class it serves, and the skills it teaches.
struct LearnValue
{
    int id;
    int behavior_id;
    String name;
    short min_level;
    short max_level;
    short class_requirement;
    vector<LearnItemVal> skills;

    LearnValue();
    LearnValue(int record_id);
    ~LearnValue();
    void AddSkill(int skill_id, int level_requirement, int needed_class, int price, int skill_requirement_1,
    int skill_requirement_2, int skill_requirement_3, int skill_requirement_4, int str_requirement, int wis_requirement,
    int int_requirement, int agi_requirement, int con_requirement, int cha_requirement);
};

#endif
