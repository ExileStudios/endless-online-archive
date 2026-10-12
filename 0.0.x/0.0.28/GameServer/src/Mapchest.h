#ifndef MapchestH
#define MapchestH

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

#include "Itemchest.h"

struct MapChest;

// A chest on a map: its spot, the key it needs and its slots.
struct MapChest
{
    unsigned short x;
    unsigned short y;
    unsigned short key_id;
    bool updated;
    vector<ChestItem> slots;

    MapChest();
    MapChest(int x, int y, int key_id);
    ~MapChest();
};


#endif
