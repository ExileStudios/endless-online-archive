#ifndef LearnvaluesH
#define LearnvaluesH

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
struct LearnValue;

#include "Learnvalue.h"

struct LearnValues;

// The skill masters, read from pub/dsm001.emf.
struct LearnValues
{
    char loaded;
    vector<LearnValue> record_list;
    int record_index;

    LearnValues();
    ~LearnValues();
    void LoadSkillMasters();
    void AddSkill(LearnValue *record, int skill_id, int level_requirement, int class_requirement, int price,
        int skill_requirement_1, int skill_requirement_2, int skill_requirement_3, int skill_requirement_4,
        int str_requirement, int int_requirement, int wis_requirement, int agi_requirement, int con_requirement,
        int cha_requirement);
    void Clear();
    char HasSkill(int master_id, int skill_id);
    LearnItemVal GetSkill(int master_id, int skill_id);
    String BuildOpenData(int behavior_id);
    unsigned int GetCount();
    String EncodeNumber(unsigned int value, int width);
    int DecodeNumber(String value);
};


#endif
