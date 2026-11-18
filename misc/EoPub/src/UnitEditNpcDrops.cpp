#include <vcl.h>
#pragma hdrstop

#include "UnitEditNpcDrops.h"
#include "UnitMain.h"
#include "UnitGfx.h"
#include "UnitWarningDelete.h"
#include "UnitSelectItem.h"
#include "Datagrid.h"
#include "Npcvalue.h"
#include "Npcvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma link "ElTreeSpinEdit"
#pragma link "ElTreeStdEditors"
#pragma resource "*.dfm"

TFormNpcEditDrops *FormNpcEditDrops;

__fastcall TFormNpcEditDrops::TFormNpcEditDrops(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_NPC_EDIT_DROPS_FORM_SHOW(TFormNpcEditDrops)

// Opens the drops of the npc at the index, titled with the npc's name, ID and graphic.
void TFormNpcEditDrops::Open(int index)
{
    pub_grid = FormMain->npcs_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    gfx_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    name_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    itemNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    itemNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    ShowNpcGraphic(StrToInt(row->ColumnText->Strings[gfx_column->text_index]));
    imageNpc->Hint =
        FormGfx->GraphicHint(StrToInt(row->ColumnText->Strings[gfx_column->text_index]), pub_grid,
            gfx_column->text_index);
    LayoutCaptions();
    ShowModal();
}

DEFINE_LAYOUT_CAPTIONS(TFormNpcEditDrops)

// Shows the row count in the list's caption, or the empty-list panel, and refreshes the rate total.
void TFormNpcEditDrops::ShowCount()
{
    ShowRateTotal();
    labelGridCaption->Caption = "Item Drops";
    panelNoAdds->Visible = false;
    if (dropGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + "x";
}

// Shows the sum of the drop rates as a percent, with a warning past 100 percent.
void TFormNpcEditDrops::ShowRateTotal()
{
    int i;
    try
    {
        if (dropGrid->Items->Count <= 0)
        {
            panelTotalPerc->Visible = false;
            return;
        }
        float total = 0;
        i = 0;
        for (; i < dropGrid->Items->Count; i++)
        {
            TElTreeItem *row = dropGrid->Items->Item[i];
            if (row->ColumnText->Strings[0].Length() < 1)
                continue;
            int rate = ParseNumber(row->ColumnText->Strings[4]);
            total = rate + total;
        }
        labelPercTotal->Caption = FormatRate(IntToStr((int)(total / 100.0)), 1000);
        // Rates are in hundredths of a percent, so a total past 100 percent is flagged.
        if (total > 10000.0)
            imageBadTotal->Visible = true;
        else
            imageBadTotal->Visible = false;
        panelTotalPerc->Visible = true;
    }
    catch (...)
    {
        panelTotalPerc->Visible = false;
    }
}

DEFINE_RENUMBER(TFormNpcEditDrops)

// Fills the list with the npc's drops: item ID and name, least and most amount, and rate.
void TFormNpcEditDrops::FillGrid()
{
    TElWideStrings *rate_cells;
    TElWideStrings *item_id_cells;
    TElWideStrings *item_name_cells;
    TElWideStrings *min_amount_cells;
    TElWideStrings *max_amount_cells;
    NpcValue *npc;
    int i;
    npc = FormMain->npcs_tab->npc_values->GetNpc(record_index);
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    for (i = 0; npc->drops.size() > i; i++)
    {
        NpcDropItem drop = npc->drops[i];
        // Rates are hundredths of a percent, shown from 0,00 % to 100,00 %.
        if (drop.rate > 10000)
            drop.rate = 10000;
        String rate_text = IntToStr(drop.rate);
        if (rate_text.Length() < 3)
        {
            if (rate_text.Length() == 1)
                rate_text = "00" + rate_text;
            if (rate_text.Length() == 2)
                rate_text = "0" + rate_text;
        }
        rate_text.Insert(",", rate_text.Length() - 1);
        rate_text = rate_text + " %";
        String item_name = "[ITEM OOB]";
        TElTreeItem *item_row = FindItemRow(IntToStr(drop.item_id));
        if (item_row)
        {
            DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            item_name = item_row->ColumnText->Strings[name_grid_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(drop.item_id)));
        item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(item_name));
        min_amount_cells = row->ColumnText;
        min_amount_cells->Add(WideString(IntToStr(drop.min_amount)));
        max_amount_cells = row->ColumnText;
        max_amount_cells->Add(WideString(IntToStr(drop.max_amount)));
        rate_cells = row->ColumnText;
        rate_cells->Add(WideString(rate_text));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

DEFINE_FIND_ITEM_ROW(TFormNpcEditDrops)

// Shows the npc's graphic from the npc library at double size, or an error caption when it is missing.
DEFINE_SHOW_NPC_GRAPHIC(TFormNpcEditDrops)

// Shows the item graphic in the image, gold for 35, at triple size when it fits, else double.
DEFINE_SHOW_ITEM_GRAPHIC(TFormNpcEditDrops)

void __fastcall TFormNpcEditDrops::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
}

void __fastcall TFormNpcEditDrops::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowItemHint(X, Y);
}

// Shows the hint with the name, type and graphic of the drop's item under the mouse.
DEFINE_SHOW_ITEM_HINT(TFormNpcEditDrops)

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormNpcEditDrops)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormNpcEditDrops)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormNpcEditDrops)

