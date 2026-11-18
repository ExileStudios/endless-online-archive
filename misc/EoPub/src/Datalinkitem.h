#ifndef DatalinkitemH
#define DatalinkitemH

#include <ElTreeGrids.hpp>

// A link from a column to a column of another grid, found by its header.
struct DataLinkItem
{
    TElTreeStringGrid *grid;
    String column_name;
    short column_index;

    DataLinkItem(TElTreeStringGrid *link_grid, String link_column);
    ~DataLinkItem();
    short FindColumnIndex();
};

#endif
