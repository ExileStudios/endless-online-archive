#include <vcl.h>
#pragma hdrstop

#include "Uitabinns.h"
#include "UnitMain.h"
#include "Datagrid.h"
#include "Datagridview.h"
#include "Inndataadapter.h"
#include "Innvalues.h"
#include <vector.h>

#pragma package(smart_init)

UiTabInns::UiTabInns(String pub_path, int gfx_library, int icon_library)
{
    main_grid = FormMain->ElTreeStringGrid13;
    sub_grid = FormMain->ElTreeStringGrid14;
    grid_ui = new DataGridUI(String("inns"), String("Inn"), main_grid, sub_grid, String(""));
    inn_values = new InnValues(pub_path);
    adapter = new InnDataAdapter();
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Shop Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        InnGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tag", "Shop id-Tag", 70, ColumnKind_Number, "* linked id by eoserv", 0, 64008,
        InnGroup_Basic, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Name", "Shop Name", 200, ColumnKind_Text, "", 0, 24, InnGroup_Basic,
        ColumnAlign_Left, ColumnFormat_Capitalized);
    grid_ui->data_grid->AddColumn("Cost", "Sleep Cost", 100, ColumnKind_Number, "", 0, 0xF71AE3, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Sleep Map", "Sleep Spawn Map", 134, ColumnKind_Number, "", 0, 64008, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Sleep X", "Sleep Spawn X", 80, ColumnKind_Number, "", 0, 252, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Sleep Y", "Sleep Spawn Y", 80, ColumnKind_Number, "", 0, 252, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Home Map", "Home Spawn Map", 134, ColumnKind_Number, "", 0, 64008, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Home X", "Home Spawn X", 80, ColumnKind_Number, "", 0, 252, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Home Y", "Home Spawn Y", 80, ColumnKind_Number, "", 0, 252, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hi Map", "Hi-Level Spawn Map", 134, ColumnKind_Number, "", 0, 64008,
        InnGroup_HighLevelSpawn, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hi X", "Hi-Level Spawn X", 80, ColumnKind_Number, "", 0, 252,
        InnGroup_HighLevelSpawn, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hi Y", "Hi-Level Spawn Y", 80, ColumnKind_Number, "", 0, 252,
        InnGroup_HighLevelSpawn, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hi-Level", "Hi-Level Requirement", 100, ColumnKind_Number, "", 0, 252,
        InnGroup_HighLevelSpawn, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Quiz", "Quiz Questions", 64, ColumnKind_InnQuiz, "", 1, 64008, InnGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    DataGridView *view = grid_ui->data_grid->AddView("basic view");
    view->AddGroup(InnGroup_Basic);
    view = grid_ui->data_grid->AddView("all view");
    view->AddGroup(InnGroup_Basic);
    view->AddGroup(InnGroup_HighLevelSpawn);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeInnView, 45, 235);
    TElPopupMenu *filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeInnFilterType, FormMain->imageChangeInnFilterType, 145,
        235, "", "All Inns");
    grid_ui->SetEditPanel(FormMain->PanelSaveInnsHint);
    grid_ui->SetSearchEdit(FormMain->editSearchInn);
    grid_ui->SetInfoLabels(FormMain->labelInnInspectorFilter, FormMain->labelInnInspectorID,
        FormMain->labelInnInspectorName, FormMain->imageInnInspector, "");
    grid_ui->SetMessagePanel(FormMain->panelInfoHostInns, FormMain->panelInsideInfoInns);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the inns from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabInns::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    inn_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (inn_values->header_read)
    {
        String file_path = inn_values->file_path;
        if (FormMain->config_check->PathExists(file_path))
            file_unreadable = true;
    }
    if (!main_grid->IsUpdating)
        main_grid->IsUpdating = true;
    main_grid->Items->Clear();
    FillGrid(main_grid);
    grid_ui->data_grid->edit_row = 0;
    grid_ui->data_grid->edit_detail_item = 0;
    main_grid->IsUpdating = false;
    grid_ui->is_ready = true;
}

// Fills the grid with a row per inn; notes a record named EOF and hides blank names unless shown.
void UiTabInns::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *tag_cells;
    TElWideStrings *name_cells;
    TElWideStrings *cost_cells;
    TElWideStrings *sleep_map_cells;
    TElWideStrings *sleep_x_cells;
    TElWideStrings *sleep_y_cells;
    TElWideStrings *home_map_cells;
    TElWideStrings *home_x_cells;
    TElWideStrings *home_y_cells;
    TElWideStrings *hi_map_cells;
    TElWideStrings *hi_x_cells;
    TElWideStrings *hi_y_cells;
    TElWideStrings *hi_level_cells;
    has_eof_record = false;
    int name_index = 0;
    for (int section = 1; section < grid->HeaderSections->Count; section++)
    {
        if (String(grid->HeaderSections->Item[section]->Text) == "Name")
        {
            name_index = section - 1;
            break;
        }
    }
    for (int i = 0; i < (unsigned int)inn_values->Count(); i++)
    {
        InnValue *inn = inn_values->GetInn(i);
        if (LowerCase(inn->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(inn->id));
        tag_cells = item->ColumnText;
        tag_cells->Add(WideString(NumberText(inn->behavior_id)));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(inn->name));
        cost_cells = item->ColumnText;
        cost_cells->Add(WideString(NumberText(inn->sleep_cost)));
        sleep_map_cells = item->ColumnText;
        sleep_map_cells->Add(WideString(NumberText(inn->sleep_map)));
        sleep_x_cells = item->ColumnText;
        sleep_x_cells->Add(WideString(NumberText(inn->sleep_x)));
        sleep_y_cells = item->ColumnText;
        sleep_y_cells->Add(WideString(NumberText(inn->sleep_y)));
        home_map_cells = item->ColumnText;
        home_map_cells->Add(WideString(NumberText(inn->spawn_map)));
        home_x_cells = item->ColumnText;
        home_x_cells->Add(WideString(NumberText(inn->spawn_x)));
        home_y_cells = item->ColumnText;
        home_y_cells->Add(WideString(NumberText(inn->spawn_y)));
        hi_map_cells = item->ColumnText;
        hi_map_cells->Add(WideString(NumberText(inn->alternate_spawn_map)));
        hi_x_cells = item->ColumnText;
        hi_x_cells->Add(WideString(NumberText(inn->alternate_spawn_x)));
        hi_y_cells = item->ColumnText;
        hi_y_cells->Add(WideString(NumberText(inn->alternate_spawn_y)));
        hi_level_cells = item->ColumnText;
        hi_level_cells->Add(WideString(NumberText(inn->alternate_spawn_level)));
        item->ColumnText->Add(WideString(NumberText(inn->questions.size())));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled() && inn->name.Length() < 2)
            item->Hidden = true;
    }
}

UiTabInns::~UiTabInns()
{
}

DEFINE_NUMBER_TEXT(UiTabInns)
