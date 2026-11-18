#include <vcl.h>
#pragma hdrstop

#include "Iteminventory.h"

#pragma package(smart_init)

InventoryItem::InventoryItem(int id)
{
    item_id = id;
    is_visible = true;
    is_placed = false;
    highlight_ticks = 0;
}

InventoryItem::~InventoryItem()
{
}
