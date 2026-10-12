#include <vcl.h>
#pragma hdrstop

#include "Itemspell.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

SpellItem::SpellItem(int id)
{
    spell_id = id;
    is_visible = 1;
    is_placed = 0;
}

SpellItem::~SpellItem()
{
}
