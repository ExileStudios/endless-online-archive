#ifndef UitabshopsH
#define UitabshopsH

enum ShopGroup
{
    ShopGroup_Basic = 0
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct ShopDataAdapter;
struct ShopValues;

// The shops tab: the shops, their grid view and the adapter that keeps them in step.
struct UiTabShops
{
    ShopValues *shop_values;
    DataGridUI *grid_ui;
    ShopDataAdapter *adapter;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabShops(String pub_path, int gfx_library, int icon_library);
    ~UiTabShops();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
