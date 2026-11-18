#include <vcl.h>
#pragma hdrstop

#include "UnitEditNpcChats.h"
#include "UnitMain.h"
#include "UnitGfx.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include "Datagrid.h"
#include "Npcvalue.h"
#include "Npcvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeStdEditors"
#pragma link "ElSpin"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma resource "*.dfm"

TFormNpcEditChats *FormNpcEditChats;

__fastcall TFormNpcEditChats::TFormNpcEditChats(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first line.
DEFINE_T_FORM_NPC_EDIT_CHATS_FORM_SHOW(TFormNpcEditChats)

// Opens the chat lines of the npc at the index, titled with the npc's name, ID and graphic.
void TFormNpcEditChats::Open(int index)
{
    pub_grid = FormMain->npcs_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    gfx_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    name_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    itemNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    itemNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    Panel2->Visible = true;
    rows_deleted = false;
    FillGrid();
    ShowCount();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    ShowNpcGraphic(StrToInt(row->ColumnText->Strings[gfx_column->text_index]));
    imageNpc->Hint =
        FormGfx->GraphicHint(StrToInt(row->ColumnText->Strings[gfx_column->text_index]), pub_grid,
            gfx_column->text_index);
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
DEFINE_LAYOUT_CAPTIONS(TFormNpcEditChats)

// Numbers the lines by their position.
DEFINE_FORM_RENUMBER(TFormNpcEditChats)

// Loads the npc's chat interval and chance, 5 and 100 when unset, and fills the list with its lines.
void TFormNpcEditChats::FillGrid()
{
    TElWideStrings *chat_cells;
    NpcValue *npc = FormMain->npcs_tab->npc_values->GetNpc(record_index);
    spinInterval->Value = npc->talk_interval;
    spinChance->Value = npc->talk_rate;
    chat_interval = 5;
    chat_chance = 100;
    if (npc->talk_interval > 0)
        chat_interval = npc->talk_interval;
    if (npc->talk_rate > 0)
        chat_chance = npc->talk_rate;
    lineGrid->IsUpdating = true;
    lineGrid->Items->Clear();
    for (int i = 0; i < npc->talk_lines.size(); i++)
    {
        TElTreeItem *row = lineGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        chat_cells = row->ColumnText;
        chat_cells->Add(WideString(npc->talk_lines[i]));
    }
    lineGrid->IsUpdating = false;
    ShowCount();
}

// Shows the npc's graphic from the npc library at double size, or an error caption when it is missing.
DEFINE_SHOW_NPC_GRAPHIC(TFormNpcEditChats)

// Shows the line count in the list's caption with the interval and chance, or the empty-list panel.
void TFormNpcEditChats::ShowCount()
{
    bool settings_shown = Panel2->Visible;
    labelGridCaption->Caption = "Chat Lines";
    panelNoAdds->Visible = false;
    if (lineGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        Panel2->Visible = false;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(lineGrid->Items->Count) + "x";
    spinInterval->Value = chat_interval;
    spinChance->Value = chat_chance;
    Panel2->Visible = true;
    if (!settings_shown)
        spinInterval->SetFocus();
}

void __fastcall TFormNpcEditChats::Panel7Click(TObject *Sender)
{
    spinInterval->Value = 5;
}

void __fastcall TFormNpcEditChats::Panel1Click(TObject *Sender)
{
    spinInterval->Value = 10;
}

void __fastcall TFormNpcEditChats::Panel2Click(TObject *Sender)
{
    spinInterval->Value = 15;
}

void __fastcall TFormNpcEditChats::Panel4Click(TObject *Sender)
{
    spinChance->Value = 50;
}

void __fastcall TFormNpcEditChats::Panel6Click(TObject *Sender)
{
    spinChance->Value = 100;
}

// Clears every line once the warning is confirmed.
void __fastcall TFormNpcEditChats::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Chat Lines";
    String message = "The entire list will be cleared, are you sure?";
    String button_text = "CLEAR LIST";
    FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text);
    if (FormDeleteWarning->ShowModal() == mrOk)
    {
        lineGrid->IsUpdating = true;
        lineGrid->Items->Clear();
        lineGrid->IsUpdating = false;
    }
    ShowCount();
}

// Adds an empty line at the end and selects it; refuses past 250 lines.
void __fastcall TFormNpcEditChats::buttonNewNpcClick(TObject *Sender)
{
    TElTreeItem *row;
    if (lineGrid->Items->Count >= 250)
    {
        String title = "Full Notice";
        String message = "The number of lines should be exceed 250. Cannot add more new chat lines.";
        FormWarning->ShowWarning(this, title, message);
        return;
    }
    row = lineGrid->Items->AddItem(0);
    row->Text = WideString(IntToStr(row->Index + 1));
    TElWideStrings *chat_cells = row->ColumnText;
    chat_cells->Add(WideString(""));
    ShowCount();
    lineGrid->Selected = row;
    if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)lineGrid, -71))((int)lineGrid) &&
        (char)((int __fastcall (*)(int))FindDynaInst((void *)lineGrid, -68))((int)lineGrid))
        lineGrid->SetFocus();
}

