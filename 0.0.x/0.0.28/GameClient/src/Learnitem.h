#ifndef LearnitemH
#define LearnitemH

// One skill the skill master teaches, with its price and what it needs: level, class, skills and stats.
struct LearnItem
{
    int skill_id;
    short required_level;
    short required_class;
    int price;
    short required_skill;
    short second_required_skill;
    short third_required_skill;
    short fourth_required_skill;
    short required_str;
    short required_wis;
    short required_int;
    short required_agi;
    short required_con;
    short required_cha;

    LearnItem(int id);
    ~LearnItem();
};

#endif
