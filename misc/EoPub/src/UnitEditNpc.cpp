#include <vcl.h>
#pragma hdrstop

#include "UnitEditNpc.h"
#include "UnitMain.h"
#include "UnitGfx.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Npcvalue.h"
#include "Npcvalues.h"

#pragma package(smart_init)
#pragma link "ElSpin"
#pragma link "ElCombos"
#pragma link "ElTreeGrids"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormNpcEdit *FormNpcEdit;

__fastcall TFormNpcEdit::TFormNpcEdit(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    data_form_made = false;
    combos_filled = false;
    font_color_saved = false;
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and clears the quick-save cell.
DEFINE_T_FORM_ITEM_EDIT_FORM_SHOW(TFormNpcEdit)

DEFINE_FORM_KEY_DOWN(TFormNpcEdit)

// Opens the form over the main window to create, edit or duplicate the npc, with its data grid and graphic.
void TFormNpcEdit::Open(int operation, int index)
{
    pub_grid = (TCustomElTree *)FormMain->npcs_tab->main_grid;
    edit_grid = (TCustomElTree *)editStringGrid;
    editName->Hint = String("");
    imageWarnDupe->Visible = false;
    if (!data_form_made)
    {
        data_form = new DataForm(String("npc"), FormMain->npcs_tab->grid_ui, (int)pub_grid, editStringGrid);
        data_form->SetGraphics(FormMain->gfx_vault->npcs_library, 101, 40);
        data_form_made = true;
    }
    gfx_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    name_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Name");
    level = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Level");
    experience = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Experience");
    behaviour_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Behaviour");
    behaviour_id_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Beh.ID");
    save_mode = operation;
    open_operation = operation;
    record_index = index;
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    if (!operation)
        FindNewId();
    if (operation == 2)
        operation = 1;
    FillCombos();
    LoadFields(record_index, operation);
    if (open_operation == 2)
        FindDupeId();
    DataComboItem *type_item = behaviour_column->GetComboItem((short)comboEditBehaviour->ItemIndex);
    labelEditGridVisible->Caption = IntToStr(data_form->FillGrid(editStringGrid, type_item, operation, ""));
    data_form->ShowGraphic(panelImage, imageNpc, Label11, spinEditGfx->Value);
    imageNpc->Hint = FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_column->text_index);
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe npc) and the current type, and lines up the captions to fit.
void TFormNpcEdit::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new npc";
    if (operation == 1)
        labelOperation->Caption = "edit npc";
    if (operation == 2)
        labelOperation->Caption = "dupe npc";
    captionCurrentType->Caption = comboEditBehaviour->Text;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new npc's defaults, or with the npc row at the index for an update.
void TFormNpcEdit::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        editName->Text = "NoName";
        spinEditGfx->Value = 1;
        spinEditLvl->Value = 1;
        spinEditExp->Value = 1;
        comboEditBehaviour->ItemIndex = 0;
        spinEditBehaviourID->Value = 0;
        itemNameOriginal->Caption = "New Npc";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        editName->Text = row->ColumnText->Strings[name_column->text_index];
        spinEditGfx->Value = data_form->DigitsOnly(row->ColumnText->Strings[gfx_column->text_index]);
        spinEditLvl->Value = data_form->DigitsOnly(row->ColumnText->Strings[level->text_index]);
        spinEditExp->Value = data_form->DigitsOnly(row->ColumnText->Strings[experience->text_index]);
        comboEditBehaviour->Text = row->ColumnText->Strings[behaviour_column->text_index];
        spinEditBehaviourID->Value = data_form->DigitsOnly(row->ColumnText->Strings[behaviour_id_column->text_index]);
        record_id = row->Text;
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

void TFormNpcEdit::FillCombos()
{
    if (combos_filled)
        return;
    data_form->FillCombo(comboEditBehaviour, "Behaviour");
    combos_filled = true;
}

