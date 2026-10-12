#ifndef ShopcraftH
#define ShopcraftH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>
#define CRAFT_INGREDIENT_COUNT 4



struct ShopCraftVal;

// A craft of a shop: the item and its four ingredients.
struct ShopCraftVal
{
    int id;
    int ingredient_item_ids[CRAFT_INGREDIENT_COUNT];
    int ingredient_amounts[CRAFT_INGREDIENT_COUNT];

    ShopCraftVal(int id);
    ~ShopCraftVal();
};


#endif
