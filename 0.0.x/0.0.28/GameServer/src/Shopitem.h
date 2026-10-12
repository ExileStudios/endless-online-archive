#ifndef ShopitemH
#define ShopitemH

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



struct ShopItemVal;

// A trade of a shop: the item, its prices and the most at once.
struct ShopItemVal
{
    int item_id;
    int buy_price;
    int sell_price;
    int max_amount;

    ShopItemVal(int item_id);
    ~ShopItemVal();
};


#endif