void __fastcall TFormNpcEditDrops::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then stores the filled rows as the npc's drops and their count.
void __fastcall TFormNpcEditDrops::buttonOkClick(TObject *Sender)
{
    NpcValue *npc;
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
    npc = FormMain->npcs_tab->npc_values->GetNpc(record_index);
    // The rows replace the npc's drop list; rows without an item are left out.
    npc->drops.clear();
    for (int i = 0; dropGrid->Items->Count > i; i++)
    {
        TElTreeItem *row = dropGrid->Items->Item[i];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        int item_id = StrToInt(row->ColumnText->Strings[0]);
        int min_amount = StrToInt(row->ColumnText->Strings[2]);
        int max_amount = StrToInt(row->ColumnText->Strings[3]);
        int rate = ParseNumber(row->ColumnText->Strings[4]);
        npc->AddDrop(item_id, min_amount, max_amount, rate);
    }
    DataGridCol *drop_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Drop");
    pub_grid->Selected->ColumnText->Strings[drop_column->text_index] =
        WideString(FormMain->npcs_tab->NumberText(npc->drops.size()));
    ((TCustomForm *)this)->Close();
}

// The text's number as a percent like 12,50 %, its whole part capped at the limit.
String TFormNpcEditDrops::FormatRate(String text, short limit)
{
    String whole_text = "";
    String fraction_text = "";
    int separator_count = 0;
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] == ',')
            separator_count++;
        else if (text[i] == '.')
            separator_count++;
        else if (text[i] >= '0')
        {
            if (text[i] <= '9')
            {
                if (!separator_count)
                    whole_text = whole_text + text[i];
                if (separator_count == 1)
                    fraction_text = fraction_text + text[i];
            }
        }
    }
    // The whole part is held within the limit and the fraction kept to two digits.
    int whole = 0;
    if (whole_text.Length() > 0)
        whole = StrToInt(whole_text);
    if (whole < 0)
        whole = 0;
    if (whole > limit)
        whole = limit;
    if (whole > 1000)
        whole = 999;
    whole_text = IntToStr(whole);
    if (fraction_text.Length() < 1)
        fraction_text = "00";
    if (fraction_text.Length() > 2)
        fraction_text = "99";
    if (fraction_text.Length() < 2)
        fraction_text = fraction_text + "0";
    String rate_text = whole_text + "," + fraction_text + " %";
    return rate_text;
}

// The digits of the text as a number; 0 when it has none.
int TFormNpcEditDrops::ParseNumber(String text)
{
    String digits = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] >= '0' && text[i] <= '9')
            digits = digits + text[i];
    }
    if (digits.Length() < 1)
        return 0;
    return StrToInt(digits);
}

DEFINE_DROP_GRID_ENTER(TFormNpcEditDrops)

// Formats the edited rate as a percent and refreshes the total soon after.
void __fastcall TFormNpcEditDrops::ElTreeInplaceEdit1AfterOperation(TObject *Sender, bool &Accepted,
    bool &DefaultConversion)
{
    TElTreeInplaceEdit *cast_Sender = (TElTreeInplaceEdit *)Sender;
    if (FormMain->config->GetMouseQuickSave())
        Accepted = true;
    if (Accepted)
    {
        String rate_text = FormatRate(cast_Sender->Editor->Text, 100);
        if (rate_text.Length() < 1)
        {
            Accepted = false;
            return;
        }
        cast_Sender->Editor->Text = WideString(rate_text);
        timerUpdate->Enabled = true;
    }
}

