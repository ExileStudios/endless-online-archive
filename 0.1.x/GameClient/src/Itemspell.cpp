#include <vcl.h>
#pragma hdrstop

#include "Itemspell.h"

#pragma package(smart_init)

SpellItem::SpellItem(int id)
{
    spell_id = id;
    is_visible = true;
    is_placed = false;
}

SpellItem::~SpellItem()
{
}
