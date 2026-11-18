#ifndef ItemcraftH
#define ItemcraftH

#define MAX_CRAFT_INGREDIENTS 4

struct CraftIngredient
{
    String name;
    int item_id;
    int amount;
    int owned;
    char is_hovered;
    int x;
    int y;
    char is_result;

    CraftIngredient(int id);
    ~CraftIngredient();
};

// One item a shop crafts, with up to four ingredients and their amounts.
struct CraftItem
{
    int item_id;
    int ingredient_ids[MAX_CRAFT_INGREDIENTS];
    int ingredient_amounts[MAX_CRAFT_INGREDIENTS];

    CraftItem(int id);
    ~CraftItem();
};

#endif