// Swaps the selected line's cells with the next line's.
DEFINE_FORM_POPUP_MOVE_DOWN_CLICK(TFormNpcEditChats)

// Swaps the selected line's cells with the previous line's.
DEFINE_FORM_POPUP_MOVE_UP_CLICK(TFormNpcEditChats)

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_FORM_POPUP_DELETE_CLICK(TFormNpcEditChats)

void __fastcall TFormNpcEditChats::spinIntervalEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

// Keeps the number between 1 and the most the box allows.
void __fastcall TFormNpcEditChats::spinIntervalExit(TObject *Sender)
{
    if (((TElSpinEdit *)Sender)->Tag < ((TElSpinEdit *)Sender)->Value)
        ((TElSpinEdit *)Sender)->Value = ((TElSpinEdit *)Sender)->Tag;
    if (((TElSpinEdit *)Sender)->Value < 1)
        ((TElSpinEdit *)Sender)->Value = 1;
}

void __fastcall TFormNpcEditChats::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then stores the interval, chance and filled lines into the npc.
void __fastcall TFormNpcEditChats::buttonOkClick(TObject *Sender)
{
    TElTreeItem *row;
    int i;
    TElTreeStringGrid *grid = lineGrid;
    if (((TCustomElTree *)grid)->IsEditing())
        ((TCustomElTree *)grid)->EndEdit(false);
    if (FormMain->config->GetDeleteWarning() && rows_deleted)
    {
        String title = "Delete Notice";
        String message = "One(1) or more rows have been deleted in this session, please confirm.";
        String button_text = "SAVE |  ID: " + IntToStr(record_index + 1);
        FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text);
        if (FormDeleteWarning->ShowModal() != mrOk)
            return;
    }
    NpcValue *npc = FormMain->npcs_tab->npc_values->GetNpc(record_index);
    npc->talk_interval = (char)spinInterval->Value;
    npc->talk_rate = (char)spinChance->Value;
    // The rows replace the npc's chat lines; empty rows are left out.
    npc->talk_lines.clear();
    i = 0;
    for (; i < grid->Items->Count; i++)
    {
        row = grid->Items->Item[i];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        npc->talk_lines.insert(npc->talk_lines.end(), row->ColumnText->Strings[0]);
    }
    DataGridCol *chat_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Chat");
    TElWideStrings *npc_cells = pub_grid->Selected->ColumnText;
    npc_cells->Strings[chat_column->text_index] = WideString(FormMain->npcs_tab->NumberText(npc->talk_lines.size()));
    ((TCustomForm *)this)->Close();
}

DEFINE_LINE_GRID_ENTER(TFormNpcEditChats)

// Trims the edited text, and accepts it at once when quick save is on.
DEFINE_EL_TREE_INPLACE_EDIT1_AFTER_OPERATION(TFormNpcEditChats)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_LINE_GRID_KEY_DOWN(TFormNpcEditChats)

void __fastcall TFormNpcEditChats::lineGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        popupDeleteClick((TObject *)this);
    if (Shift.Contains(ssCtrl) && Key == 'N')
        buttonNewNpcClick((TObject *)this);
}

// Limits a chat line to 160 characters.
void __fastcall TFormNpcEditChats::ElTreeInplaceEdit1BeforeOperation(TObject *Sender, bool &DefaultConversion)
{
    ElTreeInplaceEdit1->Editor->MaxLength = 160;
}
