#ifndef KillcounterH
#define KillcounterH

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



struct KillCounter;

// A player's kills today.
struct KillCounter
{
    String name;
    int count;

    KillCounter(String name);
    KillCounter(String name, int count);
    ~KillCounter();
};


#endif
