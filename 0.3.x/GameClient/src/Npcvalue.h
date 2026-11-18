#ifndef NpcvalueH
#define NpcvalueH

#include <vector.h>

#include "Npcdrop.h"

// One NPC of the NPC file.
struct NpcValue
{
    int npc_id;
    String name;
    unsigned short graphic;
    char race;
    unsigned short boss;
    short child;
    unsigned short npc_type;
    unsigned short behavior_id;
    int hp;
    short tp;
    short min_damage;
    short max_damage;
    short accuracy;
    short evade;
    short armor;
    short return_damage;
    unsigned short element;
    short element_damage;
    unsigned short element_weakness;
    short element_weakness_damage;
    unsigned char level;
    int experience;
    bool talk_interval;
    bool talk_rate;
    bool unused_flag;
    short drop_rate_total;
    vector<NpcDropItem> drops;
    vector<AnsiString> talk_lines;

    NpcValue(int record_id);
    ~NpcValue();
};

#endif