// Takes the ID after the last npc, or with recycling on, the first row with an empty or EOF name.
void TFormNpcEdit::FindNewId()
{
    int row;
    TCustomElTree *grid = pub_grid;
    record_id = (int)(grid->Items->Count + 1);
    buttonOk->Hint = WideString("Create New Item");
    if (!FormMain->config->GetCreateRecycled())
        return;
    int name_index = 0;
    for (int i = 1; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count; i++)
    {
        if (String(((TElTreeStringGrid *)grid)->HeaderSections->Item[i]->Text) == "Name")
        {
            name_index = i - 1;
            break;
        }
    }
    for (row = 0; row < grid->Items->Count; row++)
    {
        if (grid->Items->Item[row]->ColumnText->Strings[name_index].Length() < 2 ||
            LowerCase(grid->Items->Item[row]->ColumnText->Strings[name_index]) == "eof")
        {
            record_index = row;
            save_mode = 1;
            record_id = String(grid->Items->Item[row]->Text);
            buttonOk->Hint = WideString("Create Recycled Npc");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FORM_FIND_DUPE_ID(TFormNpcEdit)

// Blanks a zero in the edited cell when zeros are hidden.
DEFINE_EDIT_STRING_GRID_ITEM_CHANGE(TFormNpcEdit)

void __fastcall TFormNpcEdit::editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    char edited = data_form->EditCell(Item, SectionIndex, Editor, comboEditBehaviour->Text);
}

void __fastcall TFormNpcEdit::editStringGridEnter(TObject *Sender)
{
    data_form->SelectFirstRow();
}

// Opens the graphics picker on the npc library and takes the chosen graphic.
void __fastcall TFormNpcEdit::panelImageClick(TObject *Sender)
{
    DataGridCol *gfx_grid_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    FormGfx->SetLibrary(FormMain->gfx_vault->npcs_library, 101, 40, FormMain->npcs_tab->grid_ui->gfx_first,
        (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
    FormGfx->ShowCaptions(this, "Npc Gfx", editName->Text, spinEditGfx->Value);
    imageNpc->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
    spinEditGfx->Value = StrToInt(FormGfx->chosen_number);
    data_form->ShowGraphic(panelImage, imageNpc, Label11, spinEditGfx->Value);
    imageNpc->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
}

void __fastcall TFormNpcEdit::Panel6Click(TObject *Sender)
{
    spinEditGfx->Value = FormMain->npcs_tab->grid_ui->gfx_first;
    spinEditGfxChange((TObject *)spinEditGfx);
}

void __fastcall TFormNpcEdit::Panel7Click(TObject *Sender)
{
    spinEditGfx->Value = 1;
    spinEditGfxChange((TObject *)spinEditGfx);
}

// Shows the graphic of the number in the box, with its hint.
void __fastcall TFormNpcEdit::spinEditGfxChange(TObject *Sender)
{
    if (String(spinEditGfx->Text) == "")
        return;
    data_form->ShowGraphic(panelImage, imageNpc, Label11, spinEditGfx->Value);
    imageNpc->Hint = FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_column->text_index);
}

// Shows the chosen behaviour and refills the data grid with that behaviour's fields.
void __fastcall TFormNpcEdit::comboEditBehaviourChange(TObject *Sender)
{
    captionCurrentType->Caption = comboEditBehaviour->Text;
    DataComboItem *type_item = behaviour_column->GetComboItem((short)comboEditBehaviour->ItemIndex);
    labelEditGridVisible->Caption =
        IntToStr(data_form->FillGrid(editStringGrid, type_item, (char)open_operation, comboEditBehaviour->Text));
}

void __fastcall TFormNpcEdit::spinEditGfxEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

void __fastcall TFormNpcEdit::comboEditBehaviourEnter(TObject *Sender)
{
    ((TElComboBox *)Sender)->Tag = 0;
    FormMain->combo_text = ((TElComboBox *)Sender)->Text;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormNpcEdit)

void __fastcall TFormNpcEdit::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses short or EOF names, then writes the fields and the data grid into a new or the chosen npc row.
void __fastcall TFormNpcEdit::buttonOkClick(TObject *Sender)
{
    TElWideStrings *name_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *level_cells;
    TElWideStrings *experience_cells;
    TElWideStrings *drop_cells;
    TElWideStrings *chat_cells;
    TElWideStrings *behaviour_cells;
    TElWideStrings *behaviour_id_cells;
    TCustomElTree *grid;
    int row[1];
    // Empty names mark deleted rows for recycling, so a name this short needs expert mode.
    if (editName->Text.Length() < 2 && !FormMain->config->GetExpertMode())
    {
        String short_name_message =
            "The item name [ " + editName->Text +
                " ] is too short, please do not use empty item names, empty names are reserved to recycle deleted "
                    "rows.";
        FormWarning->ShowWarning(this, "Item Name Length", short_name_message);
        return;
    }
    if (LowerCase(editName->Text) == "eof")
    {
        String eof_message = "EOF tags are not needed. The Endless parser supports EOF automatically once needed.";
        FormWarning->ShowWarning(this, "About EOF..", eof_message);
        return;
    }
    if (edit_grid->IsEditing())
        edit_grid->EndEdit(false);
    grid = pub_grid;
    if (!grid->IsUpdating)
        grid->IsUpdating = true;
    FormMain->npcs_tab->grid_ui->is_ready = false;
    if (!save_mode)
    {
        row[0] = (int)grid->Items->AddItem(0);
        for (int i = 0; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count - 1; i++)
            ((TElTreeItem *)row[0])->ColumnText->Add(WideString(""));
        FormMain->npcs_tab->npc_values->InsertNpc(FormMain->npcs_tab->npc_values->Count());
    }
    if (save_mode == 1)
        row[0] = (int)grid->Items->Item[record_index];
    int column = 0;
    name_cells = ((TElTreeItem *)row[0])->ColumnText;
    name_cells->Strings[column++] = WideString(data_form->HideZeroText(editName->Text));
    gfx_cells = ((TElTreeItem *)row[0])->ColumnText;
    gfx_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditGfx->Text));
    level_cells = ((TElTreeItem *)row[0])->ColumnText;
    level_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditLvl->Text));
    experience_cells = ((TElTreeItem *)row[0])->ColumnText;
    experience_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditExp->Text));
    drop_cells = ((TElTreeItem *)row[0])->ColumnText;
    drop_cells->Strings[column++] = WideString("");
    chat_cells = ((TElTreeItem *)row[0])->ColumnText;
    chat_cells->Strings[column++] = WideString("");
    behaviour_cells = ((TElTreeItem *)row[0])->ColumnText;
    behaviour_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditBehaviour->Text));
    behaviour_id_cells = ((TElTreeItem *)row[0])->ColumnText;
    behaviour_id_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditBehaviourID->Text));
    for (int column_count = ((TElTreeItem *)row[0])->ColumnText->Count; column < column_count; column++)
    {
        String cell_text(edit_grid->Items->Item[column + 1]->ColumnText->Strings[0]);
        TElWideStrings *data_cells = ((TElTreeItem *)row[0])->ColumnText;
        data_cells->Strings[column] = WideString(cell_text);
    }
    FormMain->npcs_tab->grid_ui->is_ready = true;
    FormMain->npcs_tab->grid_ui->ShowMessage("Npc ( " + record_id + " ) " + editName->Text + " " + ResultText());
    ((TElTreeItem *)row[0])->Text = WideString(record_id);
    ((TElTreeItem *)row[0])->Hidden = false;
    grid->IsUpdating = false;
    NpcValue *npc = FormMain->npcs_tab->npc_values->GetNpc(StrToInt(record_id) - 1);
    if (npc)
    {
        npc->drops.clear();
        npc->talk_lines.clear();
    }
    ((TCustomForm *)this)->Close();
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormNpcEdit)

