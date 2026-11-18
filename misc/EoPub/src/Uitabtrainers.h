#ifndef UitabtrainersH
#define UitabtrainersH

enum TrainerGroup
{
    TrainerGroup_Basic = 0
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct LearnDataAdapter;
struct LearnValues;

// The skill masters tab: the masters, their grid view and the adapter that keeps them in step.
struct UiTabTrainers
{
    LearnValues *trainer_values;
    DataGridUI *grid_ui;
    LearnDataAdapter *adapter;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabTrainers(String pub_path, int gfx_library, int icon_library);
    ~UiTabTrainers();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
