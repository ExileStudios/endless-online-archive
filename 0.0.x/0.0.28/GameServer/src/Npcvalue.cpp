#include <vcl.h>
#pragma hdrstop

#include "Npcvalue.h"
#include "Npcdrop.h"

#pragma package(smart_init)

NpcValue::NpcValue()
{
    talk_rate = 0;
    has_talk = false;
    drops.clear();
    talk_lines.clear();
}

NpcValue::NpcValue(int id)
{
    this->id = id;
    this->talk_rate = 0;
    this->has_talk = false;
    this->drops.clear();
    this->talk_lines.clear();
}

NpcValue::~NpcValue()
{
}

void NpcValue::AddDrop(int item_id, int min_amount, int max_amount, int rate)
{
    NpcDropItem drop(item_id);
    drop.min_amount = min_amount;
    drop.max_amount = max_amount;
    drop.rate = rate;
    drops.insert(drops.end(), drop);
}

// Adds a talk line; the last rate given is the npc's talk rate.
void NpcValue::AddTalkMessage(int rate, String message)
{
    has_talk = true;
    talk_rate = rate;
    talk_lines.insert(talk_lines.end(), message);
}
