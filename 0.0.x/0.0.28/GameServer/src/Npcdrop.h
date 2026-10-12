#ifndef NpcdropH
#define NpcdropH

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



struct NpcDropItem;

// One drop of an npc: the item, the amounts and the rate.
struct NpcDropItem
{
    int item_id;
    int min_amount;
    int max_amount;
    int rate;

    NpcDropItem(int item_id);
    ~NpcDropItem();
};


#endif
