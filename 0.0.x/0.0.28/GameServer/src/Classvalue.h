#ifndef ClassvalueH
#define ClassvalueH

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



struct ClassValue;

// A class from the pub class file: its stat group and the stats it adds.
struct ClassValue
{
    int id;
    int parent_type;
    String name;
    short stat_group;
    short str;
    short intl;
    short wis;
    short agi;
    short con;
    short cha;

    ClassValue(int id);
    ~ClassValue();
};


#endif
