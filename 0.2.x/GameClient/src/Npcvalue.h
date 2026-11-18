#ifndef NpcvalueH
#define NpcvalueH

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

    NpcValue(int record_id);
    ~NpcValue();
};

#endif
