#ifndef QuestcounterlistH
#define QuestcounterlistH

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



struct QuestCounterList;

// How many times a player finished a quest today.
struct QuestCounterList
{
    String name;
    int count;

    QuestCounterList(String name);
    QuestCounterList(String name, int count);
    ~QuestCounterList();
};


#endif
