#include <vcl.h>
#pragma hdrstop

#include "UnitEditShopCraftsReq.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include "UnitSelectItem.h"
#include "UnitEditShopCrafts.h"
#include "Datagrid.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElTreeSpinEdit"
#pragma link "ElMenus"
#pragma resource "*.dfm"

TFormShopEditCraftsReq *FormShopEditCraftsReq;

__fastcall TFormShopEditCraftsReq::TFormShopEditCraftsReq(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_NPC_EDIT_DROPS_FORM_SHOW(TFormShopEditCraftsReq)

// Opens the ingredients of the craft row over the crafts form, titled with the craft's name and ID.
void TFormShopEditCraftsReq::Open(TFormShopEditCrafts *crafts_form, String craft_id, String craft_name,
    TElTreeItem *row)
{
    pub_grid = FormMain->shops_tab->main_grid;
    craft_row = row;
    itemNameOriginal->Caption = craft_name;
    itemNameOriginalID->Caption = "| ID: " + craft_id;
    hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = (crafts_form->Width - Width) / 2 + crafts_form->Left;
    Top = (crafts_form->Height - Height) / 2 + crafts_form->Top;
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
DEFINE_FORM_LAYOUT_CAPTIONS(TFormShopEditCraftsReq)

// Shows the ingredient count out of four in the list's caption.
void TFormShopEditCraftsReq::ShowCount()
{
    labelGridCaption->Caption = "Craft Item Requirements";
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + " / 4";
}

DEFINE_RENUMBER(TFormShopEditCraftsReq)

// Fills the list with the craft row's ingredients that have an item and an amount.
void TFormShopEditCraftsReq::FillGrid()
{
    int item_id;
    TElWideStrings *item_id_cells;
    TElWideStrings *item_name_cells;
    TElWideStrings *amount_cells;
    int amount;
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    unsigned char first_column = 3;
    unsigned char ingredient_count = 4;
    for (int i = 0; i < ingredient_count; i++)
    {
        String item_id_text(craft_row->ColumnText->Strings[first_column + 2 * i]);
        String amount_text(craft_row->ColumnText->Strings[first_column + 2 * i + 1]);
        if (item_id_text.Length() <= 0 || amount_text.Length() <= 0)
            continue;
        item_id = FormMain->NumberOr(item_id_text, 0);
        amount = FormMain->NumberOr(amount_text, 0);
        if (item_id < 1 || amount < 1)
            continue;
        String item_name = "[ITEM OOB]";
        TElTreeItem *item_row = FindItemRow(item_id_text);
        if (item_row)
        {
            DataGridCol *name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            item_name = item_row->ColumnText->Strings[name_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(item_id_text));
        item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(item_name));
        amount_cells = row->ColumnText;
        amount_cells->Add(WideString(amount_text));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

DEFINE_FIND_ITEM_ROW(TFormShopEditCraftsReq)

// Shows the item graphic in the image, gold for 35, as large as the panel and the kind allow.
DEFINE_SHOW_ICON(TFormShopEditCraftsReq)

void __fastcall TFormShopEditCraftsReq::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
}

void __fastcall TFormShopEditCraftsReq::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowItemHint(X, Y);
}

// Shows the hint with the name, type and graphic of the ingredient's item under the mouse.
void TFormShopEditCraftsReq::ShowItemHint(int x, int y)
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
    DataGridCol *name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *gender_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2");
    String item_type = "item file incomplete";
    String item_name = "[SKILL OOB]";
    String item_id(row->ColumnText->Strings[0]);
    String gfx_text = "64001";
    if (item_row)
    {
        item_type = item_row->ColumnText->Strings[type_column->text_index];
        item_name = item_row->ColumnText->Strings[name_column->text_index];
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

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormShopEditCraftsReq)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormShopEditCraftsReq)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormShopEditCraftsReq)

