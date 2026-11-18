#include <vcl.h>
#pragma hdrstop

#include "Uitabshops.h"
#include "UnitMain.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Shopdataadapter.h"
#include "Shopvalue.h"
#include "Shopvalues.h"
#include <vector.h>

#pragma package(smart_init)

UiTabShops::UiTabShops(String pub_path, int gfx_library, int icon_library)
{
    main_grid = FormMain->ElTreeStringGrid9;
    sub_grid = FormMain->ElTreeStringGrid10;
    grid_ui = new DataGridUI(String("shops"), String("Shop"), main_grid, sub_grid, String(""));
    shop_values = new ShopValues(pub_path);
    adapter = new ShopDataAdapter();
    grid_ui->data_grid->SetGrids(grid_ui->main_grid, grid_ui->sub_grid);
    grid_ui->data_grid->AddColumn("Index", "Shop Index", 60, ColumnKind_Index, "Edit: Click column to edit", 0, 24,
        ShopGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Tag", "Shop id-Tag", 70, ColumnKind_Number, "* linked id by eoserv", 0, 64008,
        ShopGroup_Basic, ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Name", "Shop Name", 260, ColumnKind_Text, "", 0, 24, ShopGroup_Basic,
        ColumnAlign_Left, ColumnFormat_Capitalized);
    grid_ui->data_grid->AddColumn("Req:Lvl.min", "Required Min Level", 114, ColumnKind_Number, "", 0, 252,
        ShopGroup_Basic, ColumnAlign_Right, ColumnFormat_KeepZero);
    grid_ui->data_grid->AddColumn("Req:Lvl.max", "Required Max Level", 114, ColumnKind_Number, "", 0, 252,
        ShopGroup_Basic, ColumnAlign_Left, ColumnFormat_KeepZero);
    DataGridCol *class_column =
        grid_ui->data_grid->AddColumn("Req:Cls", "Required Class", 130, ColumnKind_Link, "Link: class.name", 0, 0,
            ShopGroup_Basic, ColumnAlign_Left, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Trade", "Shop Trade Items", 64, ColumnKind_ShopTrades, "", 1, 64008, ShopGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    grid_ui->data_grid->AddColumn("Craft", "Shop Craft Items", 64, ColumnKind_ShopCrafts, "", 1, 64008, ShopGroup_Basic,
        ColumnAlign_Right, ColumnFormat_None);
    class_column->AddLink(FormMain->ElTreeStringGrid3, "Name");
    DataGridView *view = grid_ui->data_grid->AddView("basic view");
    view->AddGroup(ShopGroup_Basic);
    view = grid_ui->data_grid->AddView("all view");
    view->AddGroup(ShopGroup_Basic);
    TElPopupMenu *view_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetViewMenu(view_menu, FormMain->labelChangeShopView, 45, 235);
    TElPopupMenu *filter_menu = new TElPopupMenu((TComponent *)FormMain);
    grid_ui->SetFilterMenu(filter_menu, FormMain->labelChangeShopFilterType, FormMain->imageChangeShopFilterType, 145,
        235, "", "All Shops");
    grid_ui->SetEditPanel(FormMain->PanelSaveShopsHint);
    grid_ui->SetSearchEdit(FormMain->editSearchShop);
    grid_ui->SetInfoLabels(FormMain->labelShopInspectorFilter, FormMain->labelShopInspectorID,
        FormMain->labelShopInspectorName, FormMain->imageShopInspector, "");
    grid_ui->SetMessagePanel(FormMain->panelInfoHostShops, FormMain->panelInsideInfoShops);
    main_grid->IsUpdating = true;
    sub_grid->IsUpdating = true;
    grid_ui->data_grid->BuildHeader(main_grid);
    grid_ui->data_grid->AddEmptyRow(sub_grid);
    grid_ui->CreateColumnOrder();
    sub_grid->IsUpdating = false;
    has_eof_record = false;
}

// Reloads the shops from the pub folder and fills the grid again, noting a file there that would not load.
void UiTabShops::Reload()
{
    grid_ui->data_grid->has_link_out_of_bounds = false;
    grid_ui->data_grid->has_out_of_bounds = false;
    grid_ui->is_ready = false;
    shop_values->LoadFrom(FormMain->config->GetPubPath());
    file_unreadable = false;
    if (shop_values->header_read)
    {
        String file_path = shop_values->file_path;
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

// Fills the grid with a row per shop, with its trade and craft counts; hides blank names unless shown.
void UiTabShops::FillGrid(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *tag_cells;
    TElWideStrings *name_cells;
    TElWideStrings *min_level_cells;
    TElWideStrings *max_level_cells;
    TElWideStrings *class_cells;
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
    for (int i = 0; i < (unsigned int)shop_values->Count(); i++)
    {
        ShopValue *shop = shop_values->GetShop(i);
        if (LowerCase(shop->name) == "eof")
            has_eof_record = true;
        item = grid->Items->AddItem(0);
        item->Text = WideString(IntToStr(shop->id));
        tag_cells = item->ColumnText;
        tag_cells->Add(WideString(NumberText(shop->behavior_id)));
        name_cells = item->ColumnText;
        name_cells->Add(WideString(shop->name));
        min_level_cells = item->ColumnText;
        min_level_cells->Add(WideString(IntToStr(shop->min_level)));
        max_level_cells = item->ColumnText;
        max_level_cells->Add(WideString(IntToStr(shop->max_level)));
        class_cells = item->ColumnText;
        class_cells->Add(WideString(grid_ui->data_grid->CellTextOrDash("Req:Cls", shop->class_requirement)));
        item->ColumnText->Add(WideString(NumberText(shop->trades.size())));
        item->ColumnText->Add(WideString(NumberText(shop->crafts.size())));
        if (FormMain->config->GetNameHints())
            item->Hint = item->Text + " : " + item->ColumnText->Strings[name_index];
        if (!FormMain->config->GetShowRecycled() && shop->name.Length() < 2)
            item->Hidden = true;
    }
}

UiTabShops::~UiTabShops()
{
}

DEFINE_NUMBER_TEXT(UiTabShops)