void __fastcall TFormNpcEdit::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another npc already has it.
DEFINE_FORM_CHECK_DUPLICATE_NAME(TFormNpcEdit)

void __fastcall TFormNpcEdit::editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item, TElHeaderSection *Section,
    TElFString &Text, THintWindow *HintWindow, TPoint &MousePos, bool &DoShowHint)
{
    DoShowHint = false;
}

// Settles the typed behaviour text on a list entry, by partial name or number, or an out-of-bounds tag.
void __fastcall TFormNpcEdit::comboEditBehaviourExit(TObject *Sender)
{
    String text(((TElComboBox *)Sender)->Text);
    String lower_text = LowerCase(text);
    for (int i = 0; ((TElComboBox *)Sender)->Items->Count > i; i++)
    {
        if (((TElComboBox *)Sender)->Text.Length() > 2)
        {
            if (LowerCase(((TElComboBox *)Sender)->Items->Strings[i]).Pos(lower_text) > 0)
            {
                ((TElComboBox *)Sender)->ItemIndex = i;
                ((TElComboBox *)Sender)->Text = ((TElComboBox *)Sender)->Items->Strings[i];
                TypeChosen();
                FormMain->combo_text = ((TElComboBox *)Sender)->Text;
                return;
            }
        }
        else if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[i])
        {
            TypeChosen();
            FormMain->combo_text = ((TElComboBox *)Sender)->Text;
            return;
        }
    }
    // A typed number picks the entry at that position; past the list it becomes an out-of-bounds tag.
    int number = FormMain->NumberFromDigits(text);
    if (number >= 0)
    {
        if (((TElComboBox *)Sender)->Items->Count > number)
        {
            ((TElComboBox *)Sender)->ItemIndex = number;
            text = ((TElComboBox *)Sender)->Items->Strings[number];
        }
        else
        {
            if (number > 252)
                number = 252;
            text = "[OOB-" + IntToStr(number) + "]";
        }
        ((TElComboBox *)Sender)->Text = WideString(text);
        FormMain->combo_text = ((TElComboBox *)Sender)->Text;
    }
    for (int j = 0; ((TElComboBox *)Sender)->Items->Count > j; j++)
    {
        if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[j])
        {
            TypeChosen();
            FormMain->combo_text = ((TElComboBox *)Sender)->Text;
            return;
        }
    }
    int out_of_bounds = FormMain->OutOfBoundsValue(text);
    if (out_of_bounds >= 0)
    {
        if (out_of_bounds > 252)
            out_of_bounds = 252;
        ((TElComboBox *)Sender)->Text = WideString("[OOB-" + IntToStr(out_of_bounds) + "]");
        TypeChosen();
        FormMain->combo_text = ((TElComboBox *)Sender)->Text;
        return;
    }
    for (int k = 0; ((TElComboBox *)Sender)->Items->Count > k; k++)
    {
        if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[k])
        {
            TypeChosen();
            FormMain->combo_text = ((TElComboBox *)Sender)->Text;
            return;
        }
    }
    ((TElComboBox *)Sender)->Text = WideString(FormMain->combo_text);
    TypeChosen();
}