void __fastcall TFormShopEditCraftsReq::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then writes the ingredients and their count into the craft row.
void __fastcall TFormShopEditCraftsReq::buttonOkClick(TObject *Sender)
{
    unsigned char i;
    TElWideStrings *count_cells;
    TElWideStrings *ingredient_id_cells;
    TElWideStrings *ingredient_amount_cells;
    if (((TCustomElTree *)dropGrid)->IsEditing())
        ((TCustomElTree *)dropGrid)->EndEdit(false);
    if (FormMain->config->GetDeleteWarning() && rows_deleted)
    {
        String title = "Delete Notice";
        String message = "One(1) or more rows have been deleted in this session, please confirm.";
        String button_text = "UPDATE";
        FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text);
        if (FormDeleteWarning->ShowModal() != mrOk)
            return;
    }
    unsigned char first_column = 3;
    unsigned char ingredient_count = 4;
    for (i = 0; (int)dropGrid->Items->Count > i; i++)
    {
        craft_row->ColumnText->Strings[first_column + 2 * i] = dropGrid->Items->Item[i]->ColumnText->Strings[0];
        craft_row->ColumnText->Strings[first_column + 2 * i + 1] = dropGrid->Items->Item[i]->ColumnText->Strings[2];
    }
    // Slots past the last row are cleared to zero.
    for (; i < ingredient_count; i++)
    {
        ingredient_id_cells = craft_row->ColumnText;
        ingredient_id_cells->Strings[first_column + 2 * i] = WideString("0");
        ingredient_amount_cells = craft_row->ColumnText;
        ingredient_amount_cells->Strings[first_column + 2 * i + 1] = WideString("0");
    }
    count_cells = craft_row->ColumnText;
    count_cells->Strings[2] = WideString(IntToStr(dropGrid->Items->Count));
    ModalResult = mrOk;
}

DEFINE_DROP_GRID_ENTER(TFormShopEditCraftsReq)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormShopEditCraftsReq)

// Delete removes the selected row and Ctrl+N adds a new one.
DEFINE_DROP_GRID_KEY_UP(TFormShopEditCraftsReq)

// Clears every ingredient once the warning is confirmed.
DEFINE_BUTTON_CLEAN_LIST_CLICK(TFormShopEditCraftsReq)

// Adds an ingredient of the item picked in the item picker; four at most.
void __fastcall TFormShopEditCraftsReq::buttonNewShopCraftClick(TObject *Sender)
{
    TElTreeItem *row;
    // The pub file holds four ingredients per craft.
    if (dropGrid->Items->Count >= 4)
    {
        FormWarning->ShowWarning(this, "Full - Cannot add",
            "The pub file can only store a maximum of 4 craft ingredients (items)");
        return;
    }
    FormSelectItem->Open(this, "add : craft Item", -1);
    if (FormSelectItem->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *cells = row->ColumnText;
        cells->Add(WideString(IntToStr(FormSelectItem->selected_id)));
        cells = row->ColumnText;
        cells->Add(WideString(FormSelectItem->selected_name));
        cells = row->ColumnText;
        cells->Add(WideString("1"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Gives each column its editor: an ID spin, the item picker, or the amount spin.
void __fastcall TFormShopEditCraftsReq::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item,
    int SectionIndex, TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
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
                hintPanel->Visible = false;
            }
        }
        return;
    }
    if (SectionIndex == 3)
    {
        ElTreeInplaceSpinEdit1->Editor->MinValue = 1;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 64008;
        ElTreeInplaceSpinEdit1->Editor->Tag = 999999;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
}

// Fills the item name for a typed ID; refuses an ID with no item.
DEFINE_EL_TREE_INPLACE_SPIN_EDIT1_AFTER_OPERATION(TFormShopEditCraftsReq)

// Blanks the zero cells of the changed row when zeros are hidden.
DEFINE_DROP_GRID_ITEM_CHANGE(TFormShopEditCraftsReq)
