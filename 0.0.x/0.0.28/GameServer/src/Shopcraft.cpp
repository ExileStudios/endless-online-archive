#include <vcl.h>
#pragma hdrstop

#include "Shopcraft.h"

#pragma package(smart_init)

ShopCraftVal::ShopCraftVal(int id)
{
    this->id = id;
    for (int i = 0; i < CRAFT_INGREDIENT_COUNT; i++)
    {
        this->ingredient_item_ids[i] = 0; this->ingredient_amounts[i] = 0;
    }
}

ShopCraftVal::~ShopCraftVal()
{
}
