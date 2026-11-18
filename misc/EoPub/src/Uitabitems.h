#ifndef UitabitemsH
#define UitabitemsH

enum ItemGroup
{
    ItemGroup_Basic = 0,
    ItemGroup_TargetArea = 1,
    ItemGroup_Data1 = 2,
    ItemGroup_Data2And3 = 3,
    ItemGroup_Requirements = 4,
    ItemGroup_HpTp = 5,
    ItemGroup_Battle = 6,
    ItemGroup_StatBonus = 7,
    ItemGroup_Element = 8,
    ItemGroup_Resistance = 9
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct ItemValues;

// The items tab: the item file and the grid view that shows and edits it.
struct UiTabItems
{
    ItemValues *item_values;
    DataGridUI *grid_ui;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabItems(String pub_path, HINSTANCE__ *gfx_library, HINSTANCE__ *icon_library);
    ~UiTabItems();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
