#include <vcl.h>
#pragma hdrstop

#include "Npcdataadapter.h"
#include "UnitMain.h"
#include "Npcvalue.h"
#include "Npcvalues.h"
#include <vector.h>

#pragma package(smart_init)

NpcDataAdapter::NpcDataAdapter()
{
}

NpcDataAdapter::~NpcDataAdapter()
{
}

// Counts the drops and chat lines and the NPCs having them; returns both NPC counts added.
int NpcDataAdapter::CountLists()
{
    NpcValue *npc;
    chat_npc_count = 0;
    message_count = 0;
    drop_npc_count = 0;
    drop_count = 0;
    for (int i = 0; i < (unsigned int)FormMain->npcs_tab->npc_values->Count(); i++)
    {
        npc = FormMain->npcs_tab->npc_values->GetNpc(i);
        message_count = npc->talk_lines.size() + message_count;
        if (npc->talk_lines.size() > 0)
            chat_npc_count++;
        drop_count = npc->drops.size() + drop_count;
        if (npc->drops.size() > 0)
            drop_npc_count++;
    }
    return chat_npc_count + drop_npc_count;
}

// How many drops of all the NPCs are of the item.
int NpcDataAdapter::ItemDropCount(int item_id)
{
    NpcValue *npc;
    int count = 0;
    for (int i = 0; i < (unsigned int)FormMain->npcs_tab->npc_values->Count(); i++)
    {
        npc = FormMain->npcs_tab->npc_values->GetNpc(i);
        for (int j = 0; j < npc->drops.size(); j++)
        {
            if (npc->drops[j].item_id == item_id)
                count++;
        }
    }
    return count;
}

// Deletes the NPC at the index and moves the IDs of the NPCs after it down by one.
void NpcDataAdapter::DeleteAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->npcs_tab->npc_values->Count() <= index)
        return;
    FormMain->npcs_tab->npc_values->DeleteNpc(index);
    while ((unsigned int)FormMain->npcs_tab->npc_values->Count() > index)
    {
        FormMain->npcs_tab->npc_values->GetNpc(index)->npc_id--;
        index++;
    }
}

// Inserts an NPC at the index and moves the IDs of the NPCs after it up by one.
void NpcDataAdapter::InsertAt(int index)
{
    if (index < 0 || (unsigned int)FormMain->npcs_tab->npc_values->Count() <= index)
        return;
    FormMain->npcs_tab->npc_values->InsertNpc(index);
    while ((unsigned int)FormMain->npcs_tab->npc_values->Count() > index)
    {
        FormMain->npcs_tab->npc_values->GetNpc(index)->npc_id++;
        index++;
    }
}

// Copies the drops and chat lines of the NPC with the first ID onto the NPC with the second.
void NpcDataAdapter::CopyLists(String source_id, String target_id)
{
    NpcValue *target;
    if (source_id.Length() < 1)
        return;
    if (target_id.Length() < 1)
        return;
    NpcValue *source = FormMain->npcs_tab->npc_values->GetNpc(StrToInt(source_id) - 1);
    target = FormMain->npcs_tab->npc_values->GetNpc(StrToInt(target_id) - 1);
    target->drops.clear();
    target->talk_lines.clear();
    for (int i = 0; i < source->drops.size(); i++)
    {
        NpcDropItem drop = source->drops[i];
        NpcDropItem drop_copy(drop.item_id);
        drop_copy = drop;
        target->drops.insert(target->drops.end(), drop_copy);
    }
    for (int j = 0; j < source->talk_lines.size(); j++)
        target->talk_lines.insert(target->talk_lines.end(), source->talk_lines[j]);
}
