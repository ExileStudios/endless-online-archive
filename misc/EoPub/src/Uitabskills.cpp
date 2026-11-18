#include <vcl.h>
#pragma hdrstop

#include "Uitabskills.h"
#include "UnitMain.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Skillvalue.h"
#include "Skillvalues.h"

#pragma package(smart_init)

// Sets up the skills tab: its grids, columns, choices, views, filter, gfx viewer and icon.
UiTabSkills::UiTabSkills(String pub_path, HINSTANCE__ *gfx_library, HINSTANCE__ *icon_library)
{
    main_grid = FormMain->ElTreeStringGrid7;
    sub_grid = FormMain->ElTreeStringGrid8;
    grid_ui = new DataGridUI(String("skills"), String("Skill"), main_grid, sub_grid, String("Type"));
    grid_ui->SetMainGraphics(gfx_library, "Effect Gfx", 101, FormMain->gfx_vault->skill_graphic_count, 3, 2);
    grid_ui->SetSubGraphics(icon_library, "Gfx", 101, FormMain->gfx_vault->icon_graphic_count, 1, true);
    skill_values = new SkillValues(pub_path, false);
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Skill Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        SkillGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Name", "Skill Name", 160, ColumnKind_Text, "", 0, 24, SkillGroup_Basic,
        ColumnAlign_Left, ColumnFormat_Capitalized);
    grid_ui->data_grid->AddColumn("Gfx", "Skill Icon Gfx", 65, ColumnKind_Graphic, "", 1, 64008, SkillGroup_Basic,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Effect Gfx", "Skill Effect Gfx ID", 100, ColumnKind_Graphic, "", 1, 64008,
        SkillGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Chant", "Skill Chant", 180, ColumnKind_Text, "", 0, 24, SkillGroup_Basic,
        ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *nature_column =
        grid_ui->data_grid->AddColumn("Nature", "Skill Nature", 60, ColumnKind_Combo, "", 0, 252, SkillGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *type_column =
        grid_ui->data_grid->AddColumn("Type", "Skill Type", 70, ColumnKind_Combo, "", 0, 0xF71AE3, SkillGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *target_column =
        grid_ui->data_grid->AddColumn("Target", "Skill Target", 75, ColumnKind_Combo, "", 0, 252, SkillGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *target_type_column =
        grid_ui->data_grid->AddColumn("Target Type", "Skill Target Type", 115, ColumnKind_Combo, "", 0, 252,
            SkillGroup_Basic, ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *range_column =
        grid_ui->data_grid->AddColumn("Range", "Skill Range Area", 100, ColumnKind_Combo, "", 0, 252, SkillGroup_Target,
            ColumnAlign_Center, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Target Time", "Target Time", 110, ColumnKind_Number, "", 0, 252, SkillGroup_Target,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tp Cost", "Skill Tp Cost", 100, ColumnKind_Number, "", 0, 64008,
        SkillGroup_Requirements, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Sp Cost", "Skill Sp Cost", 100, ColumnKind_Number, "", 0, 64008,
        SkillGroup_Requirements, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Delay", "Skill Casting Delay", 90, ColumnKind_Number, "", 0, 252,
        SkillGroup_Requirements, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Max Lvl", "Skill Maximum Level", 100, ColumnKind_Number, "", 0, 64008,
        SkillGroup_Requirements, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hp+", "Healed Hitpoints", 90, ColumnKind_Number, "", 0, 64008, SkillGroup_Heal,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tp+", "Healed Techpoints", 90, ColumnKind_Number, "", 0, 64008, SkillGroup_Heal,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Sp+", "Healed Stamina", 90, ColumnKind_Number, "", 0, 252, SkillGroup_Heal,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-lo", "Skill Damage Low", 100, ColumnKind_Number, "", 0, 64008, SkillGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-hi", "Skill Damage High", 100, ColumnKind_Number, "", 0, 64008,
        SkillGroup_Battle, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hitrate", "Skill Hitrate", 100, ColumnKind_Number, "", 0, 64008, SkillGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Evade", "Skill Evade", 100, ColumnKind_Number, "", 0, 64008, SkillGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Armor", "Skill Armor", 100, ColumnKind_Number, "", 0, 64008, SkillGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("R-Dmg", "Skill Return Dmg", 100, ColumnKind_Number, "", 0, 252, SkillGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Str", "Skill Strength", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Int", "Skill Intelligence", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Wis", "Skill Wisdom", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Agi", "Skill Agility", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Con", "Skill Constitution", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Cha", "Skill Charisma", 80, ColumnKind_Number, "", 0, 252, SkillGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *element_column =
        grid_ui->data_grid->AddColumn("Element", "Item Element", 90, ColumnKind_Combo, "", 0, 252, SkillGroup_Element,
            ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Element.Dmg", "Item Element Dmg", 120, ColumnKind_Number, "", 0, 64008,
        SkillGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *reserve_column =
        grid_ui->data_grid->AddColumn("Reserve", "Skill Reserve", 90, ColumnKind_Number, "Note: do not use", 0, 252,
            SkillGroup_Reserve, ColumnAlign_Right, ColumnFormat_None);
    nature_column->AddComboItem(0, "spell");
    nature_column->AddComboItem(1, "skill");
    nature_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    type_column->AddComboItemWithFiveGroups(0, "heal", SkillGroup_Basic, SkillGroup_Target, SkillGroup_Requirements,
        SkillGroup_Heal, SkillGroup_StatBonus);
    type_column->AddComboItemWithFiveGroups(1, "attack", SkillGroup_Basic, SkillGroup_Target, SkillGroup_Requirements,
        SkillGroup_Battle, SkillGroup_Element);
    type_column->AddComboItemWithTwoGroups(2, "bard", SkillGroup_Basic, SkillGroup_Requirements);
    type_column->AddComboItemWithTwoGroups(3, "discover", SkillGroup_Basic, SkillGroup_Requirements);
    type_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    target_column->AddComboItem(0, "target");
    target_column->AddComboItem(1, "self");
    target_column->AddComboItem(2, "any");
    target_column->AddComboItem(3, "group");
    target_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    target_type_column->AddComboItem(0, "npc-only");
    target_type_column->AddComboItem(1, "friend");
    target_type_column->AddComboItem(2, "npc+pk");
    target_type_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    element_column->AddComboItem(0, "-");
    element_column->AddComboItem(1, "light");
    element_column->AddComboItem(2, "dark");
    element_column->AddComboItem(3, "earth");
    element_column->AddComboItem(4, "wind");
    element_column->AddComboItem(5, "water");
    element_column->AddComboItem(6, "fire");
    element_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    range_column->AddComboItem(0, "-");
    range_column->AddComboItem(1, "1x1 aoe");
    range_column->AddComboItem(2, "3x3 aoe");
    range_column->AddComboItem(3, "5x5 aoe");
    range_column->AddComboItem(4, "7x7 aoe");
    range_column->AddComboItem(5, "9x9 aoe");
    range_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    DataGridView *view = grid_ui->data_grid->AddView("basic view");
    view->AddGroup(SkillGroup_Basic);
    view = grid_ui->data_grid->AddView("elemental view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Element);
    view = grid_ui->data_grid->AddView("statbonus view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_StatBonus);
    view = grid_ui->data_grid->AddView("requirements view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Requirements);
    view = grid_ui->data_grid->AddView("spell battle view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Requirements);
    view->AddGroup(SkillGroup_Battle);
    view = grid_ui->data_grid->AddView("spell extended view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Requirements);
    view->AddGroup(SkillGroup_Battle);
    view->AddGroup(SkillGroup_Element);
    view = grid_ui->data_grid->AddView("spell heal view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Heal);
    view = grid_ui->data_grid->AddView("spell buff view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Heal);
    view->AddGroup(SkillGroup_StatBonus);
    view = grid_ui->data_grid->AddView("skill view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Target);
    view->AddGroup(SkillGroup_Requirements);
    view->AddGroup(SkillGroup_Battle);
    view = grid_ui->data_grid->AddView("skill extended view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Target);
    view->AddGroup(SkillGroup_Requirements);
    view->AddGroup(SkillGroup_Battle);
    view->AddGroup(SkillGroup_Element);
    view = grid_ui->data_grid->AddView("all view");
    view->AddGroup(SkillGroup_Basic);
    view->AddGroup(SkillGroup_Target);
    view->AddGroup(SkillGroup_Requirements);
    view->AddGroup(SkillGroup_Heal);
    view->AddGroup(SkillGroup_Battle);
    view->AddGroup(SkillGroup_StatBonus);
    view->AddGroup(SkillGroup_Element);
    view->AddGroup(SkillGroup_Reserve);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeSkillView, 45, 235);
    TElPopupMenu *filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeSkillFilterType, FormMain->imageChangeSkillView, 145, 235,
        "Type", "All Skills");
    grid_ui->SetEditPanel(FormMain->PanelSaveSkillsHint);
    grid_ui->SetSearchEdit(FormMain->editSearchSkills);
    grid_ui->SetInfoLabels(FormMain->labelSkillInspectorFilter, FormMain->labelSkillInspectorID,
        FormMain->labelSkillInspectorName, FormMain->imageSkillInspector, "Type");
    grid_ui->SetGfxViewer(FormMain->imageSkill, FormMain->panelSkillImage, FormMain->labelSkillGfxName,
        FormMain->labelSkillGfxID);
    grid_ui->SetIconImage(FormMain->imageIconSkill);
    grid_ui->SetMessagePanel(FormMain->panelInfoHostSkills, FormMain->panelInsideInfoSkills);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the skills from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabSkills::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    skill_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (skill_values->header_read)
    {
        String part_path = skill_values->file_path;
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

// Fills the grid with a row per skill; notes a record named EOF and hides blank names unless shown.
void UiTabSkills::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *name_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *effect_gfx_cells;
    TElWideStrings *chant_cells;
    TElWideStrings *nature_cells;
    TElWideStrings *type_cells;
    TElWideStrings *target_cells;
    TElWideStrings *target_type_cells;
    TElWideStrings *range_cells;
    TElWideStrings *target_time_cells;
    TElWideStrings *tp_cost_cells;
    TElWideStrings *sp_cost_cells;
    TElWideStrings *delay_cells;
    TElWideStrings *max_level_cells;
    TElWideStrings *healed_hp_cells;
    TElWideStrings *healed_tp_cells;
    TElWideStrings *healed_sp_cells;
    TElWideStrings *min_damage_cells;
    TElWideStrings *max_damage_cells;
    TElWideStrings *accuracy_cells;
    TElWideStrings *evade_cells;
    TElWideStrings *armor_cells;
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
    for (int i = 0; i < (unsigned int)skill_values->Count(); i++)
    {
        SkillValue *skill = skill_values->GetSkill(i);
        if (LowerCase(skill->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(skill->skill_id));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(skill->name));
        gfx_cells = item->ColumnText;
        gfx_cells->Add(WideString(NumberText(skill->icon)));
        effect_gfx_cells = item->ColumnText;
        effect_gfx_cells->Add(WideString(NumberText(skill->graphic)));
        chant_cells = item->ColumnText;
        chant_cells->Add(WideString(skill->shout));
        nature_cells = item->ColumnText;
        nature_cells->Add(WideString(grid_ui->data_grid->CellText("Nature", skill->category)));
        type_cells = item->ColumnText;
        type_cells->Add(WideString(grid_ui->data_grid->CellText("Type", skill->type)));
        target_cells = item->ColumnText;
        target_cells->Add(WideString(grid_ui->data_grid->CellText("Target", skill->target_type)));
        target_type_cells = item->ColumnText;
        target_type_cells->Add(WideString(grid_ui->data_grid->CellText("Target Type", skill->target_restriction)));
        range_cells = item->ColumnText;
        range_cells->Add(WideString(grid_ui->data_grid->CellText("Range", skill->area)));
        target_time_cells = item->ColumnText;
        target_time_cells->Add(WideString(NumberText(skill->target_time)));
        tp_cost_cells = item->ColumnText;
        tp_cost_cells->Add(WideString(NumberText(skill->tp_cost)));
        sp_cost_cells = item->ColumnText;
        sp_cost_cells->Add(WideString(NumberText(skill->sp_cost)));
        delay_cells = item->ColumnText;
        delay_cells->Add(WideString(NumberText(skill->cast_time)));
        max_level_cells = item->ColumnText;
        max_level_cells->Add(WideString(NumberText(skill->max_level)));
        healed_hp_cells = item->ColumnText;
        healed_hp_cells->Add(WideString(NumberText(skill->hp_heal)));
        healed_tp_cells = item->ColumnText;
        healed_tp_cells->Add(WideString(NumberText(skill->tp_heal)));
        healed_sp_cells = item->ColumnText;
        healed_sp_cells->Add(WideString(NumberText(skill->sp_heal)));
        min_damage_cells = item->ColumnText;
        min_damage_cells->Add(WideString(NumberText(skill->min_damage)));
        max_damage_cells = item->ColumnText;
        max_damage_cells->Add(WideString(NumberText(skill->max_damage)));
        accuracy_cells = item->ColumnText;
        accuracy_cells->Add(WideString(NumberText(skill->accuracy)));
        evade_cells = item->ColumnText;
        evade_cells->Add(WideString(NumberText(skill->evade)));
        armor_cells = item->ColumnText;
        armor_cells->Add(WideString(NumberText(skill->armor)));
        item->ColumnText->Add(WideString(NumberText(skill->return_damage)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_str)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_int)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_wis)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_agi)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_con)));
        item->ColumnText->Add(WideString(NumberText(skill->bonus_cha)));
        item->ColumnText->Add(WideString(grid_ui->data_grid->CellText("Element", skill->element)));
        item->ColumnText->Add(WideString(NumberText(skill->element_power)));
        item->ColumnText->Add(WideString(NumberText(skill->spare_byte)));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled())
        {
            String *skill_name = &skill->name;
            if (skill_name->Length() < 2)
                item->Hidden = true;
        }
    }
}

UiTabSkills::~UiTabSkills()
{
}

DEFINE_NUMBER_TEXT(UiTabSkills)
