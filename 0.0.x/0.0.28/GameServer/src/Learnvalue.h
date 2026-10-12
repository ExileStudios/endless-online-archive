#ifndef LearnvalueH
#define LearnvalueH

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

struct LearnItemVal;

#include "Learnitem.h"

struct LearnValue;

// A skill master: the skills it teaches and what each needs.
struct LearnValue
{
    int id;
    String name;
    short min_level;
    short max_level;
    short class_requirement;
    vector<LearnItemVal> skills;

    LearnValue();
    LearnValue(int id);
    ~LearnValue();
    void AddSkill(int skill_id, int level_requirement, int class_requirement, int price, int skill_requirement_1,
        int skill_requirement_2, int skill_requirement_3, int skill_requirement_4, int str_requirement,
        int int_requirement, int wis_requirement, int agi_requirement, int con_requirement, int cha_requirement);
};


#endif
