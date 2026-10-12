#include <vcl.h>
#pragma hdrstop

#include "Wedding.h"

#pragma package(smart_init)

Wedding::Wedding(int map_id, int priest_npc_index)
{
    this->map_id = map_id;
    this->priest_npc_index = priest_npc_index;
    this->player1_confirmed = false;
    this->player2_confirmed = false;
}

Wedding::~Wedding()
{
}
