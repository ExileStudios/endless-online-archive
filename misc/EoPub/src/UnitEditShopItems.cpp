#include <vcl.h>
#pragma hdrstop

#include "UnitEditShopItems.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include "UnitSelectItem.h"
#include "Datagrid.h"
#include "Shopvalue.h"
#include "Shopvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma link "ElTreeSpinEdit"
#pragma resource "*.dfm"

TFormShopEditItems *FormShopEditItems;

__fastcall TFormShopEditItems::TFormShopEditItems(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the theme and its shop picture, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_SHOP_EDIT_ITEMS_FORM_SHOW(TFormShopEditItems)

// Opens the trade items of the shop at the index, titled with the shop's name and ID.
void TFormShopEditItems::Open(int index)
{
    pub_grid = FormMain->shops_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    name_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    itemNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    itemNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
DEFINE_LAYOUT_CAPTIONS(TFormShopEditItems)

// Shows the row count in the list's caption, or the empty-list panel when there are none.
void TFormShopEditItems::ShowCount()
{
    labelGridCaption->Caption = "Shop Trade Items";
    panelNoAdds->Visible = false;
    if (dropGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + "x";
}

DEFINE_RENUMBER(TFormShopEditItems)

// Fills the list with the shop's trade items: item ID and name, buy and sell price, and most amount.
void TFormShopEditItems::FillGrid()
{
    TElWideStrings *max_amount_cells;
    TElWideStrings *item_id_cells;
    TElWideStrings *item_name_cells;
    TElWideStrings *buy_price_cells;
    TElWideStrings *sell_price_cells;
    ShopValue *shop;
    int i;
    shop = FormMain->shops_tab->shop_values->GetShop(record_index);
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    for (i = 0; shop->trades.size() > i; i++)
    {
        ShopItemVal trade_item = shop->trades[i];
        String item_name = "[ITEM OOB]";
        TElTreeItem *item_row = FindItemRow(IntToStr(trade_item.item_id));
        if (item_row)
        {
            DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            item_name = item_row->ColumnText->Strings[name_grid_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(trade_item.item_id)));
        item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(item_name));
        buy_price_cells = row->ColumnText;
        buy_price_cells->Add(WideString(FormMain->shops_tab->NumberText(trade_item.buy_price)));
        sell_price_cells = row->ColumnText;
        sell_price_cells->Add(WideString(FormMain->shops_tab->NumberText(trade_item.sell_price)));
        max_amount_cells = row->ColumnText;
        max_amount_cells->Add(WideString(FormMain->shops_tab->NumberText(trade_item.max_amount)));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

DEFINE_FIND_ITEM_ROW(TFormShopEditItems)

// Shows the item graphic in the image, gold for 35, at triple size when it fits, else double.
DEFINE_SHOW_ITEM_GRAPHIC(TFormShopEditItems)

// Hides the hints once the mouse is off the list.
void __fastcall TFormShopEditItems::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
}

void __fastcall TFormShopEditItems::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowItemHint(X, Y);
}

// Shows the hint with the name, type and graphic of the trade row's item under the mouse.
DEFINE_SHOW_ITEM_HINT(TFormShopEditItems)

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormShopEditItems)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormShopEditItems)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormShopEditItems)

void __fastcall TFormShopEditItems::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses rows without a buy or sell price, then stores the rows as the shop's trade items.
void __fastcall TFormShopEditItems::buttonOkClick(TObject *Sender)
{
    if (((TCustomElTree *)dropGrid)->IsEditing())
        ((TCustomElTree *)dropGrid)->EndEdit(false);
    if (FormMain->config->GetDeleteWarning() && rows_deleted)
    {
        String title = "Delete Notice";
        String message = "One(1) or more rows have been deleted in this session, please confirm.";
        String button_text = "SAVE |  ID: " + IntToStr(record_index + 1);
        FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text);
        if (FormDeleteWarning->ShowModal() != mrOk)
            return;
    }
    for (int i = 0; dropGrid->Items->Count > i; i++)
    {
        if (dropGrid->Items->Item[i]->ColumnText->Strings[2].Length() < 1 &&
            dropGrid->Items->Item[i]->ColumnText->Strings[3].Length() < 1)
        {
            FormWarning->ShowWarning(this, "Cannot Save",
                "One or more item(s) do not have both [buy] and [sell] price.");
            return;
        }
        if (String(dropGrid->Items->Item[i]->ColumnText->Strings[2]) == "0" &&
            String(dropGrid->Items->Item[i]->ColumnText->Strings[3]) == "0")
        {
            FormWarning->ShowWarning(this, "Cannot Save",
                "One or more item(s) do not have both [buy] and [sell] price.");
            return;
        }
    }
    ShopValue *shop = FormMain->shops_tab->shop_values->GetShop(record_index);
    // The rows replace the shop's trade list; rows without an item are left out.
    shop->trades.clear();
    for (int j = 0; dropGrid->Items->Count > j; j++)
    {
        TElTreeItem *row = dropGrid->Items->Item[j];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        int item_id = StrToInt(row->ColumnText->Strings[0]);
        int buy_price = FormMain->shops_tab->grid_ui->DigitsOnly(row->ColumnText->Strings[2]);
        int sell_price = FormMain->shops_tab->grid_ui->DigitsOnly(row->ColumnText->Strings[3]);
        int max_amount = FormMain->shops_tab->grid_ui->DigitsOnly(row->ColumnText->Strings[4]);
        shop->AddTrade(item_id, sell_price, buy_price, max_amount);
    }
    DataGridCol *trade_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Trade");
    pub_grid->Selected->ColumnText->Strings[trade_column->text_index] =
        WideString(FormMain->shops_tab->NumberText(shop->trades.size()));
    ((TCustomForm *)this)->Close();
}

