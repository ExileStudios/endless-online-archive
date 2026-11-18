#include <vcl.h>
#pragma hdrstop

#include "Npcvalue.h"

#pragma package(smart_init)

void Npcvalue_UnusedInstances1(std::vector<NpcDropItem> &drops, std::vector<String> &talk_lines)
{
    drops.clear();
    talk_lines.clear();
}

// An NPC with no drops and no chat lines.
NpcValue::NpcValue(int id)
{
    npc_id = id;
    talk_interval = 0;
    talk_rate = 0;
    drops.clear();
    talk_lines.clear();
}

NpcValue::~NpcValue()
{
}

// Adds a drop: the item, its least and most amount and its rate.
void NpcValue::AddDrop(int item_id, int min_amount, int max_amount, int rate)
{
    NpcDropItem drop(item_id);
    drop.min_amount = min_amount;
    drop.max_amount = max_amount;
    drop.rate = rate;
    drops.insert(drops.end(), drop);
}

void Npcvalue_UnusedInstances2(std::vector<String> &items, const String &item)
{
    items.insert(items.end(), item);
}
