#include <vcl.h>
#pragma hdrstop

#include "Iteminventory.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

InventoryItem::InventoryItem(int id)
{
    item_id = id;
    is_visible = true;
    is_placed = false;
}

InventoryItem::~InventoryItem()
{
}
