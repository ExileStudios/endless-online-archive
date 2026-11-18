#ifndef LearnitemH
#define LearnitemH

// A skill a master teaches, with its price and what it needs: level, class, skills and stats.
struct LearnItemVal
{
    int id;
    short level_requirement;
    short class_requirement;
    int price;
    short skill_requirement_1;
    short skill_requirement_2;
    short skill_requirement_3;
    short skill_requirement_4;
    short str_requirement;
    short wis_requirement;
    short int_requirement;
    short agi_requirement;
    short con_requirement;
    short cha_requirement;

    LearnItemVal();
    LearnItemVal(int skill_id);
    ~LearnItemVal();
};

#endif
