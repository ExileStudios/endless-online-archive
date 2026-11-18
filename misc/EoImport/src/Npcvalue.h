#ifndef NpcvalueH
#define NpcvalueH

#include <new>
#include <vector.h>

#include "Npcdrop.h"

// One NPC, with its drops and its chat lines.
struct NpcValue
{
    int npc_id;
    String name;
    short graphic;
    char race;
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
    char level;
    int experience;
    unsigned char talk_interval;
    unsigned char talk_rate;
    vector<NpcDropItem> drops;
    vector<AnsiString> talk_lines;

    NpcValue(int id);
    ~NpcValue();
    void AddDrop(int item_id, int min_amount, int max_amount, int rate);
};

#endif
