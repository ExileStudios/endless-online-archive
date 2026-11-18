#include <vcl.h>
#pragma hdrstop

#include "UnitEditInn.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datagrid.h"
#include "Innvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElSpin"
#pragma resource "*.dfm"

TFormEditInn *FormEditInn;

__fastcall TFormEditInn::TFormEditInn(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    font_color_saved = false;
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and selects the name.
void __fastcall TFormEditInn::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Color = (TColor)0x525252;
    }
    if (!font_color_saved)
    {
        name_font_color = editName->Font->Color;
        font_color_saved = true;
    }
    editName->SetFocus();
    ((TCustomEdit *)editName)->SelectAll();
}

// Opens the form over the main window to create, edit or duplicate the inn at the index.
void TFormEditInn::Open(int operation, int index)
{
    names_grid = (TCustomElTree *)FormMain->inns_tab->main_grid;
    pub_grid = (TCustomElTree *)FormMain->inns_tab->main_grid;
    editName->Hint = String("");
    imageWarnDupe->Visible = false;
    name_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Name");
    save_mode = operation;
    open_operation = operation;
    record_index = index;
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    if (!operation)
        FindNewId();
    if (operation == 2)
        operation = 1;
    LoadFields(record_index, operation);
    if (open_operation == 2)
        FindDupeId();
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe inn), and lines up the captions to fit.
void TFormEditInn::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new inn";
    if (operation == 1)
        labelOperation->Caption = "edit inn";
    if (operation == 2)
        labelOperation->Caption = "dupe inn";
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new inn's defaults, or with the inn row at the index for an update.
void TFormEditInn::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        tag = "";
        editName->Text = "NewInn";
        spinCost->Value = 0;
        spinSleepMap->Value = 0;
        spinSleepX->Value = 0;
        spinSleepY->Value = 0;
        spinHomeMap->Value = 0;
        spinHomeX->Value = 0;
        spinHomeY->Value = 0;
        spinHomeHi->Value = 0;
        spinHomeHiMap->Value = 0;
        spinHomeHiX->Value = 0;
        spinHomeHiY->Value = 0;
        itemNameOriginal->Caption = "New Inn";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        record_id = row->Text;
        DataGridCol *tag_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Tag");
        DataGridCol *name_grid_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *cost_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Cost");
        DataGridCol *sleep_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep Map");
        DataGridCol *sleep_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep X");
        DataGridCol *sleep_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep Y");
        DataGridCol *home_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home Map");
        DataGridCol *home_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home X");
        DataGridCol *home_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home Y");
        DataGridCol *high_level_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi-Level");
        DataGridCol *high_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi Map");
        DataGridCol *high_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi X");
        DataGridCol *high_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi Y");
        tag = String(row->ColumnText->Strings[tag_column->text_index]);
        editName->Text = row->ColumnText->Strings[name_grid_column->text_index];
        spinCost->Value = NumberOrZero(row->ColumnText->Strings[cost_column->text_index]);
        spinSleepMap->Value = NumberOrZero(row->ColumnText->Strings[sleep_map_column->text_index]);
        spinSleepX->Value = NumberOrZero(row->ColumnText->Strings[sleep_x_column->text_index]);
        spinSleepY->Value = NumberOrZero(row->ColumnText->Strings[sleep_y_column->text_index]);
        spinHomeMap->Value = NumberOrZero(row->ColumnText->Strings[home_map_column->text_index]);
        spinHomeX->Value = NumberOrZero(row->ColumnText->Strings[home_x_column->text_index]);
        spinHomeY->Value = NumberOrZero(row->ColumnText->Strings[home_y_column->text_index]);
        spinHomeHi->Value = NumberOrZero(row->ColumnText->Strings[high_level_column->text_index]);
        spinHomeHiMap->Value = NumberOrZero(row->ColumnText->Strings[high_map_column->text_index]);
        spinHomeHiX->Value = NumberOrZero(row->ColumnText->Strings[high_x_column->text_index]);
        spinHomeHiY->Value = NumberOrZero(row->ColumnText->Strings[high_y_column->text_index]);
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

// Takes the ID after the last inn, or with recycling on, the first row with an empty or EOF name.
void TFormEditInn::FindNewId()
{
    int row;
    TCustomElTree *grid = pub_grid;
    record_id = IntToStr(grid->Items->Count + 1);
    buttonOk->Hint = WideString("create new inn");
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
            buttonOk->Hint = WideString("Create Recycled Inn");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FIND_DUPE_ID(TFormEditInn)

DEFINE_NUMBER_OR_ZERO(TFormEditInn)

DEFINE_HIDE_ZERO_TEXT(TFormEditInn)

void __fastcall TFormEditInn::spinSleepMapEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

// Drops the number's leading zeros and caps it at the most the box allows.
void __fastcall TFormEditInn::spinSleepMapExit(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->Text = WideString(FormMain->TrimLeadingZeros(((TElSpinEdit *)Sender)->Text));
    if (((TElSpinEdit *)Sender)->Tag < ((TElSpinEdit *)Sender)->Value)
        ((TElSpinEdit *)Sender)->Value = ((TElSpinEdit *)Sender)->Tag;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormEditInn)

void __fastcall TFormEditInn::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormEditInn)

