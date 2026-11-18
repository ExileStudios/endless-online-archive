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
    short tp_cost;
    short sp_cost;
    short cast_time;
    short category;
    short spare_byte;
    int type;
    short element;
    short element_power;
    short target_restriction;
    short target_type;
    short target_time;
    short area;
    short min_damage;
    short max_damage;
    short accuracy;
    short evade;
    short armor;
    short return_damage;
    short hp_heal;
    short tp_heal;
    short sp_heal;
    short bonus_str;
    short bonus_int;
    short bonus_wis;
    short bonus_agi;
    short bonus_con;
    short bonus_cha;

    SkillValue();
    SkillValue(int record_id);
    ~SkillValue();
};

#endif
