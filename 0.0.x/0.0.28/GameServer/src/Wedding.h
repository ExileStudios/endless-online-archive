#ifndef WeddingH
#define WeddingH

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



struct Wedding;

// A wedding at a priest: the two players, whether each has said yes, and the step it has reached.
struct Wedding
{
    int map_id;
    int priest_npc_index;
    int player1_id;
    String player1_name;
    bool player1_confirmed;
    int player2_id;
    String player2_name;
    bool player2_confirmed;
    int step;
    int countdown;

    Wedding(int map_id, int priest_npc_index);
    ~Wedding();
};


#endif
