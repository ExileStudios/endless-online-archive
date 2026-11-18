#include <vcl.h>
#pragma hdrstop

#include "UnitEditShopCrafts.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include "UnitSelectItem.h"
#include "UnitEditShopCraftsReq.h"
#include "Datagrid.h"
#include "Shopvalue.h"
#include "Shopvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma link "ElTreeSpinEdit"
#pragma resource "*.dfm"

TFormShopEditCrafts *FormShopEditCrafts;

__fastcall TFormShopEditCrafts::TFormShopEditCrafts(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the theme and its shop picture, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_SHOP_EDIT_ITEMS_FORM_SHOW(TFormShopEditCrafts)

// Opens the crafts of the shop at the index, titled with the shop's name and ID.
void TFormShopEditCrafts::Open(int index)
{
    pub_grid = FormMain->shops_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    name_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    itemNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    itemNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    hint_row = -1;
    ingredient_hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    LayoutCaptions();
    ShowModal();
}

DEFINE_LAYOUT_CAPTIONS(TFormShopEditCrafts)

// Shows the row count in the list's caption, or the empty-list panel when there are none.
void TFormShopEditCrafts::ShowCount()
{
    labelGridCaption->Caption = "Shop Craft Items";
    panelNoAdds->Visible = false;
    if (dropGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + "x";
}

// Numbers the rows by their position.
DEFINE_RENUMBER(TFormShopEditCrafts)

// Fills the list with the shop's crafts: item ID and name, ingredient count, and the four ingredients.
void TFormShopEditCrafts::FillGrid()
{
    TElWideStrings *fourth_amount_cells;
    TElWideStrings *item_id_cells;
    TElWideStrings *item_name_cells;
    TElWideStrings *ingredient_count_cells;
    TElWideStrings *first_ingredient_cells;
    TElWideStrings *first_amount_cells;
    TElWideStrings *second_ingredient_cells;
    TElWideStrings *second_amount_cells;
    TElWideStrings *third_ingredient_cells;
    TElWideStrings *third_amount_cells;
    TElWideStrings *fourth_ingredient_cells;
    ShopValue *shop;
    int i;
    shop = FormMain->shops_tab->shop_values->GetShop(record_index);
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    for (i = 0; shop->crafts.size() > i; i++)
    {
        ShopCraftVal craft = shop->crafts[i];
        String item_name = "[ITEM OOB]";
        TElTreeItem *item_row = FindItemRow(IntToStr(craft.id));
        if (item_row)
        {
            DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            item_name = item_row->ColumnText->Strings[name_grid_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(craft.id)));
        item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(item_name));
        ingredient_count_cells = row->ColumnText;
        ingredient_count_cells->Add(WideString(IngredientCount(&craft)));
        first_ingredient_cells = row->ColumnText;
        first_ingredient_cells->Add(WideString(IntToStr(craft.ingredient_item_ids[0])));
        first_amount_cells = row->ColumnText;
        first_amount_cells->Add(WideString(IntToStr(craft.ingredient_amounts[0])));
        second_ingredient_cells = row->ColumnText;
        second_ingredient_cells->Add(WideString(IntToStr(craft.ingredient_item_ids[1])));
        second_amount_cells = row->ColumnText;
        second_amount_cells->Add(WideString(IntToStr(craft.ingredient_amounts[1])));
        third_ingredient_cells = row->ColumnText;
        third_ingredient_cells->Add(WideString(IntToStr(craft.ingredient_item_ids[2])));
        third_amount_cells = row->ColumnText;
        third_amount_cells->Add(WideString(IntToStr(craft.ingredient_amounts[2])));
        fourth_ingredient_cells = row->ColumnText;
        fourth_ingredient_cells->Add(WideString(IntToStr(craft.ingredient_item_ids[3])));
        fourth_amount_cells = row->ColumnText;
        fourth_amount_cells->Add(WideString(IntToStr(craft.ingredient_amounts[3])));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

// How many ingredient slots, from the first, have both an item and an amount.
String TFormShopEditCrafts::IngredientCount(ShopCraftVal *craft)
{
    int count = 0;
    if (!craft->ingredient_item_ids[0] || !craft->ingredient_amounts[0])
        return IntToStr(count);
    count++;
    if (!craft->ingredient_item_ids[1] || !craft->ingredient_amounts[1])
        return IntToStr(count);
    count++;
    if (!craft->ingredient_item_ids[2] || !craft->ingredient_amounts[2])
        return IntToStr(count);
    count++;
    if (!craft->ingredient_item_ids[3] || !craft->ingredient_amounts[3])
        return IntToStr(count);
    count++;
    return IntToStr(count);
}

DEFINE_FIND_ITEM_ROW(TFormShopEditCrafts)

// Shows the item graphic in the image, gold for 35, as large as the panel and the kind allow.
DEFINE_SHOW_ICON(TFormShopEditCrafts)

// Hides the hints once the mouse is off the list.
void __fastcall TFormShopEditCrafts::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
    hintPanelReq1->Visible = false;
}

void __fastcall TFormShopEditCrafts::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowItemHint(X, Y);
    ShowIngredientHint(X, Y);
}

// Shows the hint with the name, type and graphic of the craft's item under the mouse.
void TFormShopEditCrafts::ShowItemHint(int x, int y)
{
    int hint_right;
    int row_index;
    if (hint_row == -2)
    {
        hint_row = -1;
        return;
    }
    int hint_left = dropGrid->HeaderSections->Item[0]->Width;
    hint_right = dropGrid->HeaderSections->Item[0]->Width;
    hint_right += dropGrid->HeaderSections->Item[1]->Width;
    hint_right += dropGrid->HeaderSections->Item[2]->Width;
    int row_count = dropGrid->Items->Count;
    if (x < hint_left)
    {
        hintPanel->Visible = false;
        return;
    }
    if (hint_right < x)
    {
        hintPanel->Visible = false;
        return;
    }
    if (y < 0)
    {
        hintPanel->Visible = false;
        return;
    }
    if (24 * row_count < y)
    {
        hintPanel->Visible = false;
        return;
    }
    int hover_column = 0;
    row_index = y / dropGrid->DefaultRowHeight;
    int visible_row = row_index;
    row_index += dropGrid->TopIndex;
    if (row_index >= row_count)
    {
        hintPanel->Visible = false;
        return;
    }
    // Still over the same row: the hint already shows it.
    if (row_index == hint_row)
    {
        hintPanel->Visible = true;
        return;
    }
    hint_row = row_index;
    TElTreeItem *row = dropGrid->Items->Item[row_index];
    TElTreeItem *item_row = FindItemRow(row->ColumnText->Strings[0]);
    if (dropGrid->HeaderSections->Item[0]->Width < x)
        hover_column = 1;
    if (dropGrid->HeaderSections->Item[0]->Width + dropGrid->HeaderSections->Item[1]->Width < x)
        hover_column = 2;
    int hint_x = Panel1->Left;
    int hint_y = Panel1->Top;
    hint_x += 20;
    if (hover_column == 1)
        hint_x += dropGrid->HeaderSections->Item[0]->Width;
    if (hover_column == 2)
    {
        hint_x += dropGrid->HeaderSections->Item[0]->Width;
        hint_x += dropGrid->HeaderSections->Item[1]->Width;
    }
    hint_y += dropGrid->Top + dropGrid->DefaultRowHeight + 4;
    hint_y += dropGrid->DefaultRowHeight * visible_row;
    DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *gender_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2");
    String item_type = "item file incomplete";
    String item_name = "[ITEM OOB]";
    String item_id(row->ColumnText->Strings[0]);
    String gfx_text = "64001";
    if (item_row)
    {
        item_type = item_row->ColumnText->Strings[type_column->text_index];
        item_name = item_row->ColumnText->Strings[name_grid_column->text_index];
        item_id = item_row->Text;
        if (item_row->ColumnText->Strings[gfx_column->text_index].Length() > 0)
            gfx_text = item_row->ColumnText->Strings[gfx_column->text_index];
    }
    hintCaption->Caption = item_name;
    hintIDCaption->Caption = "| ID: " + item_id;
    hintSubString->Caption = item_type;
    hintIDCaption->Left = hintCaption->Left + hintCaption->Width + 8;
    hintPanel->Width = 337;
    int needed_width = hintIDCaption->Left + hintIDCaption->Width + 24;
    if (needed_width > hintPanel->Width)
        hintPanel->Width = needed_width;
    if (item_row)
    {
        if (LowerCase(item_type).Pos("armor") > 0)
        {
            String gender_text(item_row->ColumnText->Strings[gender_column->text_index]);
            if (gender_text == "1")
                hintSubString->Caption = item_type + " (m)";
            else
                hintSubString->Caption = item_type + " (f)";
        }
    }
    ShowIcon(hintImage, hintImagePanel, 3, StrToInt(gfx_text));
    hintPanel->Left = hint_x;
    hintPanel->Top = hint_y;
    hintPanel->Visible = true;
}

// Over the ingredients column, shows a hint listing each ingredient's name, type, graphic and amount.
void TFormShopEditCrafts::ShowIngredientHint(int x, int y)
{
    bool loop_entered = false;
    int row_index;
    int amount;
    int hint_left =
        dropGrid->HeaderSections->Item[0]->Width + dropGrid->HeaderSections->Item[1]->Width +
            dropGrid->HeaderSections->Item[2]->Width + 1;
    int hint_right = hint_left;
    hint_right += dropGrid->HeaderSections->Item[3]->Width;
    int row_count = dropGrid->Items->Count;
    if (hint_left > x)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (x > hint_right)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (y < 0)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (24 * row_count < y)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    int hover_column[1];
    hover_column[0] = 0;
    row_index = y / dropGrid->DefaultRowHeight;
    int visible_row = row_index;
    row_index += dropGrid->TopIndex;
    if (row_index >= row_count)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (row_index == ingredient_hint_row)
    {
        hintPanelReq1->Visible = true;
        return;
    }
    ingredient_hint_row = row_index;
    TElTreeItem *row = dropGrid->Items->Item[row_index];
    TElTreeItem *item_row = FindItemRow(row->ColumnText->Strings[0]);
    int hint_x = Panel1->Left;
    int hint_y = Panel1->Top;
    hint_y += dropGrid->Top + dropGrid->DefaultRowHeight + 4;
    hint_y += dropGrid->DefaultRowHeight * visible_row;
    DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *gender_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2");
    labelCaptionIDReq1->Caption = "| ID: " + row->ColumnText->Strings[0];
    labelCaptionReq1->Caption = row->ColumnText->Strings[1];
    labelCaptionIDReq1->Left = labelCaptionReq1->Left + labelCaptionReq1->Width + 6;
    panelReq1->Visible = false;
    panelReq2->Visible = false;
    panelReq3->Visible = false;
    panelReq4->Visible = false;
    int shown_count = 0;
    int widest = 300;
    // One panel per ingredient slot that has both an item and an amount.
    for (int i = 0; i < 4; i++)
    {
        loop_entered = true;
        int first_column = 3;
        String ingredient_id_text(row->ColumnText->Strings[2 * i + first_column]);
        String amount_text(row->ColumnText->Strings[2 * i + first_column + 1]);
        if (!ingredient_id_text.Length())
            continue;
        if (!amount_text.Length())
            continue;
        hint_left = StrToInt(ingredient_id_text);
        amount = StrToInt(amount_text);
        if (!hint_left)
            continue;
        if (!amount)
            continue;
        String item_type = "item file incomplete";
        String item_name = "[ITEM OOB]";
        String gfx_text = "64001";
        TElTreeItem *ingredient_row = FindItemRow(hint_left);
        if (ingredient_row)
        {
            item_type = ingredient_row->ColumnText->Strings[type_column->text_index];
            item_name = ingredient_row->ColumnText->Strings[name_grid_column->text_index];
            ingredient_id_text = ingredient_row->Text;
            if (ingredient_row->ColumnText->Strings[gfx_column->text_index].Length() > 0)
                gfx_text = ingredient_row->ColumnText->Strings[gfx_column->text_index];
        }
        if (!shown_count)
        {
            panelReq1->Visible = true;
            hintCaptionReq1->Caption = item_name;
            hintIDCaptionReq1->Caption = "| ID: " + ingredient_id_text;
            hintSubStringReq1->Caption = item_type;
            hintCraftCount1->Caption = amount_text + "x";
            hintIDCaptionReq1->Left = hintCaptionReq1->Left + hintCaptionReq1->Width + 6;
            ShowIcon(hintImageReq1, hintImagePanelReq1, 1, StrToInt(gfx_text));
            int first_caption_right = hintIDCaptionReq1->Left + hintIDCaptionReq1->Width;
            if (first_caption_right > widest)
                widest = first_caption_right;
        }
        if (shown_count == 1)
        {
            panelReq2->Visible = true;
            hintCaptionReq2->Caption = item_name;
            hintIDCaptionReq2->Caption = "| ID: " + ingredient_id_text;
            hintSubStringReq2->Caption = item_type;
            hintCraftCount2->Caption = amount_text + "x";
            hintIDCaptionReq2->Left = hintCaptionReq2->Left + hintCaptionReq2->Width + 6;
            ShowIcon(hintImageReq2, hintImagePanelReq2, 1, StrToInt(gfx_text));
            int second_caption_right = hintIDCaptionReq2->Left + hintIDCaptionReq2->Width;
            if (second_caption_right > widest)
                widest = second_caption_right;
        }
        if (shown_count == 2)
        {
            panelReq3->Visible = true;
            hintCaptionReq3->Caption = item_name;
            hintIDCaptionReq3->Caption = "| ID: " + ingredient_id_text;
            hintSubStringReq3->Caption = item_type;
            hintCraftCount3->Caption = amount_text + "x";
            hintIDCaptionReq3->Left = hintCaptionReq3->Left + hintCaptionReq3->Width + 6;
            ShowIcon(hintImageReq3, hintImagePanelReq3, 1, StrToInt(gfx_text));
            int third_caption_right = hintIDCaptionReq3->Left + hintIDCaptionReq3->Width;
            if (third_caption_right > widest)
                widest = third_caption_right;
        }
        if (shown_count == 3)
        {
            panelReq4->Visible = true;
            hintCaptionReq4->Caption = item_name;
            hintIDCaptionReq4->Caption = "| ID: " + ingredient_id_text;
            hintSubStringReq4->Caption = item_type;
            hintCraftCount4->Caption = amount_text + "x";
            hintIDCaptionReq4->Left = hintCaptionReq4->Left + hintCaptionReq4->Width + 6;
            ShowIcon(hintImageReq4, hintImagePanelReq4, 1, StrToInt(gfx_text));
            int fourth_caption_right = hintIDCaptionReq4->Left + hintIDCaptionReq4->Width;
            if (fourth_caption_right > widest)
                widest = fourth_caption_right;
        }
        shown_count++;
    }
    hintPanelReq1->Width = widest + 36;
    panelReq1->Width = hintPanelReq1->Width - panelReq1->Left - 2;
    panelReq2->Width = hintPanelReq1->Width - panelReq2->Left - 2;
    panelReq3->Width = hintPanelReq1->Width - panelReq3->Left - 2;
    panelReq4->Width = hintPanelReq1->Width - panelReq4->Left - 2;
    hintPanelReq1->Height = 44;
    if (panelReq1->Visible)
        hintPanelReq1->Height = panelReq1->Top + panelReq1->Height + 10;
    if (panelReq2->Visible)
        hintPanelReq1->Height = panelReq2->Top + panelReq2->Height + 10;
    if (panelReq3->Visible)
        hintPanelReq1->Height = panelReq3->Top + panelReq3->Height + 10;
    if (panelReq4->Visible)
        hintPanelReq1->Height = panelReq4->Top + panelReq4->Height + 10;
    hintReq1AddHint->Visible = false;
    if (shown_count <= 0)
    {
        hintReq1AddHint->Visible = true;
        hintPanelReq1->Height = hintPanelReq1->Height + 32;
    }
    hintPanelReq1->Visible = false;
    hintPanelReq1->Left = hint_x + hint_right - hintPanelReq1->Width - 20;
    hintPanelReq1->Top = hint_y;
    if (hintPanelReq1->Top > 320)
    {
        hintPanelReq1->Top = hintPanelReq1->Top - hintPanelReq1->Height;
        hintPanelReq1->Top = hintPanelReq1->Top - 2 * dropGrid->DefaultRowHeight;
    }
    hintPanelReq1->Visible = true;
}

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormShopEditCrafts)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormShopEditCrafts)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormShopEditCrafts)

