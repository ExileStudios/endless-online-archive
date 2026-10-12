#include <vcl.h>
#pragma hdrstop

#include "Sys_item.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MapItem::MapItem()
{
    label_shown = 0;
    protection_ticks = 0;
}

MapItem::MapItem(int item_uid, int record_id, int tile_x, int tile_y, int item_amount)
{
    label_shown = 0;
    protection_ticks = 0;
    uid = item_uid;
    item_id = record_id;
    x = tile_x;
    y = tile_y;
    amount = item_amount;
}

MapItem::~MapItem()
{
}
