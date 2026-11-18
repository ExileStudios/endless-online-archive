#ifndef ShopcraftH
#define ShopcraftH

// An item a shop crafts from up to four ingredients.
struct ShopCraftVal
{
    int id;
    int ingredient_item_ids[4];
    int ingredient_amounts[4];

    ShopCraftVal();
    ShopCraftVal(int item_id);
    ~ShopCraftVal();
};

#endif
