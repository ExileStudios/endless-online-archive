#ifndef UitabskillsH
#define UitabskillsH

enum SkillGroup
{
    SkillGroup_Basic = 0,
    SkillGroup_Target = 1,
    SkillGroup_Requirements = 2,
    SkillGroup_Heal = 3,
    SkillGroup_Battle = 4,
    SkillGroup_StatBonus = 5,
    SkillGroup_Element = 6,
    SkillGroup_Reserve = 7
};

#include <ElTreeGrids.hpp>

struct DataGridUI;
struct SkillValues;

// The skills tab: the skill file and the grid view that shows and edits it.
struct UiTabSkills
{
    SkillValues *skill_values;
    DataGridUI *grid_ui;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    bool has_eof_record;
    bool file_unreadable;

    UiTabSkills(String pub_path, HINSTANCE__ *gfx_library, HINSTANCE__ *icon_library);
    ~UiTabSkills();
    void Reload();
    void FillGrid(TElTreeStringGrid *grid);
    String NumberText(int value);
};

#endif