void __fastcall TFormShopEditCrafts::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses crafts without ingredients, then stores the rows as the shop's crafts and their count.
void __fastcall TFormShopEditCrafts::buttonOkClick(TObject *Sender)
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
        if (dropGrid->Items->Item[i]->ColumnText->Strings[2].Length() < 1)
        {
            FormWarning->ShowWarning(this, "Cannot Save",
                "Some craftable item(s) do not have any requirements added, please add one ore more ingredients");
            return;
        }
        if (String(dropGrid->Items->Item[i]->ColumnText->Strings[2]) == "0")
        {
            FormWarning->ShowWarning(this, "Cannot Save",
                "Some craftable item(s) do not have REQS, please add one ore more ingredients");
            return;
        }
    }
    ShopValue *shop = FormMain->shops_tab->shop_values->GetShop(record_index);
    // The rows replace the shop's craft list; rows without an item are left out.
    shop->crafts.clear();
    for (int j = 0; dropGrid->Items->Count > j; j++)
    {
        TElTreeItem *row = dropGrid->Items->Item[j];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        int item_id = StrToInt(row->ColumnText->Strings[0]);
        int column = 3;
        int first_ingredient_id = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int first_amount = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int second_ingredient_id = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int second_amount = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int third_ingredient_id = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int third_amount = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int fourth_ingredient_id = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int fourth_amount = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        shop->AddCraft(item_id, first_ingredient_id, first_amount, second_ingredient_id, second_amount,
            third_ingredient_id, third_amount, fourth_ingredient_id, fourth_amount);
    }
    DataGridCol *craft_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Craft");
    pub_grid->Selected->ColumnText->Strings[craft_column->text_index] =
        WideString(FormMain->shops_tab->NumberText(shop->crafts.size()));
    ((TCustomForm *)this)->Close();
}

