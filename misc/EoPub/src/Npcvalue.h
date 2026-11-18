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
    unsigned short graphic;
    unsigned char race;
    unsigned short boss;
    unsigned short child;
    unsigned short npc_type;
    unsigned short behavior_id;
    int hp;
    unsigned short tp;
    unsigned short min_damage;
    unsigned short max_damage;
    unsigned short accuracy;
    unsigned short evade;
    unsigned short armor;
    unsigned short return_damage;
    unsigned short element;
    unsigned short element_damage;
    unsigned short element_weakness;
    unsigned short element_weakness_damage;
    unsigned char level;
    int experience;
    unsigned char talk_interval;
    unsigned char talk_rate;
    bool unused_flag;
    unsigned short drop_rate_total;
    vector<NpcDropItem> drops;
    vector<AnsiString> talk_lines;

    NpcValue();
    NpcValue(int id);
    ~NpcValue();
    void AddDrop(int item_id, int min_amount, int max_amount, int rate);
    void AddMessage(char interval, char rate, String message);
};

#endif
