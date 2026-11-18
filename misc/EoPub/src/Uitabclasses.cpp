#include <vcl.h>
#pragma hdrstop

#include "Uitabclasses.h"
#include "UnitMain.h"
#include "Classvalue.h"
#include "Classvalues.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"

#pragma package(smart_init)

UiTabClasses::UiTabClasses(String pub_path, int gfx_library, int icon_library)
{
    DataGridView *all_view;
    TElPopupMenu *filter_menu;
    main_grid = FormMain->ElTreeStringGrid3;
    sub_grid = FormMain->ElTreeStringGrid4;
    grid_ui = new DataGridUI(String("classes"), String("Class"), main_grid, sub_grid, String(""));
    class_values = new ClassValues(pub_path, false);
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Item Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        ClassGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *name_column =
        grid_ui->data_grid->AddColumn("Name", "Item Name", 160, ColumnKind_LinkSource, "", 0, 24, ClassGroup_Basic,
            ColumnAlign_Left, ColumnFormat_Capitalized);
    DataGridCol *stat_group_column =
        grid_ui->data_grid->AddColumn("Stat Group", "Class Stat Group", 150, ColumnKind_Combo, "Link: external.ini", 0,
            252, ClassGroup_Basic, ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *parent_column =
        grid_ui->data_grid->AddColumn("Parent", "Parent Class", 160, ColumnKind_Link, "Link: class.name", 0, 252,
            ClassGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Str", "Base Strength", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Int", "Base Intelligence", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Wis", "Base Wisdom", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Agi", "Base Agility", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Con", "Base Constitution", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *charisma_column =
        grid_ui->data_grid->AddColumn("Cha", "Base Charisma", 72, ColumnKind_Number, "", 0, 64008, ClassGroup_Basic,
            ColumnAlign_Right, ColumnFormat_None);
    for (int i = 0; i <= 9; i++)
        stat_group_column->AddComboItemWithGroup(i, "group-" + IntToStr(i), ClassGroup_Basic);
    stat_group_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    parent_column->AddLink(FormMain->ElTreeStringGrid3, "Name");
    name_column->AddLink(FormMain->ElTreeStringGrid3, "Parent");
    name_column->AddLink(FormMain->ElTreeStringGrid1, "Req:Cls");
    name_column->AddLink(FormMain->ElTreeStringGrid9, "Req:Cls");
    name_column->AddLink(FormMain->ElTreeStringGrid11, "Req:Cls");
    DataGridView *basic_view = grid_ui->data_grid->AddView("basic view");
    basic_view->AddGroup(ClassGroup_Basic);
    all_view = grid_ui->data_grid->AddView("all view");
    all_view->AddGroup(ClassGroup_Basic);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeClassView, 45, 235);
    filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeClassFilterType, FormMain->imageChangeClassFilterType, 145,
        235, "Stat Group", "All Classes");
    grid_ui->SetEditPanel(FormMain->PanelSaveClassesHint);
    grid_ui->SetSearchEdit(FormMain->editSearchClass);
    grid_ui->SetInfoLabels(FormMain->labelClassInspectorFilter, FormMain->labelClassInspectorID,
        FormMain->labelClassInspectorName, FormMain->imageClassInspector, "Stat Group");
    grid_ui->SetMessagePanel(FormMain->panelInfoHostClasses, FormMain->panelInsideInfoClasses);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the classes from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabClasses::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    class_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (class_values->file_missing)
    {
        String part_path = class_values->part_file_path;
        if (FormMain->config_check->PathExists(part_path))
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

// Fills the grid with a row per class; notes a record named EOF and hides blank names unless shown.
void UiTabClasses::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *name_cells;
    TElWideStrings *stat_group_cells;
    TElWideStrings *parent_cells;
    TElWideStrings *str_cells;
    TElWideStrings *int_cells;
    TElWideStrings *wis_cells;
    TElWideStrings *agi_cells;
    TElWideStrings *con_cells;
    TElWideStrings *cha_cells;
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
    for (int i = 0; i < (unsigned int)class_values->Count(); i++)
    {
        ClassValue *record = class_values->GetRecord(i);
        if (LowerCase(record->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(record->class_id));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(record->name));
        stat_group_cells = item->ColumnText;
        stat_group_cells->Add(WideString(grid_ui->data_grid->CellText("Stat Group", record->parent_class_id)));
        parent_cells = item->ColumnText;
        parent_cells->Add(WideString(grid_ui->data_grid->CellTextOrDash("Parent", record->stat_group)));
        str_cells = item->ColumnText;
        str_cells->Add(WideString(NumberText(record->strength)));
        int_cells = item->ColumnText;
        int_cells->Add(WideString(NumberText(record->intelligence)));
        wis_cells = item->ColumnText;
        wis_cells->Add(WideString(NumberText(record->wisdom)));
        agi_cells = item->ColumnText;
        agi_cells->Add(WideString(NumberText(record->agility)));
        con_cells = item->ColumnText;
        con_cells->Add(WideString(NumberText(record->constitution)));
        cha_cells = item->ColumnText;
        cha_cells->Add(WideString(NumberText(record->charisma)));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled() && record->name.Length() < 2)
            item->Hidden = true;
    }
}

UiTabClasses::~UiTabClasses()
{
}

DEFINE_NUMBER_TEXT(UiTabClasses)