DEFINE_DROP_GRID_ENTER(TFormShopEditCrafts)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormShopEditCrafts)

// Delete removes the selected row and Ctrl+N adds a new one.
DEFINE_DROP_GRID_KEY_UP(TFormShopEditCrafts)

// Clears every craft once the warning is confirmed.
DEFINE_BUTTON_CLEAN_LIST_CLICK(TFormShopEditCrafts)

// Adds a craft of the item picked in the item picker, with no ingredients yet.
void __fastcall TFormShopEditCrafts::buttonNewShopCraftClick(TObject *Sender)
{
    TElTreeItem *row;
    FormSelectItem->Open(this, "add : craft Item", -1);
    if (FormSelectItem->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(FormSelectItem->selected_id)));
        TElWideStrings *item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(FormSelectItem->selected_name));
        TElWideStrings *ingredient_count_cells = row->ColumnText;
        ingredient_count_cells->Add(WideString(""));
        TElWideStrings *first_ingredient_cells = row->ColumnText;
        first_ingredient_cells->Add(WideString("0"));
        TElWideStrings *first_amount_cells = row->ColumnText;
        first_amount_cells->Add(WideString("0"));
        TElWideStrings *second_ingredient_cells = row->ColumnText;
        second_ingredient_cells->Add(WideString("0"));
        TElWideStrings *second_amount_cells = row->ColumnText;
        second_amount_cells->Add(WideString("0"));
        TElWideStrings *third_ingredient_cells = row->ColumnText;
        third_ingredient_cells->Add(WideString("0"));
        TElWideStrings *third_amount_cells = row->ColumnText;
        third_amount_cells->Add(WideString("0"));
        TElWideStrings *fourth_ingredient_cells = row->ColumnText;
        fourth_ingredient_cells->Add(WideString("0"));
        TElWideStrings *fourth_amount_cells = row->ColumnText;
        fourth_amount_cells->Add(WideString("0"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Gives each column its editor: an ID spin, the item picker, or the ingredients form.
void __fastcall TFormShopEditCrafts::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
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
            FormSelectItem->Open(this, "select : craft Item", StrToInt(dropGrid->Selected->ColumnText->Strings[0]));
            if (FormSelectItem->ModalResult == mrOk)
            {
                TElWideStrings *item_id_cells = dropGrid->Selected->ColumnText;
                item_id_cells->Strings[0] = WideString(IntToStr(FormSelectItem->selected_id));
                TElWideStrings *item_name_cells = dropGrid->Selected->ColumnText;
                item_name_cells->Strings[1] = WideString(FormSelectItem->selected_name);
                hint_row = -1;
                ingredient_hint_row = -1;
                hintPanel->Visible = false;
            }
        }
        return;
    }
    if (SectionIndex == 3)
    {
        Editor = 0;
        if (dropGrid->Selected->ColumnText->Strings[0].Length() > 0)
        {
            FormShopEditCraftsReq->Open(this, dropGrid->Selected->ColumnText->Strings[0],
                itemNameOriginal->Caption + " : " + dropGrid->Selected->ColumnText->Strings[1], Item);
            if (FormShopEditCraftsReq->ModalResult == mrOk)
            {
                hint_row = -1;
                ingredient_hint_row = -1;
                hintPanel->Visible = false;
                hintPanelReq1->Visible = false;
            }
        }
        return;
    }
}

// Fills the item name for a typed ID; refuses an ID with no item.
void __fastcall TFormShopEditCrafts::ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted,
    bool &DefaultConversion)
{
    if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag > 0)
    {
        if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag < ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value)
            ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value = ((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag;
    }
    if (FormMain->config->GetMouseQuickSave())
        Accepted = true;
    if (dropGrid->Selected == NULL)
        Accepted = false;
    if (Accepted && edit_column == 1)
    {
        TElTreeItem *item_row = FindItemRow((int)((TElTreeInplaceSpinEdit *)Sender)->Editor->Value);
        if (!item_row)
        {
            Accepted = false;
            return;
        }
        DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
        dropGrid->Selected->ColumnText->Strings[1] = item_row->ColumnText->Strings[name_grid_column->text_index];
        hint_row = -1;
        ingredient_hint_row = -1;
        hintPanel->Visible = false;
    }
}

// Blanks the zero cells of the changed row when zeros are hidden.
DEFINE_DROP_GRID_ITEM_CHANGE(TFormShopEditCrafts)
