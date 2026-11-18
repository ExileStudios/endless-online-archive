#ifndef UitabinnsH
#define UitabinnsH

enum InnGroup
{
    InnGroup_Basic = 0,
    InnGroup_HighLevelSpawn = 1
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct InnDataAdapter;
struct InnValues;

// The inns tab: the inns, their grid view and the adapter that keeps them in step.
struct UiTabInns
{
    InnValues *inn_values;
    DataGridUI *grid_ui;
    InnDataAdapter *adapter;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabInns(String pub_path, int gfx_library, int icon_library);
    ~UiTabInns();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
