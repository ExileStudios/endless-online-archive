#ifndef QuestcountersH
#define QuestcountersH

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

struct QuestCounter;

#include "Questcounter.h"

struct QuestCounters;

// How often each player finished each quest, one cache/quest<id>.chk file per quest.
struct QuestCounters
{
    vector<QuestCounter> counters;

    QuestCounters();
    ~QuestCounters();
    void Clear();
    void Load();
    void Save();
    int RecordCompletion(String name, int quest_id);
    int GetCompletionCount(String player_name, int quest_id);
};


#endif
