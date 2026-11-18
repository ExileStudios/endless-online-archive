#include <vcl.h>
#pragma hdrstop

#include "UnitEditTrainer.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Learnvalue.h"
#include "Learnvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElSpin"
#pragma link "ElCombos"
#pragma resource "*.dfm"

TFormEditTrainer *FormEditTrainer;

__fastcall TFormEditTrainer::TFormEditTrainer(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    combos_filled = false;
    font_color_saved = false;
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and focuses the name.
DEFINE_FORM_FORM_SHOW(TFormEditTrainer)

// Escape closes the form; Ctrl+A selects all the text of the box.
DEFINE_FORM_KEY_DOWN(TFormEditTrainer)

// Opens the form over the main window to create, edit or duplicate the trainer at the index.
void TFormEditTrainer::Open(int operation, int index)
{
    names_grid = (TCustomElTree *)FormMain->trainers_tab->main_grid;
    pub_grid = (TCustomElTree *)FormMain->trainers_tab->main_grid;
    editName->Hint = String("");
    imageWarnDupe->Visible = false;
    name_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Name");
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
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe trainer), and lines up the captions to fit.
void TFormEditTrainer::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new trainer";
    if (operation == 1)
        labelOperation->Caption = "edit trainer";
    if (operation == 2)
        labelOperation->Caption = "dupe trainer";
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new trainer's defaults, or with the trainer row at the index for an update.
void TFormEditTrainer::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        tag = "";
        editName->Text = "NewTrainer";
        comboEditClass->ItemIndex = 0;
        spinEditMin->Value = 0;
        spinEditMax->Value = 252;
        itemNameOriginal->Caption = "New Trainer";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        record_id = row->Text;
        DataGridCol *tag_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Tag");
        DataGridCol *name_grid_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *class_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Cls");
        DataGridCol *level_min_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Lvl.min");
        DataGridCol *level_max_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Lvl.max");
        tag = String(row->ColumnText->Strings[tag_column->text_index]);
        editName->Text = row->ColumnText->Strings[name_grid_column->text_index];
        comboEditClass->Text = row->ColumnText->Strings[class_column->text_index];
        spinEditMin->Value = NumberOrZero(row->ColumnText->Strings[level_min_column->text_index]);
        spinEditMax->Value = NumberOrZero(row->ColumnText->Strings[level_max_column->text_index]);
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

DEFINE_FILL_COMBOS(TFormEditTrainer)

// Refills the combo with a dash and then the column's text from every row of the grid.
DEFINE_FILL_COMBO_FROM_GRID(TFormEditTrainer)

// Takes the ID after the last trainer, or with recycling on, the first row with an empty or EOF name.
void TFormEditTrainer::FindNewId()
{
    int row;
    TCustomElTree *grid = pub_grid;
    record_id = IntToStr(grid->Items->Count + 1);
    buttonOk->Hint = WideString("create new trainer");
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
            buttonOk->Hint = WideString("Create Recycled Trainer");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FIND_DUPE_ID(TFormEditTrainer)

DEFINE_NUMBER_OR_ZERO(TFormEditTrainer)

DEFINE_HIDE_ZERO_TEXT(TFormEditTrainer)

void __fastcall TFormEditTrainer::spinEditMinEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_MIN_EXIT(TFormEditTrainer)

void __fastcall TFormEditTrainer::comboEditClassEnter(TObject *Sender)
{
    ((TElComboBox *)Sender)->Tag = 0;
    FormMain->combo_text = ((TElComboBox *)Sender)->Text;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormEditTrainer)

void __fastcall TFormEditTrainer::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormEditTrainer)

void __fastcall TFormEditTrainer::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another trainer already has it.
DEFINE_CHECK_DUPLICATE_NAME(TFormEditTrainer)

// Escape restores the combo's text from entry; typing drops the list open and [ closes it.
DEFINE_COMBO_EDIT_CLASS_KEY_DOWN(TFormEditTrainer)

// Refuses short or EOF names, then writes the fields into a new or the chosen trainer row.
void __fastcall TFormEditTrainer::buttonOkClick(TObject *Sender)
{
    TElWideStrings *tag_cells;
    TElWideStrings *name_cells;
    TElWideStrings *level_min_cells;
    TElWideStrings *level_max_cells;
    TElWideStrings *class_cells;
    TElWideStrings *skill_cells;
    LearnValue *trainer;
    TCustomElTree *grid;
    int row;
    // Empty names mark deleted rows for recycling, so a name this short needs expert mode.
    if (editName->Text.Length() < 2 && !FormMain->config->GetExpertMode())
    {
        String short_name_message =
            "The shop name [ " + editName->Text +
                " ] is too short, please do not use empty shop names, empty names are reserved to recycle deleted "
                    "shops.";
        FormWarning->ShowWarning(this, "Trainer Name Length", short_name_message);
        return;
    }
    if (LowerCase(editName->Text) == "eof")
    {
        String eof_message = "EOF tags are not needed. The Endless parser supports EOF automatically once needed.";
        FormWarning->ShowWarning(this, "About EOF..", eof_message);
        return;
    }
    grid = pub_grid;
    if (!grid->IsUpdating)
        grid->IsUpdating = true;
    FormMain->trainers_tab->grid_ui->is_ready = false;
    if (!save_mode)
    {
        row = (int)grid->Items->AddItem(0);
        for (int i = 0; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count - 1; i++)
            ((TElTreeItem *)row)->ColumnText->Add(WideString(""));
        FormMain->trainers_tab->trainer_values->InsertTrainer(FormMain->trainers_tab->trainer_values->Count());
    }
    if (save_mode == 1)
        row = (int)grid->Items->Item[record_index];
    int column = 0;
    tag_cells = ((TElTreeItem *)row)->ColumnText;
    tag_cells->Strings[column++] = WideString(tag);
    name_cells = ((TElTreeItem *)row)->ColumnText;
    name_cells->Strings[column++] = WideString(HideZeroText(editName->Text));
    level_min_cells = ((TElTreeItem *)row)->ColumnText;
    level_min_cells->Strings[column++] = spinEditMin->Text;
    level_max_cells = ((TElTreeItem *)row)->ColumnText;
    level_max_cells->Strings[column++] = spinEditMax->Text;
    class_cells = ((TElTreeItem *)row)->ColumnText;
    class_cells->Strings[column++] = WideString(HideZeroText(comboEditClass->Text));
    skill_cells = ((TElTreeItem *)row)->ColumnText;
    skill_cells->Strings[column++] = WideString("");
    FormMain->trainers_tab->grid_ui->is_ready = true;
    FormMain->trainers_tab->grid_ui->ShowMessage("Trainer ( " + record_id + " ) " + editName->Text + " " +
        ResultText());
    ((TElTreeItem *)row)->Text = WideString(record_id);
    ((TElTreeItem *)row)->Hidden = false;
    grid->IsUpdating = false;
    trainer = FormMain->trainers_tab->trainer_values->Insert(StrToInt(record_id) - 1);
    if (trainer)
        trainer->skills.clear();
    ((TCustomForm *)this)->Close();
}
