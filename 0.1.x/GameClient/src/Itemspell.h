#ifndef ItemspellH
#define ItemspellH

// One spell on the spell grid: its place, id and level.
struct SpellItem
{
    int grid_x;
    int grid_y;
    int spell_id;
    unsigned int level;
    char is_visible;
    char is_placed;

    SpellItem(int id);
    ~SpellItem();
};

#endif
