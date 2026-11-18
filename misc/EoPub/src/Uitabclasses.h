#ifndef UitabclassesH
#define UitabclassesH

enum ClassGroup
{
    ClassGroup_Basic = 0
};

#include <ElTreeGrids.hpp>

struct ClassValues;
struct DataGridUI;

// The classes tab: the class file and the grid view that shows and edits it.
struct UiTabClasses
{
    ClassValues *class_values;
    DataGridUI *grid_ui;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabClasses(String pub_path, int gfx_library, int icon_library);
    ~UiTabClasses();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
