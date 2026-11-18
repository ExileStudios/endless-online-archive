#ifndef NpcvalueH
#define NpcvalueH

// One NPC of the NPC file.
struct NpcValue
{
    int npc_id;
    String name;
    short graphic;
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

    NpcValue(int record_id);
    ~NpcValue();
};

#endif
