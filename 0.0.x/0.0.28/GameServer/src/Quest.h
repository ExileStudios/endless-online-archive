#ifndef QuestH
#define QuestH

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

struct QuestState;

#include "Queststate.h"

struct Quest;

// A quest file: its name, version and states.
struct Quest
{
    int quest_id;
    String name;
    int version;
    int state_count;
    char loaded;
    vector<QuestState *> states;

    Quest(int quest_id);
    ~Quest();
};


#endif