DEFINE_DROP_GRID_ENTER(TFormShopEditItems)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormShopEditItems)

// Delete removes the selected row and Ctrl+N adds a new one.
void __fastcall TFormShopEditItems::dropGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        popupDeleteClick((TObject *)this);
    if (Shift.Contains(ssCtrl) && Key == 'N')
        buttonNewShopItemClick((TObject *)this);
}

// Clears every trade item once the warning is confirmed.
void __fastcall TFormShopEditItems::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Shop Items";
    String message = "The entire shop list will be cleared, are you sure?";
    String button_text = "CLEAR LIST";
    FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text);
    if (FormDeleteWarning->ShowModal() == mrOk)
    {
        dropGrid->IsUpdating = true;
        dropGrid->Items->Clear();
        dropGrid->IsUpdating = false;
    }
    ShowCount();
}

// Adds a trade row of the item picked in the item picker, priced 100 to buy, at most 4.
void __fastcall TFormShopEditItems::buttonNewShopItemClick(TObject *Sender)
{
    TElTreeItem *row;
    FormSelectItem->Open(this, "add : shop Item", -1);
    if (FormSelectItem->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(FormSelectItem->selected_id)));
        TElWideStrings *item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(FormSelectItem->selected_name));
        TElWideStrings *buy_price_cells = row->ColumnText;
        buy_price_cells->Add(WideString("100"));
        TElWideStrings *sell_price_cells = row->ColumnText;
        sell_price_cells->Add(WideString(""));
        TElWideStrings *max_amount_cells = row->ColumnText;
        max_amount_cells->Add(WideString("4"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Gives each column its editor: an ID spin, the item picker, price spins, or the amount spin.
void __fastcall TFormShopEditItems::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    edit_column = SectionIndex;
    if (SectionIndex == 1)
    {
        ElTreeInplaceSpinEdit1->Editor->MinValue = 1;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 64000;
        ElTreeInplaceSpinEdit1->Editor->Tag = 0;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
    if (SectionIndex == 2)
    {
        Editor = 0;
        if (dropGrid->Selected->ColumnText->Strings[0].Length() > 0)
        {
            FormSelectItem->Open(this, "select : shop Item", StrToInt(dropGrid->Selected->ColumnText->Strings[0]));
            if (FormSelectItem->ModalResult == mrOk)
            {
                TElWideStrings *item_id_cells = dropGrid->Selected->ColumnText;
                item_id_cells->Strings[0] = WideString(IntToStr(FormSelectItem->selected_id));
                TElWideStrings *item_name_cells = dropGrid->Selected->ColumnText;
                item_name_cells->Strings[1] = WideString(FormSelectItem->selected_name);
                hint_row = -1;
                hintPanel->Visible = false;
            }
        }
        return;
    }
    if (SectionIndex == 3 || SectionIndex == 4)
    {
        ElTreeInplaceSpinEdit1->Editor->MinValue = 0;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 999999999;
        ElTreeInplaceSpinEdit1->Editor->Tag = 0xF71AE3;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
    if (SectionIndex == 5)
    {
        ElTreeInplaceSpinEdit1->Editor->MinValue = 1;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 9999;
        ElTreeInplaceSpinEdit1->Editor->Tag = 252;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
}

// Fills the item name for a typed ID; refuses an ID with no item.
DEFINE_EL_TREE_INPLACE_SPIN_EDIT1_AFTER_OPERATION(TFormShopEditItems)

// Blanks the zero cells of the changed row when zeros are hidden.
DEFINE_DROP_GRID_ITEM_CHANGE(TFormShopEditItems)
