#include <vcl.h>
#pragma hdrstop

#include "Itemvalue.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

ItemValue::ItemValue(int record_id)
{
    item_id = record_id;
}

ItemValue::~ItemValue()
{
}
