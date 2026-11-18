#include <vcl.h>
#pragma hdrstop

#include "Npcvalue.h"

#pragma package(smart_init)

NpcValue::NpcValue()
{
}

void Npcvalue_UnusedLocals1()
{
    NpcValue object;
}

NpcValue::NpcValue(int id)
{
    npc_id = id;
    talk_interval = 0;
    talk_rate = 0;
    unused_flag = false;
    drop_rate_total = 0;
    drops.clear();
    talk_lines.clear();
}

NpcValue::~NpcValue()
{
}

// Adds a drop: the item, its least and most amount and its rate, which counts toward the total.
void NpcValue::AddDrop(int item_id, int min_amount, int max_amount, int rate)
{
    NpcDropItem drop(item_id);
    drop.min_amount = min_amount;
    drop.max_amount = max_amount;
    drop.rate = rate;
    drop_rate_total += (short)rate;
    drops.insert(drops.end(), drop);
}

// Adds a chat line and sets how often the NPC talks.
void NpcValue::AddMessage(char interval, char rate, String message)
{
    talk_interval = interval;
    talk_rate = rate;
    talk_lines.insert(talk_lines.end(), message);
}
