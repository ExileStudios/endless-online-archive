#ifndef LearnitemH
#define LearnitemH

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

// A spell a skill master teaches: its price and what it needs.
struct LearnItemVal
{
    int id;
    short level_requirement;
    short class_requirement;
    int price;
    short skill_requirement_1;
    short skill_requirement_2;
    short skill_requirement_3;
    short skill_requirement_4;
    short str_requirement;
    short int_requirement;
    short wis_requirement;
    short agi_requirement;
    short con_requirement;
    short cha_requirement;

    LearnItemVal();
    LearnItemVal(int id);
    ~LearnItemVal();
};


#endif
