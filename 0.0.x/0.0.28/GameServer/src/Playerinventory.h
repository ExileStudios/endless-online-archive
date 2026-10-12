#ifndef PlayerinventoryH
#define PlayerinventoryH

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



struct PlayerInventory;

// An item and its amount, in the inventory, the bank or a trade.
struct PlayerInventory
{
    int item_id;
    unsigned int amount;

    PlayerInventory(int id);
    ~PlayerInventory();
};


#endif
