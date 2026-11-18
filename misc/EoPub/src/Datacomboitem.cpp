#include <vcl.h>
#pragma hdrstop

#include "Datacomboitem.h"

#pragma package(smart_init)

DataComboItem::DataComboItem(int item_index)
{
    index = item_index;
    group_count = 0;
}

DataComboItem::~DataComboItem()
{
}

bool DataComboItem::HasGroup(int group)
{
    for (int i = 0; i < group_count; i++)
    {
        if (group == groups[i])
            return true;
    }
    return false;
}
