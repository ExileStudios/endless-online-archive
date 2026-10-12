#ifndef ItemgroundH
#define ItemgroundH

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



struct ItemObj;

// An item lying on a map.
struct ItemObj
{
    int index;
    int item_id;
    int x;
    int y;
    int amount;
    TTimeStamp drop_time;
    unsigned int owner_account_ident;
    short protect_ticks;

    ItemObj();
    ~ItemObj();
};


#endif