// Fills the item name for a typed ID, and keeps the least amount at or under the most.
void __fastcall TFormNpcEditDrops::ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted,
    bool &DefaultConversion)
{
    TElWideStrings *min_amount_cells;
    TElWideStrings *max_amount_cells;
    TElTreeInplaceSpinEdit *cast_Sender = (TElTreeInplaceSpinEdit *)Sender;
    if (cast_Sender->Editor->Text.Length() <= 0)
        cast_Sender->Editor->Value = 0;
    if (cast_Sender->Editor->Tag > 0)
    {
        if (cast_Sender->Editor->Tag < cast_Sender->Editor->Value)
            cast_Sender->Editor->Value = cast_Sender->Editor->Tag;
    }
    if (FormMain->config->GetMouseQuickSave())
        Accepted = true;
    if (dropGrid->Selected == NULL)
        Accepted = false;
    if (Accepted)
    {
        if (edit_column == 1)
        {
            TElTreeItem *item_row = FindItemRow((int)cast_Sender->Editor->Value);
            if (!item_row)
            {
                Accepted = false;
                return;
            }
            DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            dropGrid->Selected->ColumnText->Strings[1] = item_row->ColumnText->Strings[name_grid_column->text_index];
            hint_row = -1;
            hintPanel->Visible = false;
        }
        if (edit_column == 3)
        {
            if (StrToInt(dropGrid->Selected->ColumnText->Strings[3]) < cast_Sender->Editor->Value)
            {
                max_amount_cells = dropGrid->Selected->ColumnText;
                max_amount_cells->Strings[3] = WideString(IntToStr(cast_Sender->Editor->Value));
            }
        }
        if (edit_column == 4)
        {
            if (StrToInt(dropGrid->Selected->ColumnText->Strings[2]) > cast_Sender->Editor->Value)
            {
                min_amount_cells = dropGrid->Selected->ColumnText;
                min_amount_cells->Strings[2] = WideString(IntToStr(cast_Sender->Editor->Value));
            }
        }
    }
}

// Gives each column its editor: an ID spin, the item picker, amount spins, or the rate text.
void __fastcall TFormNpcEditDrops::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
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
            FormSelectItem->Open(this, "select : drop Item", StrToInt(dropGrid->Selected->ColumnText->Strings[0]));
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
        ElTreeInplaceSpinEdit1->Editor->MinValue = 1;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 999999999;
        ElTreeInplaceSpinEdit1->Editor->Tag = 0xF71AE3;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
    if (SectionIndex == 5)
    {
        ElTreeInplaceEdit1->Editor->MaxLength = 8;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceEdit1;
        return;
    }
}

// Clears every drop once the warning is confirmed.
void __fastcall TFormNpcEditDrops::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Npc Drops";
    String message = "The entire item drop list will be cleared, are you sure?";
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

// Adds a drop of the item picked in the item picker, one of it at a ten percent rate.
void __fastcall TFormNpcEditDrops::buttonNewNpcClick(TObject *Sender)
{
    TElTreeItem *row;
    FormSelectItem->Open(this, "add : drop Item", -1);
    if (FormSelectItem->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *item_id_cells = row->ColumnText;
        item_id_cells->Add(WideString(IntToStr(FormSelectItem->selected_id)));
        TElWideStrings *item_name_cells = row->ColumnText;
        item_name_cells->Add(WideString(FormSelectItem->selected_name));
        // A new drop starts at an amount of one and a ten percent rate.
        TElWideStrings *min_amount_cells = row->ColumnText;
        min_amount_cells->Add(WideString("1"));
        TElWideStrings *max_amount_cells = row->ColumnText;
        max_amount_cells->Add(WideString("1"));
        TElWideStrings *rate_cells = row->ColumnText;
        rate_cells->Add(WideString("10,00 %"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormNpcEditDrops)

// Delete removes the selected row and Ctrl+N adds a new one.
void __fastcall TFormNpcEditDrops::buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        popupDeleteClick((TObject *)this);
    if (Shift.Contains(ssCtrl) && Key == 'N')
        buttonNewNpcClick((TObject *)this);
}

// Refreshes the rate total once, shortly after a rate is edited.
void __fastcall TFormNpcEditDrops::timerUpdateTimer(TObject *Sender)
{
    ShowRateTotal();
    timerUpdate->Enabled = false;
}
