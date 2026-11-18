#include <vcl.h>
#pragma hdrstop

#include "Datalinkitem.h"

#pragma package(smart_init)

DataLinkItem::DataLinkItem(TElTreeStringGrid *link_grid, String link_column)
{
    column_name = link_column;
    grid = link_grid;
    column_index = -1;
}

DataLinkItem::~DataLinkItem()
{
}

// The text index of the linked column in its grid, found once and kept; -1 without a grid.
short DataLinkItem::FindColumnIndex()
{
    if (column_index != -1)
        return column_index;
    if (!grid)
        return -1;
    for (int section = 0; section < grid->HeaderSections->Count; section++)
    {
        if (String(grid->HeaderSections->Item[section]->Text) == column_name)
        {
            column_index = section - 1;
            return column_index;
        }
    }
}
