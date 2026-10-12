#ifndef NpcvalueH
#define NpcvalueH

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

struct NpcDropItem;

#include "Npcdrop.h"

struct NpcValue;

// An npc's record: its stats, drops and talk lines.
struct NpcValue
{
    int id;
    String name;
    short graphic_id;
    short race;
    short boss;
    short child;
    short npc_type;
    short behavior_id;
    int hp;
    short tp;
    short min_damage;
    short max_damage;
    short accuracy;
    short evade;
    short armor;
    short return_damage;
    short element;
    short element_damage;
    short element_weakness;
    short element_weakness_damage;
    short level;
    int experience;
    int talk_rate;
    bool has_talk;
    vector<NpcDropItem> drops;
    vector<String> talk_lines;

    NpcValue();
    NpcValue(int id);
    ~NpcValue();
    void AddDrop(int item_id, int min_amount, int max_amount, int rate);
    void AddTalkMessage(int rate, String message);
};


#endif
