#include <vcl.h>
#pragma hdrstop

#include "Uitabnpcs.h"
#include "UnitMain.h"
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Npcdataadapter.h"
#include "Npcvalue.h"
#include "Npcvalues.h"
#include <vector.h>

#pragma package(smart_init)

// Sets up the NPCs tab: its grids, columns, behaviour and element choices, views, filter and gfx viewer.
UiTabNpcs::UiTabNpcs(String pub_path, HINSTANCE__ *gfx_library, int icon_library)
{
    main_grid = FormMain->ElTreeStringGrid5;
    sub_grid = FormMain->ElTreeStringGrid6;
    grid_ui = new DataGridUI(String("npcs"), String("Npc"), main_grid, sub_grid, String("Behaviour"));
    grid_ui->SetMainGraphics(gfx_library, "Gfx", 101, FormMain->gfx_vault->npc_graphic_count, 40, false);
    npc_values = new NpcValues(pub_path, false);
    adapter = new NpcDataAdapter();
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Npc Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        NpcGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Name", "Npc Name", 200, ColumnKind_Text, "", 0, 24, NpcGroup_Basic, ColumnAlign_Left,
        ColumnFormat_Capitalized);
    grid_ui->data_grid->AddColumn("Gfx", "Npc Gfx ID", 55, ColumnKind_Graphic, "", 1, 64008, NpcGroup_Basic,
        ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Level", "Npc Level", 70, ColumnKind_Number, "", 0, 252, NpcGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Experience", "Npc Experience", 120, ColumnKind_Number, "", 0, 0xF71AE3,
        NpcGroup_Basic, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Drop", "Npc Item Drop", 64, ColumnKind_NpcDrops, "", 1, 64008, NpcGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Chat", "Npc Chat Lines", 64, ColumnKind_NpcChats, "", 1, 64008, NpcGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *behaviour_column =
        grid_ui->data_grid->AddColumn("Behaviour", "Npc Behaviour", 120, ColumnKind_Combo, "", 0, 64008, NpcGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *behaviour_id_column =
        grid_ui->data_grid->AddColumn("Beh.ID", "Npc Behaviour ID", 78, ColumnKind_Number, "", 0, 64008, NpcGroup_Basic,
            ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hp", "Npc Hitpoints", 100, ColumnKind_Number, "", 0, 0xF71AE3, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tp", "Npc Techpoints", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-lo", "Npc Damage Low", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-hi", "Npc Damage High", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hitrate", "Npc Hitrate", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Evade", "Npc Evade", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Armor", "Npc Armor", 100, ColumnKind_Number, "", 0, 64008, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("R-Dmg", "Npc Return Dmg", 100, ColumnKind_Number, "", 0, 252, NpcGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Race", "Npc Race", 85, ColumnKind_Number, "Link: behaviour [npc killer]", 0, 252,
        NpcGroup_RaceBoss, ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *boss_column = grid_ui->data_grid->AddColumn("Boss", "Npc Boss", 80, ColumnKind_Combo, "", 0, 64008,
        NpcGroup_RaceBoss, ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *boss_add_column =
        grid_ui->data_grid->AddColumn("Add", "Npc Boss Add", 100, ColumnKind_Combo, "", 0, 64008, NpcGroup_RaceBoss,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *attack_element_column =
        grid_ui->data_grid->AddColumn("Elem-A", "Npc Elem. Attack", 60, ColumnKind_Combo, "", 0, 64008,
            NpcGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Elem-A.dmg ", "Npc Elem. Attack Dmg", 120, ColumnKind_Number, "", 0, 64008,
        NpcGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *weakness_element_column =
        grid_ui->data_grid->AddColumn("Elem-W", "Npc Elem. Weakness", 60, ColumnKind_Combo, "", 0, 64008,
            NpcGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Elem-W.dmg ", "Npc Elem. Weakness Dmg", 120, ColumnKind_Number, "", 0, 64008,
        NpcGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    behaviour_column->AddComboItemWithGroup(0, "friendly", NpcGroup_Basic);
    DataComboItem *passive_item = behaviour_column->AddComboItemWithFourGroups(1, "passive", NpcGroup_Basic,
        NpcGroup_Battle, NpcGroup_RaceBoss, NpcGroup_Element);
    DataComboItem *aggressive_item = behaviour_column->AddComboItemWithFourGroups(2, "aggressive", NpcGroup_Basic,
        NpcGroup_Battle, NpcGroup_RaceBoss, NpcGroup_Element);
    behaviour_column->AddComboItemWithGroup(3, "pet", NpcGroup_Basic);
    behaviour_column->AddComboItemWithFourGroups(4, "npc mine", NpcGroup_Basic, NpcGroup_Battle, NpcGroup_RaceBoss,
        NpcGroup_Element);
    behaviour_column->AddComboItemWithFourGroups(5, "npc killer", NpcGroup_Basic, NpcGroup_Battle, NpcGroup_RaceBoss,
        NpcGroup_Element);
    DataComboItem *shop_item = behaviour_column->AddComboItemWithGroup(6, "shop", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(7, "inn", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(8, "locker", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(9, "banker", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(10, "barber", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(11, "guild", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(12, "priest", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(13, "lawyer", NpcGroup_Basic);
    DataComboItem *trainer_item = behaviour_column->AddComboItemWithGroup(14, "trainer", NpcGroup_Basic);
    behaviour_column->AddComboItemWithGroup(15, "quest", NpcGroup_Basic);
    behaviour_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    boss_column->AddComboItem(0, "-");
    boss_column->AddComboItem(1, "boss");
    boss_column->AddComboItem(2, "endboss");
    boss_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    boss_add_column->AddComboItem(0, "-");
    boss_add_column->AddComboItem(1, "boss child");
    boss_add_column->AddComboItem(2, "boss summon");
    boss_add_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    attack_element_column->AddComboItem(0, "-");
    attack_element_column->AddComboItem(1, "light");
    attack_element_column->AddComboItem(2, "dark");
    attack_element_column->AddComboItem(3, "earth");
    attack_element_column->AddComboItem(4, "wind");
    attack_element_column->AddComboItem(5, "water");
    attack_element_column->AddComboItem(6, "fire");
    attack_element_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    weakness_element_column->AddComboItem(0, "-");
    weakness_element_column->AddComboItem(1, "light");
    weakness_element_column->AddComboItem(2, "dark");
    weakness_element_column->AddComboItem(3, "earth");
    weakness_element_column->AddComboItem(4, "wind");
    weakness_element_column->AddComboItem(5, "water");
    weakness_element_column->AddComboItem(6, "fire");
    weakness_element_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    // The behaviour ID names a shop, a skill master or kill protection, by the behaviour.
    behaviour_id_column->AddSubColumn(shop_item->text, "Npc Shop ID");
    behaviour_id_column->AddSubColumn(trainer_item->text, "Npc Trainer ID");
    DataGridCol *passive_protection_column =
        behaviour_id_column->AddSubColumn(passive_item->text, "Killsteal Protection");
    DataGridCol *aggressive_protection_column =
        behaviour_id_column->AddSubColumn(aggressive_item->text, "Killsteal Protection");
    passive_protection_column->kind = ColumnKind_SmartCombo;
    passive_protection_column->AddComboItem(0, "protect");
    passive_protection_column->AddComboItem(1, "none");
    passive_protection_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    aggressive_protection_column->kind = ColumnKind_SmartCombo;
    aggressive_protection_column->AddComboItem(0, "protect");
    aggressive_protection_column->AddComboItem(1, "none");
    aggressive_protection_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    DataGridView *view = grid_ui->data_grid->AddView("basic view");
    view->AddGroup(NpcGroup_Basic);
    view = grid_ui->data_grid->AddView("battle view");
    view->AddGroup(NpcGroup_Basic);
    view->AddGroup(NpcGroup_Battle);
    view = grid_ui->data_grid->AddView("extended view");
    view->AddGroup(NpcGroup_Basic);
    view->AddGroup(NpcGroup_Battle);
    view->AddGroup(NpcGroup_RaceBoss);
    view = grid_ui->data_grid->AddView("elemental view");
    view->AddGroup(NpcGroup_Basic);
    view->AddGroup(NpcGroup_Element);
    view = grid_ui->data_grid->AddView("all view");
    view->AddGroup(NpcGroup_Basic);
    view->AddGroup(NpcGroup_Battle);
    view->AddGroup(NpcGroup_RaceBoss);
    view->AddGroup(NpcGroup_Element);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeNpcView, 45, 235);
    TElPopupMenu *filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeNpcFilterType, FormMain->imageChangeNpcFilterType, 145,
        235, "Behaviour", "All Npcs");
    grid_ui->SetEditPanel(FormMain->PanelSaveNpcsHint);
    grid_ui->SetSearchEdit(FormMain->editSearchNpcs);
    grid_ui->SetInfoLabels(FormMain->labelNpcInspectorFilter, FormMain->labelNpcInspectorID,
        FormMain->labelNpcInspectorName, FormMain->imageNpcInspector, "Behaviour");
    grid_ui->SetGfxViewer(FormMain->imageNpc, FormMain->panelNpcImage, FormMain->labelNpcGfxName,
        FormMain->labelNpcGfxID);
    grid_ui->SetMessagePanel(FormMain->panelInfoHostNpcs, FormMain->panelInsideInfoNpcs);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the NPCs from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabNpcs::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    npc_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (npc_values->header_read)
    {
        String part_path = npc_values->file_path;
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

// Fills the grid with a row per NPC, with its drop and chat counts; hides blank names unless shown.
void UiTabNpcs::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *name_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *level_cells;
    TElWideStrings *experience_cells;
    TElWideStrings *behaviour_cells;
    TElWideStrings *behaviour_id_cells;
    TElWideStrings *hp_cells;
    TElWideStrings *tp_cells;
    TElWideStrings *min_damage_cells;
    TElWideStrings *max_damage_cells;
    TElWideStrings *accuracy_cells;
    TElWideStrings *evade_cells;
    TElWideStrings *armor_cells;
    TElWideStrings *return_damage_cells;
    TElWideStrings *race_cells;
    TElWideStrings *boss_cells;
    TElWideStrings *boss_add_cells;
    TElWideStrings *attack_element_cells;
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
    for (int i = 0; i < (unsigned int)npc_values->Count(); i++)
    {
        NpcValue *npc = npc_values->GetNpc(i);
        if (LowerCase(npc->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(npc->npc_id));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(npc->name));
        gfx_cells = item->ColumnText;
        gfx_cells->Add(WideString(NumberText(npc->graphic)));
        level_cells = item->ColumnText;
        level_cells->Add(WideString(NumberText(npc->level)));
        experience_cells = item->ColumnText;
        experience_cells->Add(WideString(NumberText(npc->experience)));
        item->ColumnText->Add(WideString(NumberText(npc->drops.size())));
        item->ColumnText->Add(WideString(NumberText(npc->talk_lines.size())));
        behaviour_cells = item->ColumnText;
        behaviour_cells->Add(WideString(grid_ui->data_grid->CellText("Behaviour", npc->npc_type)));
        behaviour_id_cells = item->ColumnText;
        behaviour_id_cells->Add(WideString(NumberText(npc->behavior_id)));
        hp_cells = item->ColumnText;
        hp_cells->Add(WideString(NumberText(npc->hp)));
        tp_cells = item->ColumnText;
        tp_cells->Add(WideString(NumberText(npc->tp)));
        min_damage_cells = item->ColumnText;
        min_damage_cells->Add(WideString(NumberText(npc->min_damage)));
        max_damage_cells = item->ColumnText;
        max_damage_cells->Add(WideString(NumberText(npc->max_damage)));
        accuracy_cells = item->ColumnText;
        accuracy_cells->Add(WideString(NumberText(npc->accuracy)));
        evade_cells = item->ColumnText;
        evade_cells->Add(WideString(NumberText(npc->evade)));
        armor_cells = item->ColumnText;
        armor_cells->Add(WideString(NumberText(npc->armor)));
        return_damage_cells = item->ColumnText;
        return_damage_cells->Add(WideString(NumberText(npc->return_damage)));
        race_cells = item->ColumnText;
        race_cells->Add(WideString(NumberText(npc->race)));
        boss_cells = item->ColumnText;
        boss_cells->Add(WideString(grid_ui->data_grid->CellText("Boss", npc->boss)));
        boss_add_cells = item->ColumnText;
        boss_add_cells->Add(WideString(grid_ui->data_grid->CellText("Add", npc->child)));
        attack_element_cells = item->ColumnText;
        attack_element_cells->Add(WideString(grid_ui->data_grid->CellText("Elem-A", npc->element)));
        item->ColumnText->Add(WideString(NumberText(npc->element_damage)));
        item->ColumnText->Add(WideString(grid_ui->data_grid->CellText("Elem-W", npc->element_weakness)));
        item->ColumnText->Add(WideString(NumberText(npc->element_weakness_damage)));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled())
        {
            String *npc_name = &npc->name;
            if (npc_name->Length() < 2)
                item->Hidden = true;
        }
    }
}

UiTabNpcs::~UiTabNpcs()
{
}

DEFINE_NUMBER_TEXT(UiTabNpcs)
