#include <vcl.h>
#pragma hdrstop

#include "Uitabitems.h"
#include "UnitMain.h"
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Itemvalue.h"
#include "Itemvalues.h"

#pragma package(smart_init)

// Sets up the items tab: its grids, columns, type choices and their data columns, views, filter and viewers.
UiTabItems::UiTabItems(String pub_path, HINSTANCE__ *gfx_library, HINSTANCE__ *icon_library)
{
    main_grid = FormMain->ElTreeStringGrid1;
    sub_grid = FormMain->ElTreeStringGrid2;
    grid_ui = new DataGridUI(String("items"), String("Item"), main_grid, sub_grid, String("Type"));
    grid_ui->SetMainGraphics(gfx_library, "Gfx", 102, FormMain->gfx_vault->item_graphic_count, 2, false);
    grid_ui->SetSubGraphics(icon_library, "Gfx", 101, FormMain->gfx_vault->item_graphic_count, 2, false);
    item_values = new ItemValues(pub_path, false);
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Item Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        ItemGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Name", "Item Name", 200, ColumnKind_Text, "", 0, 24, ItemGroup_Basic,
        ColumnAlign_Left, ColumnFormat_Capitalized);
    grid_ui->data_grid->AddColumn("Gfx", "Item Gfx ID", 55, ColumnKind_Graphic, "", 1, 64008, ItemGroup_Basic,
        ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *type_column =
        grid_ui->data_grid->AddColumn("Type", "Item Type", 120, ColumnKind_Combo, "", 0, 252, ItemGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *subtype_column =
        grid_ui->data_grid->AddColumn("Subtype", "Item Subtype", 100, ColumnKind_Combo, "", 0, 252, ItemGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *unique_column =
        grid_ui->data_grid->AddColumn("Unique", "Item Unique", 120, ColumnKind_Combo, "", 0, 252, ItemGroup_Basic,
            ColumnAlign_Center, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Weight", "Item Weight", 90, ColumnKind_Number, "", 0, 252, ItemGroup_Basic,
        ColumnAlign_Left, ColumnFormat_None);
    DataGridCol *size_column =
        grid_ui->data_grid->AddColumn("Size", "Item Size", 80, ColumnKind_Combo, "Note: size in inventory", 0, 252,
            ItemGroup_Basic, ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *range_column =
        grid_ui->data_grid->AddColumn("Range", "Weapon Target Area", 85, ColumnKind_Combo, "", 0, 252,
            ItemGroup_TargetArea, ColumnAlign_Center, ColumnFormat_None);
    DataGridCol *first_data_column =
        grid_ui->data_grid->AddColumn("Data.1", "Item Data.1", 100, ColumnKind_Number, "", 0, 0xF71AE3, ItemGroup_Data1,
            ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *second_data_column =
        grid_ui->data_grid->AddColumn("Data.2", "Item Data.2", 90, ColumnKind_Number, "", 0, 252, ItemGroup_Data2And3,
            ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *third_data_column =
        grid_ui->data_grid->AddColumn("Data.3", "Item Data.3", 90, ColumnKind_Number, "", 0, 252, ItemGroup_Data2And3,
            ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Lvl", "Required Level", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *class_column =
        grid_ui->data_grid->AddColumn("Req:Cls", "Required Class", 120, ColumnKind_Link, "Link: class.name", 0, 0,
            ItemGroup_Requirements, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Str", "Required Strength", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Int", "Required Intelligence", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Wis", "Required Wisdom", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Agi", "Required Agility", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Con", "Required Constitution", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Req:Cha", "Required Charisma", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Requirements, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hp+", "Item Hitpoints", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_HpTp,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tp+", "Item Techpoints", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_HpTp,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-lo", "Item Damage Low", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Dmg-hi", "Item Damage High", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Hitrate", "Item Hitrate", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Evade", "Item Evade", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Armor", "Item Armor", 100, ColumnKind_Number, "", 0, 64008, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("R-Dmg", "Item Return Dmg", 100, ColumnKind_Number, "", 0, 252, ItemGroup_Battle,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Str", "Bonus Strength", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Int", "Bonus Intelligence", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Wis", "Bonus Wisdom", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Agi", "Bonus Agility", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Con", "Bonus Constitution", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Cha", "Bonus Charisma", 80, ColumnKind_Number, "", 0, 252, ItemGroup_StatBonus,
        ColumnAlign_Right, ColumnFormat_None);
    DataGridCol *element_column =
        grid_ui->data_grid->AddColumn("Element", "Item Element", 90, ColumnKind_Combo, "", 0, 252, ItemGroup_Element,
            ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Element.Dmg", "Item Element Dmg", 120, ColumnKind_Number, "", 0, 64008,
        ItemGroup_Element, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Light", "Resistance Light", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Resistance, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Dark", "Resistance Dark", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Resistance, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Earth", "Resistance Earth", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Resistance, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Air", "Resistance Air", 100, ColumnKind_Number, "", 0, 252, ItemGroup_Resistance,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Water", "Resistance Water", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Resistance, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("res:Fire", "Resistance Fire", 100, ColumnKind_Number, "", 0, 252,
        ItemGroup_Resistance, ColumnAlign_Right, ColumnFormat_None);
    DataComboItem *type_item;
    type_item = type_column->AddComboItemWithGroup(0, "general", ItemGroup_Basic);
    type_item = type_column->AddComboItemWithGroup(1, "static", ItemGroup_Basic);
    type_item = type_column->AddComboItemWithGroup(2, "currency", ItemGroup_Basic);
    type_item = type_column->AddComboItemWithTwoGroups(3, "potion", ItemGroup_Basic, ItemGroup_HpTp);
    DataComboItem *teleport_item = type_column->AddComboItemWithThreeGroups(4, "teleport", ItemGroup_Basic,
        ItemGroup_Data1, ItemGroup_Data2And3);
    DataComboItem *transform_item = type_column->AddComboItemWithTwoGroups(5, "transform", ItemGroup_Basic,
        ItemGroup_Data1);
    DataComboItem *exp_reward_item = type_column->AddComboItemWithTwoGroups(6, "exp reward", ItemGroup_Basic,
        ItemGroup_Data1);
    DataComboItem *skill_book_item = type_column->AddComboItemWithTwoGroups(7, "skill book", ItemGroup_Basic,
        ItemGroup_Data1);
    DataComboItem *visual_item = type_column->AddComboItemWithTwoGroups(8, "visual", ItemGroup_Basic, ItemGroup_Data1);
    DataComboItem *key_item = type_column->AddComboItemWithTwoGroups(9, "key", ItemGroup_Basic, ItemGroup_Data1);
    DataComboItem *weapon_item = type_column->AddComboItemWithNineGroups(10, "weapon", ItemGroup_Basic,
        ItemGroup_TargetArea, ItemGroup_Data1, ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle,
        ItemGroup_StatBonus, ItemGroup_Element, ItemGroup_Resistance);
    DataComboItem *shield_item = type_column->AddComboItemWithEightGroups(11, "shield", ItemGroup_Basic,
        ItemGroup_Data1, ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus,
        ItemGroup_Element, ItemGroup_Resistance);
    DataComboItem *armor_item = type_column->AddComboItemWithNineGroups(12, "armor", ItemGroup_Basic, ItemGroup_Data1,
        ItemGroup_Data2And3, ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus,
        ItemGroup_Element, ItemGroup_Resistance);
    DataComboItem *hat_item = type_column->AddComboItemWithEightGroups(13, "hat", ItemGroup_Basic, ItemGroup_Data1,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *boots_item = type_column->AddComboItemWithEightGroups(14, "boots", ItemGroup_Basic, ItemGroup_Data1,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *gloves_item = type_column->AddComboItemWithSevenGroups(15, "gloves", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *accessory_item = type_column->AddComboItemWithSevenGroups(16, "accessory", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *belt_item = type_column->AddComboItemWithSevenGroups(17, "belt", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *necklace_item = type_column->AddComboItemWithSevenGroups(18, "necklace", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *ring_item = type_column->AddComboItemWithSevenGroups(19, "ring", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *bracelet_item = type_column->AddComboItemWithSevenGroups(20, "bracelet", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *bracer_item = type_column->AddComboItemWithSevenGroups(21, "bracer", ItemGroup_Basic,
        ItemGroup_Requirements, ItemGroup_HpTp, ItemGroup_Battle, ItemGroup_StatBonus, ItemGroup_Element,
        ItemGroup_Resistance);
    DataComboItem *beverage_item = type_column->AddComboItemWithTwoGroups(22, "beverage", ItemGroup_Basic,
        ItemGroup_Data1);
    DataComboItem *effect_item = type_column->AddComboItemWithTwoGroups(23, "effect", ItemGroup_Basic, ItemGroup_Data1);
    DataComboItem *hairdye_item = type_column->AddComboItemWithTwoGroups(24, "hairdye", ItemGroup_Basic,
        ItemGroup_Data1);
    type_item = type_column->AddComboItemWithGroup(25, "cure", ItemGroup_Basic);
    DataComboItem *buff_item = type_column->AddComboItemWithTwoGroups(26, "buff*", ItemGroup_Basic, ItemGroup_Data1);
    DataComboItem *debuff_item = type_column->AddComboItemWithTwoGroups(27, "debuff*", ItemGroup_Basic,
        ItemGroup_Data1);
    DataComboItem *title_item = type_column->AddComboItemWithTwoGroups(28, "title", ItemGroup_Basic, ItemGroup_Data1);
    DataComboItem *hairtool_item = type_column->AddComboItemWithTwoGroups(29, "hairtool", ItemGroup_Basic,
        ItemGroup_Data1);
    type_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    subtype_column->AddComboItem(0, "-");
    subtype_column->AddComboItem(1, "bow");
    subtype_column->AddComboItem(2, "arrow");
    subtype_column->AddComboItem(3, "wing");
    subtype_column->AddComboItem(4, "2handed");
    subtype_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    unique_column->AddComboItem(0, "normal");
    unique_column->AddComboItem(1, "rare");
    unique_column->AddComboItem(2, "legendary");
    unique_column->AddComboItem(3, "unique");
    unique_column->AddComboItem(4, "lore");
    unique_column->AddComboItem(5, "cursed");
    unique_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    size_column->AddComboItem(0, "1x1");
    size_column->AddComboItem(1, "1x2");
    size_column->AddComboItem(2, "1x3");
    size_column->AddComboItem(3, "1x4");
    size_column->AddComboItem(4, "2x1");
    size_column->AddComboItem(5, "2x2");
    size_column->AddComboItem(6, "2x3");
    size_column->AddComboItem(7, "2x4");
    size_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    element_column->AddComboItem(0, "-");
    element_column->AddComboItem(1, "light");
    element_column->AddComboItem(2, "dark");
    element_column->AddComboItem(3, "earth");
    element_column->AddComboItem(4, "wind");
    element_column->AddComboItem(5, "water");
    element_column->AddComboItem(6, "fire");
    element_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    range_column->AddComboItem(0, "-");
    range_column->AddComboItem(1, "1");
    range_column->AddComboItem(2, "2");
    range_column->AddComboItem(3, "3");
    range_column->AddComboItem(4, "4");
    range_column->AddComboItem(5, "5");
    range_column->AddComboItem(6, "6");
    range_column->AddComboItem(7, "7");
    range_column->AddComboItem(8, "8");
    range_column->AddComboItem(9, "9");
    range_column->AddComboItem(10, "10");
    range_column->AddComboItem(11, "3x3 aoe");
    range_column->AddComboItem(12, "5x5 aoe");
    range_column->AddComboItem(13, "7x7 aoe");
    range_column->AddComboItem(14, "9x9 aoe");
    range_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    class_column->AddLink(FormMain->ElTreeStringGrid3, "Name");
    // The data columns mean something else for each item type, one variant each.
    first_data_column->AddSubColumn(teleport_item->text, "Teleport Map ID");
    second_data_column->AddSubColumn(teleport_item->text, "Teleport Coord X");
    third_data_column->AddSubColumn(teleport_item->text, "Teleport Coord Y");
    first_data_column->AddSubColumn(transform_item->text, "Transform Skin ID");
    first_data_column->AddSubColumn(skill_book_item->text, "Obtainable Skill ID");
    first_data_column->AddSubColumn(title_item->text, "Obtainable Title ID");
    first_data_column->AddSubColumn(visual_item->text, "Visual Resource ID");
    first_data_column->AddSubColumn(exp_reward_item->text, "Gained Experience");
    first_data_column->AddSubColumn(key_item->text, "Key Unlock ID");
    DataGridCol *weapon_graphic_column = first_data_column->AddSubColumn(weapon_item->text, "Weapon Resource ID");
    DataGridCol *shield_graphic_column = first_data_column->AddSubColumn(shield_item->text, "Shield Resource ID");
    DataGridCol *armor_graphic_column = first_data_column->AddSubColumn(armor_item->text, "Armor Resource ID");
    DataGridCol *gender_column =
        second_data_column->AddSubColumnWithHint(armor_item->text, "Armor Required Gender", "Values: 0:fem 1:male");
    DataGridCol *no_usage_column = third_data_column->AddSubColumn(armor_item->text, "Armor No-Usage");
    DataGridCol *hat_graphic_column = first_data_column->AddSubColumn(hat_item->text, "Hat Resource ID");
    DataGridCol *boots_graphic_column = first_data_column->AddSubColumn(boots_item->text, "Boots Resource ID");
    first_data_column->AddSubColumn(beverage_item->text, "Beverage Duration");
    first_data_column->AddSubColumn(effect_item->text, "Effect Spell ID");
    first_data_column->AddSubColumn(hairdye_item->text, "Hairdye Color ID");
    first_data_column->AddSubColumn(hairtool_item->text, "Hairmodal ID");
    first_data_column->AddSubColumn(buff_item->text, "Buff Spell ID");
    first_data_column->AddSubColumn(buff_item->text, "Debuff Spell ID");
    gender_column->kind = ColumnKind_SmartCombo;
    gender_column->AddComboItem(0, "female");
    gender_column->AddComboItem(1, "male");
    gender_column->WidenForComboItems(FormMain->config->GetNumberedLists());
    // The resource IDs of wearables are picked from their graphics.
    weapon_graphic_column->kind = ColumnKind_PaperdollGraphic;
    armor_graphic_column->kind = ColumnKind_PaperdollGraphic;
    shield_graphic_column->kind = ColumnKind_PaperdollGraphic;
    hat_graphic_column->kind = ColumnKind_PaperdollGraphic;
    boots_graphic_column->kind = ColumnKind_PaperdollGraphic;
    DataGridView *view = grid_ui->data_grid->AddView("basic view");
    view->AddGroup(ItemGroup_Basic);
    view = grid_ui->data_grid->AddView("battle view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_TargetArea);
    view->AddGroup(ItemGroup_Battle);
    view = grid_ui->data_grid->AddView("extended view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_TargetArea);
    view->AddGroup(ItemGroup_HpTp);
    view->AddGroup(ItemGroup_Battle);
    view->AddGroup(ItemGroup_StatBonus);
    view->AddGroup(ItemGroup_Element);
    view->AddGroup(ItemGroup_Resistance);
    view = grid_ui->data_grid->AddView("elemental view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_Element);
    view->AddGroup(ItemGroup_Resistance);
    view = grid_ui->data_grid->AddView("potion view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_HpTp);
    view = grid_ui->data_grid->AddView("requirement view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_Requirements);
    view = grid_ui->data_grid->AddView("statbonus view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_StatBonus);
    view = grid_ui->data_grid->AddView("teleport view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_Data1);
    view->AddGroup(ItemGroup_Data2And3);
    view = grid_ui->data_grid->AddView("weapon view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_TargetArea);
    view->AddGroup(ItemGroup_Requirements);
    view->AddGroup(ItemGroup_HpTp);
    view->AddGroup(ItemGroup_Battle);
    view->AddGroup(ItemGroup_StatBonus);
    view->AddGroup(ItemGroup_Element);
    view->AddGroup(ItemGroup_Resistance);
    view = grid_ui->data_grid->AddView("wearable view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_Requirements);
    view->AddGroup(ItemGroup_HpTp);
    view->AddGroup(ItemGroup_Battle);
    view->AddGroup(ItemGroup_StatBonus);
    view->AddGroup(ItemGroup_Element);
    view->AddGroup(ItemGroup_Resistance);
    view = grid_ui->data_grid->AddView("all view");
    view->AddGroup(ItemGroup_Basic);
    view->AddGroup(ItemGroup_TargetArea);
    view->AddGroup(ItemGroup_Data1);
    view->AddGroup(ItemGroup_Data2And3);
    view->AddGroup(ItemGroup_Requirements);
    view->AddGroup(ItemGroup_HpTp);
    view->AddGroup(ItemGroup_Battle);
    view->AddGroup(ItemGroup_StatBonus);
    view->AddGroup(ItemGroup_Element);
    view->AddGroup(ItemGroup_Resistance);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeItemView, 45, 235);
    TElPopupMenu *filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeItemFilterType, FormMain->imageChangeItemView, 145, 235,
        "Type", "All Items");
    grid_ui->SetEditPanel(FormMain->PanelSaveItemsHint);
    grid_ui->SetSearchEdit(FormMain->editSearchItems);
    grid_ui->SetInfoLabels(FormMain->labelItemInspectorFilter, FormMain->labelItemInspectorID,
        FormMain->labelItemInspectorName, FormMain->imageItemInspector, "Type");
    grid_ui->SetGfxViewer(FormMain->imageItem, FormMain->panelItemImage, FormMain->labelItemGfxName,
        FormMain->labelItemGfxID);
    grid_ui->SetIconImage(FormMain->imageItem2);
    grid_ui->SetColumnLabel(FormMain->labelItemAdditional, "Size");
    grid_ui->SetMessagePanel(FormMain->panelInfoHostItems, FormMain->panelInsideInfoItems);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the items from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabItems::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    item_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (item_values->header_read)
    {
        String part_path = item_values->file_path;
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

// Fills the grid with a row per item; notes a record named EOF and hides blank names unless shown.
void UiTabItems::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *name_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *type_cells;
    TElWideStrings *subtype_cells;
    TElWideStrings *unique_cells;
    TElWideStrings *weight_cells;
    TElWideStrings *size_cells;
    TElWideStrings *range_cells;
    TElWideStrings *first_data_cells;
    TElWideStrings *second_data_cells;
    TElWideStrings *third_data_cells;
    TElWideStrings *level_requirement_cells;
    TElWideStrings *class_requirement_cells;
    TElWideStrings *str_requirement_cells;
    TElWideStrings *int_requirement_cells;
    TElWideStrings *wis_requirement_cells;
    TElWideStrings *agi_requirement_cells;
    TElWideStrings *con_requirement_cells;
    TElWideStrings *cha_requirement_cells;
    TElWideStrings *hp_cells;
    TElWideStrings *tp_cells;
    TElWideStrings *min_damage_cells;
    TElWideStrings *max_damage_cells;
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
    for (int i = 0; i < (unsigned int)item_values->Count(); i++)
    {
        ItemValue *record = item_values->GetItem(i);
        if (LowerCase(record->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(record->item_id));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(record->name));
        gfx_cells = item->ColumnText;
        gfx_cells->Add(WideString(NumberText(record->graphic)));
        type_cells = item->ColumnText;
        type_cells->Add(WideString(grid_ui->data_grid->CellText("Type", record->type)));
        subtype_cells = item->ColumnText;
        subtype_cells->Add(WideString(grid_ui->data_grid->CellText("Subtype", record->subtype)));
        unique_cells = item->ColumnText;
        unique_cells->Add(WideString(grid_ui->data_grid->CellText("Unique", record->special)));
        weight_cells = item->ColumnText;
        weight_cells->Add(WideString(NumberText(record->weight)));
        size_cells = item->ColumnText;
        size_cells->Add(WideString(grid_ui->data_grid->CellText("Size", record->size)));
        range_cells = item->ColumnText;
        range_cells->Add(WideString(grid_ui->data_grid->CellText("Range", record->spare_byte)));
        first_data_cells = item->ColumnText;
        first_data_cells->Add(WideString(NumberText(record->spec_value)));
        second_data_cells = item->ColumnText;
        second_data_cells->Add(WideString(NumberText(record->gender)));
        third_data_cells = item->ColumnText;
        third_data_cells->Add(WideString(NumberText(record->spec3)));
        level_requirement_cells = item->ColumnText;
        level_requirement_cells->Add(WideString(NumberText(record->required_level)));
        class_requirement_cells = item->ColumnText;
        class_requirement_cells->Add(WideString(grid_ui->data_grid->CellTextOrDash("Req:Cls", record->required_class)));
        str_requirement_cells = item->ColumnText;
        str_requirement_cells->Add(WideString(NumberText(record->required_str)));
        int_requirement_cells = item->ColumnText;
        int_requirement_cells->Add(WideString(NumberText(record->required_int)));
        wis_requirement_cells = item->ColumnText;
        wis_requirement_cells->Add(WideString(NumberText(record->required_wis)));
        agi_requirement_cells = item->ColumnText;
        agi_requirement_cells->Add(WideString(NumberText(record->required_agi)));
        con_requirement_cells = item->ColumnText;
        con_requirement_cells->Add(WideString(NumberText(record->required_con)));
        cha_requirement_cells = item->ColumnText;
        cha_requirement_cells->Add(WideString(NumberText(record->required_cha)));
        hp_cells = item->ColumnText;
        hp_cells->Add(WideString(NumberText(record->hp)));
        tp_cells = item->ColumnText;
        tp_cells->Add(WideString(NumberText(record->tp)));
        min_damage_cells = item->ColumnText;
        min_damage_cells->Add(WideString(NumberText(record->min_damage)));
        max_damage_cells = item->ColumnText;
        max_damage_cells->Add(WideString(NumberText(record->max_damage)));
        item->ColumnText->Add(WideString(NumberText(record->accuracy)));
        item->ColumnText->Add(WideString(NumberText(record->evade)));
        item->ColumnText->Add(WideString(NumberText(record->armor)));
        item->ColumnText->Add(WideString(NumberText(record->return_damage)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_str)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_int)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_wis)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_agi)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_con)));
        item->ColumnText->Add(WideString(NumberText(record->bonus_cha)));
        item->ColumnText->Add(WideString(grid_ui->data_grid->CellText("Element", record->element)));
        item->ColumnText->Add(WideString(NumberText(record->element_power)));
        item->ColumnText->Add(WideString(NumberText(record->light_resist)));
        item->ColumnText->Add(WideString(NumberText(record->dark_resist)));
        item->ColumnText->Add(WideString(NumberText(record->earth_resist)));
        item->ColumnText->Add(WideString(NumberText(record->air_resist)));
        item->ColumnText->Add(WideString(NumberText(record->water_resist)));
        item->ColumnText->Add(WideString(NumberText(record->fire_resist)));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled())
        {
            String *item_name = &record->name;
            if (item_name->Length() < 2)
                item->Hidden = true;
        }
    }
}

UiTabItems::~UiTabItems()
{
}

DEFINE_NUMBER_TEXT(UiTabItems)
