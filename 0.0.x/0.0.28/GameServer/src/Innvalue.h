#ifndef InnvalueH
#define InnvalueH

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
#define INN_QUESTION_COUNT 3



struct InnValue;

// An inn: where its citizens spawn and sleep, the alternate spawn, and its three questions and answers.
struct InnValue
{
    int id;
    String name;
    short spawn_map;
    short spawn_x;
    short spawn_y;
    short sleep_map;
    short sleep_x;
    short sleep_y;
    short alternate_spawn_level;
    short alternate_spawn_map;
    short alternate_spawn_x;
    short alternate_spawn_y;
    String question[INN_QUESTION_COUNT];
    String answer[INN_QUESTION_COUNT];

    InnValue(int id);
    ~InnValue();
};


#endif
