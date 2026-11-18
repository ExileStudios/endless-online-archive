#include <vcl.h>
#pragma hdrstop

#include "Datagridview.h"

#pragma package(smart_init)

DataGridView::DataGridView(String view_name)
{
    name = view_name;
    group_count = 0;
}

DataGridView::~DataGridView()
{
}

// Adds a group whose columns the view shows.
void DataGridView::AddGroup(int group)
{
    groups[group_count] = group;
    group_count++;
}
