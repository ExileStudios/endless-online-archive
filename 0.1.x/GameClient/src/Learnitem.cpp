#include <vcl.h>
#pragma hdrstop

#include "Learnitem.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

LearnItem::LearnItem(int id)
{
    skill_id = id;
}

LearnItem::~LearnItem()
{
}
