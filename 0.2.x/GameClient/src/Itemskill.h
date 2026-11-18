#ifndef ItemskillH
#define ItemskillH

struct SkillItem
{
    int grid_x;
    int grid_y;
    int skill_id;
    unsigned int level;
    unsigned char is_visible;

    ~SkillItem();
};

#endif
