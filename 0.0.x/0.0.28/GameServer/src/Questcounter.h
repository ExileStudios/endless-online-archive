#ifndef QuestcounterH
#define QuestcounterH

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

#include "Questcounterlist.h"

struct QuestCounter;

// The finish counts of one quest, by player name ( lower case ).
struct QuestCounter
{
    int quest_id;
    TStringList *string_list;
    String current_line;
    vector<QuestCounterList> counters;

    QuestCounter(int quest_id);
    ~QuestCounter();
    void Clear();
    int Increment(String name);
    int Get(String name);
    void Set(String name, int count);
    void Load();
    String Extract();
    void Save();
};


#endif
