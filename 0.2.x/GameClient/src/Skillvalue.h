#ifndef SkillvalueH
#define SkillvalueH

// One skill of the skill file.
struct SkillValue
{
    int skill_id;
    String name;
    String shout;
    short max_level;
    unsigned short icon;
    unsigned short graphic;
    unsigned short tp_cost;
    unsigned short sp_cost;
    unsigned char cast_time;
    unsigned char category;
    char spare_byte;
    int type;
    unsigned char element;
    unsigned short element_power;
    unsigned char target_restriction;
    unsigned char target_type;
    char target_time;
    unsigned char area;
    unsigned short min_damage;
    unsigned short max_damage;
    unsigned short accuracy;
    unsigned short evade;
    unsigned short armor;
    unsigned char return_damage;
    unsigned short hp_heal;
    short tp_heal;
    char sp_heal;
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
