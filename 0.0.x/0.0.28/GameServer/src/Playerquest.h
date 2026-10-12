#ifndef PlayerquestH
#define PlayerquestH

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
#define QUEST_COUNTER_SLOTS 5



struct PlayerQuest;

// A quest a player is on: its state, its file's version and the counts of its rules.
struct PlayerQuest
{
    int quest_id;
    short state_index;
    short version;
    short counters[QUEST_COUNTER_SLOTS];
    char done;

    PlayerQuest(int quest_id, int state_index, int version);
    ~PlayerQuest();
};


#endif
