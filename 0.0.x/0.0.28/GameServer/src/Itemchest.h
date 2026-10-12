#ifndef ItemchestH
#define ItemchestH

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

struct ChestItem;

// One slot of a map chest.
struct ChestItem
{
    int item_id;
    int amount;
    bool item_present;
    bool respawn_enabled;
    int respawn_countdown;
    unsigned short respawn_delay;
    unsigned short alt_item_id0;
    unsigned short alt_item_id1;
    unsigned short alt_item_id2;
    unsigned short alt_item_id3;
    int alt_amount0;
    int alt_amount1;
    int alt_amount2;
    int alt_amount3;

    ChestItem(int item_id);
    ~ChestItem();
};

#endif
