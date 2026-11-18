#include <vcl.h>
#pragma hdrstop

#include "Npcvalue.h"

#pragma package(smart_init)

NpcValue::NpcValue(int record_id)
{
    npc_id = record_id;
    talk_interval = false;
    talk_rate = false;
    unused_flag = false;
    drop_rate_total = 0;
    drops.clear();
    talk_lines.clear();
}

NpcValue::~NpcValue()
{
}
