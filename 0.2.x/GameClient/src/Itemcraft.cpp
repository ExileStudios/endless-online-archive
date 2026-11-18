#include <vcl.h>
#pragma hdrstop

#include "Itemcraft.h"

#pragma package(smart_init)

CraftItem::CraftItem(int id)
{
    item_id = id;
    for (int i = 0; i < MAX_CRAFT_INGREDIENTS; i++)
    {
        ingredient_ids[i] = 0;
        ingredient_amounts[i] = 0;
    }
}

CraftItem::~CraftItem()
{
}