void TFormNpcEdit::TypeChosen()
{
    comboEditBehaviourChange((TObject *)comboEditBehaviour);
}

// Escape restores the combo's text from entry, Enter settles it; typing drops the list open.
void __fastcall TFormNpcEdit::comboEditBehaviourKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    String text(((TElComboBox *)Sender)->Text);
    if (Key == VK_ESCAPE)
    {
        if (((TElComboBox *)Sender)->Text != FormMain->combo_text)
        {
            ((TElComboBox *)Sender)->Text = WideString(FormMain->combo_text);
            ((TElComboBox *)Sender)->SelectAll();
            if (!((TElComboBox *)Sender)->DroppedDown)
                ((TElComboBox *)Sender)->DropDown();
        }
    }
    if (text.Length() < 1)
        return;
    if (((TElComboBox *)Sender)->DroppedDown)
    {
        if (Key == VK_OEM_4)
            ((TElComboBox *)Sender)->CloseUp(false);
    }
    else
        ((TElComboBox *)Sender)->DropDown();
    if (Key == VK_RETURN)
    {
        if (((TElComboBox *)Sender)->DroppedDown)
            ((TElComboBox *)Sender)->CloseUp(false);
        comboEditBehaviourExit(Sender);
        FormMain->combo_text = ((TElComboBox *)Sender)->Text;
        ((TElComboBox *)Sender)->SelectAll();
    }
    if (!Key && !((TElComboBox *)Sender)->DroppedDown)
        ((TElComboBox *)Sender)->DropDown();
}

// Enter opens the list on the first presses, then settles the remembered text; other keys remember it.
void __fastcall TFormNpcEdit::comboEditBehaviourKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    String text(((TElComboBox *)Sender)->Text);
    ((TElComboBox *)Sender)->Tag++;
    if (Key == VK_RETURN)
    {
        if (((TElComboBox *)Sender)->Tag < 3)
        {
            if (!((TElComboBox *)Sender)->DroppedDown)
                ((TElComboBox *)Sender)->DropDown();
            ((TElComboBox *)Sender)->SelectAll();
            return;
        }
        if (((TElComboBox *)Sender)->Text.Length() < 1)
        {
            if (FormMain->typed_text.Length() < 1)
                return;
            ((TElComboBox *)Sender)->Text = WideString(FormMain->typed_text);
            comboEditBehaviourExit(Sender);
            FormMain->combo_text = ((TElComboBox *)Sender)->Text;
            FormMain->typed_text = "";
            ((TElComboBox *)Sender)->SelectAll();
        }
        if (((TElComboBox *)Sender)->DroppedDown)
            ((TElComboBox *)Sender)->CloseUp(false);
        return;
    }
    FormMain->typed_text = text;
}

// Escape puts back the cell value kept for quick save, when quick save is on.
DEFINE_EDIT_STRING_GRID_KEY_UP(TFormNpcEdit)

// Forgets the cell value kept for quick save.
void __fastcall TFormNpcEdit::editStringGridItemFocused(TObject *Sender)
{
    FormMain->edit_backup = "";
    FormMain->edit_grid = 0;
    FormMain->edit_row = 0;
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_GFX_EXIT(TFormNpcEdit)