void __fastcall TFormEditInn::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another inn already has it.
DEFINE_CHECK_DUPLICATE_NAME(TFormEditInn)

// Escape closes the form; Ctrl+A selects all the text of the box.
DEFINE_FORM_KEY_DOWN(TFormEditInn)

// Refuses short or EOF names, then writes the fields into a new or the chosen inn row.
void __fastcall TFormEditInn::buttonOkClick(TObject *Sender)
{
    TElWideStrings *tag_cells;
    TElWideStrings *name_cells;
    TElWideStrings *cost_cells;
    TElWideStrings *sleep_map_cells;
    TElWideStrings *sleep_x_cells;
    TElWideStrings *sleep_y_cells;
    TElWideStrings *home_map_cells;
    TElWideStrings *home_x_cells;
    TElWideStrings *home_y_cells;
    TElWideStrings *high_map_cells;
    TElWideStrings *high_x_cells;
    TElWideStrings *high_y_cells;
    TElWideStrings *high_level_cells;
    TElWideStrings *quiz_cells;
    InnValue *inn;
    TCustomElTree *grid;
    int row[1];
    // Empty names mark deleted rows for recycling, so a name this short needs expert mode.
    if (editName->Text.Length() < 2 && !FormMain->config->GetExpertMode())
    {
        String short_name_message =
            "The inn name [ " + editName->Text +
                " ] is too short, please do not use empty inn names, empty names are reserved to recycle deleted inns.";
        FormWarning->ShowWarning(this, "Inn Name Length", short_name_message);
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
    FormMain->inns_tab->grid_ui->is_ready = false;
    if (!save_mode)
    {
        row[0] = (int)grid->Items->AddItem(0);
        for (int i = 0; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count - 1; i++)
            ((TElTreeItem *)row[0])->ColumnText->Add(WideString(""));
        FormMain->inns_tab->inn_values->InsertInn(FormMain->inns_tab->inn_values->Count());
    }
    if (save_mode == 1)
        row[0] = (int)grid->Items->Item[record_index];
    int column = 0;
    tag_cells = ((TElTreeItem *)row[0])->ColumnText;
    tag_cells->Strings[column++] = WideString(tag);
    name_cells = ((TElTreeItem *)row[0])->ColumnText;
    name_cells->Strings[column++] = WideString(HideZeroText(editName->Text));
    cost_cells = ((TElTreeItem *)row[0])->ColumnText;
    cost_cells->Strings[column++] = WideString(HideZeroText(spinCost->Text));
    sleep_map_cells = ((TElTreeItem *)row[0])->ColumnText;
    sleep_map_cells->Strings[column++] = WideString(HideZeroText(spinSleepMap->Text));
    sleep_x_cells = ((TElTreeItem *)row[0])->ColumnText;
    sleep_x_cells->Strings[column++] = WideString(HideZeroText(spinSleepX->Text));
    sleep_y_cells = ((TElTreeItem *)row[0])->ColumnText;
    sleep_y_cells->Strings[column++] = WideString(HideZeroText(spinSleepY->Text));
    home_map_cells = ((TElTreeItem *)row[0])->ColumnText;
    home_map_cells->Strings[column++] = WideString(HideZeroText(spinHomeMap->Text));
    home_x_cells = ((TElTreeItem *)row[0])->ColumnText;
    home_x_cells->Strings[column++] = WideString(HideZeroText(spinHomeX->Text));
    home_y_cells = ((TElTreeItem *)row[0])->ColumnText;
    home_y_cells->Strings[column++] = WideString(HideZeroText(spinHomeY->Text));
    high_map_cells = ((TElTreeItem *)row[0])->ColumnText;
    high_map_cells->Strings[column++] = WideString(HideZeroText(spinHomeHiMap->Text));
    high_x_cells = ((TElTreeItem *)row[0])->ColumnText;
    high_x_cells->Strings[column++] = WideString(HideZeroText(spinHomeHiX->Text));
    high_y_cells = ((TElTreeItem *)row[0])->ColumnText;
    high_y_cells->Strings[column++] = WideString(HideZeroText(spinHomeHiY->Text));
    high_level_cells = ((TElTreeItem *)row[0])->ColumnText;
    high_level_cells->Strings[column++] = WideString(HideZeroText(spinHomeHi->Text));
    quiz_cells = ((TElTreeItem *)row[0])->ColumnText;
    quiz_cells->Strings[column++] = WideString("");
    FormMain->inns_tab->grid_ui->is_ready = true;
    FormMain->inns_tab->grid_ui->ShowMessage("Inn ( " + record_id + " ) " + editName->Text + " " + ResultText());
    ((TElTreeItem *)row[0])->Text = WideString(record_id);
    ((TElTreeItem *)row[0])->Hidden = false;
    grid->IsUpdating = false;
    inn = FormMain->inns_tab->inn_values->GetInn(StrToInt(record_id) - 1);
    if (inn)
        inn->questions.clear();
    ((TCustomForm *)this)->Close();
}
