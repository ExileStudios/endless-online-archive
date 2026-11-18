#include <vcl.h>
#pragma hdrstop

#include "Npcvalue.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

NpcValue::NpcValue(int record_id)
{
    npc_id = record_id;
}

NpcValue::~NpcValue()
{
}
