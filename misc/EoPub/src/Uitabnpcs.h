#ifndef UitabnpcsH
#define UitabnpcsH

enum NpcGroup
{
    NpcGroup_Basic = 0,
    NpcGroup_Battle = 1,
    NpcGroup_RaceBoss = 2,
    NpcGroup_Element = 3
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct NpcDataAdapter;
struct NpcValues;

// The NPCs tab: the NPC file, its grid view and the adapter that keeps its lists in step.
struct UiTabNpcs
{
    NpcValues *npc_values;
    DataGridUI *grid_ui;
    NpcDataAdapter *adapter;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabNpcs(String pub_path, HINSTANCE__ *gfx_library, int icon_library);
    ~UiTabNpcs();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
