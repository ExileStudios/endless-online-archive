#ifndef PlayerskillH
#define PlayerskillH

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



struct PlayerSkill;

// A spell a player knows, and its level.
struct PlayerSkill
{
    int skill_id;
    unsigned int level;

    PlayerSkill(int id);
    ~PlayerSkill();
};


#endif
