#ifndef SkillvalueH
#define SkillvalueH

// One skill of the skill file.
struct SkillValue
{
    int skill_id;
    String name;
    String shout;
    short max_level;
    short icon;
    short graphic;
    unsigned short tp_cost;
    unsigned short sp_cost;
    short cast_time;
    int category;
    int spare_byte;
    int type;
    short element;
    unsigned short element_power;
    short target_restriction;
    short target_type;
    short target_time;
    short area;
    int min_damage;
    int max_damage;
    int accuracy;
    int evade;
    int armor;
    int return_damage;
    unsigned short hp_heal;
    short tp_heal;
    short sp_heal;
    short bonus_str;
    short bonus_int;
    short bonus_wis;
    short bonus_agi;
    short bonus_con;
    short bonus_cha;

    SkillValue(int record_id);
    ~SkillValue();
};

#endif
