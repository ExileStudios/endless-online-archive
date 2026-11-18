#include <vcl.h>
#pragma hdrstop

#include "UnitEditClass.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"

#pragma package(smart_init)
#pragma link "ElCombos"
#pragma link "ElSpin"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormEditClass *FormEditClass;

__fastcall TFormEditClass::TFormEditClass(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    combos_filled = false;
    font_color_saved = false;
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and focuses the name.
DEFINE_FORM_FORM_SHOW(TFormEditClass)

// Escape closes the form; Ctrl+A selects all the text of the box.
DEFINE_FORM_KEY_DOWN(TFormEditClass)

// Opens the form over the main window to create, edit or duplicate the class at the index.
void TFormEditClass::Open(int operation, int index)
{
    names_grid = (TCustomElTree *)FormMain->classes_tab->main_grid;
    pub_grid = (TCustomElTree *)FormMain->classes_tab->main_grid;
    editName->Hint = String("");
    imageWarnDupe->Visible = false;
    name_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
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
    FillParentCombo();
    LoadFields(record_index, operation);
    if (open_operation == 2)
        FindDupeId();
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe class), and lines up the captions to fit.
void TFormEditClass::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new class";
    if (operation == 1)
        labelOperation->Caption = "edit class";
    if (operation == 2)
        labelOperation->Caption = "dupe class";
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new class's defaults, or with the class row at the index for an update.
void TFormEditClass::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        editName->Text = "NewClass";
        comboEditItemType->ItemIndex = 0;
        comboEditItemParent->ItemIndex = 0;
        spinEditStr->Value = 0;
        spinEditInt->Value = 0;
        spinEditWis->Value = 0;
        spinEditAgi->Value = 0;
        spinEditCon->Value = 0;
        spinEditCha->Value = 0;
        itemNameOriginal->Caption = "New Class";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        record_id = row->Text;
        DataGridCol *name_grid_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *stat_group_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Stat Group");
        DataGridCol *parent_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Parent");
        editName->Text = row->ColumnText->Strings[name_grid_column->text_index];
        comboEditItemType->Text = row->ColumnText->Strings[stat_group_column->text_index];
        comboEditItemParent->Text = row->ColumnText->Strings[parent_column->text_index];
        spinEditStr->Value = NumberOrZero(row->ColumnText->Strings[3]);
        spinEditInt->Value = NumberOrZero(row->ColumnText->Strings[4]);
        spinEditWis->Value = NumberOrZero(row->ColumnText->Strings[5]);
        spinEditAgi->Value = NumberOrZero(row->ColumnText->Strings[6]);
        spinEditCon->Value = NumberOrZero(row->ColumnText->Strings[7]);
        spinEditCha->Value = NumberOrZero(row->ColumnText->Strings[8]);
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

void TFormEditClass::FillCombos()
{
    if (combos_filled)
        return;
    FillComboFromColumn(comboEditItemType, "Stat Group");
    combos_filled = true;
}

void TFormEditClass::FillComboFromColumn(TElComboBox *combo, String column_name)
{
    DataGridCol *column = FormMain->classes_tab->grid_ui->data_grid->FindColumn(column_name);
    if (!column)
        return;
    combo->Items->Clear();
    for (int i = 0; i < column->ComboItemCount(); i++)
    {
        DataComboItem *combo_item = column->GetComboItem(i);
        combo->Items->Add(WideString(combo_item->text));
    }
}

// Refills the parent list with a dash and then the name of every class.
void TFormEditClass::FillParentCombo()
{
    comboEditItemParent->Items->Clear();
    TElWideStrings *combo_items = comboEditItemParent->Items;
    combo_items->Add(WideString("-"));
    DataGridCol *name_grid_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
    if (!name_grid_column)
        return;
    for (int i = 0; i < pub_grid->Items->Count; i++)
        comboEditItemParent->Items->Add(pub_grid->Items->Item[i]->ColumnText->Strings[name_grid_column->text_index]);
}

// Takes the ID after the last class, or with recycling on, the first row with an empty or EOF name.
void TFormEditClass::FindNewId()
{
    int row;
    TCustomElTree *grid = pub_grid;
    record_id = IntToStr(grid->Items->Count + 1);
    buttonOk->Hint = WideString("create new class");
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
            buttonOk->Hint = WideString("Create Recycled Class");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FIND_DUPE_ID(TFormEditClass)

void __fastcall TFormEditClass::spinEditStrEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_STR_EXIT(TFormEditClass)

void __fastcall TFormEditClass::comboEditItemTypeEnter(TObject *Sender)
{
    ((TElComboBox *)Sender)->Tag = 0;
    FormMain->combo_text = ((TElComboBox *)Sender)->Text;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormEditClass)

DEFINE_NUMBER_OR_ZERO(TFormEditClass)

DEFINE_HIDE_ZERO_TEXT(TFormEditClass)

void __fastcall TFormEditClass::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses short or EOF names, then writes the fields into a new or the chosen class row.
void __fastcall TFormEditClass::buttonOkClick(TObject *Sender)
{
    TElWideStrings *name_cells;
    TElWideStrings *stat_group_cells;
    TElWideStrings *parent_cells;
    TElWideStrings *strength_cells;
    TElWideStrings *intelligence_cells;
    TElWideStrings *wisdom_cells;
    TElWideStrings *agility_cells;
    TElWideStrings *constitution_cells;
    TElWideStrings *charisma_cells;
    TCustomElTree *grid;
    int row;
    // Empty names mark deleted rows for recycling, so a name this short needs expert mode.
    if (editName->Text.Length() < 2 && !FormMain->config->GetExpertMode())
    {
        String short_name_message =
            "The class name [ " + editName->Text +
                " ] is too short, please do not use empty class names, empty names are reserved to recycle deleted "
                    "classes.";
        FormWarning->ShowWarning(this, "Class Name Length", short_name_message);
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
    FormMain->classes_tab->grid_ui->is_ready = false;
    if (!save_mode)
    {
        row = (int)grid->Items->AddItem(0);
        for (int i = 0; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count - 1; i++)
            ((TElTreeItem *)row)->ColumnText->Add(WideString(""));
    }
    if (save_mode == 1)
        row = (int)grid->Items->Item[record_index];
    int column = 0;
    name_cells = ((TElTreeItem *)row)->ColumnText;
    name_cells->Strings[column++] = WideString(HideZeroText(editName->Text));
    stat_group_cells = ((TElTreeItem *)row)->ColumnText;
    stat_group_cells->Strings[column++] = WideString(HideZeroText(comboEditItemType->Text));
    parent_cells = ((TElTreeItem *)row)->ColumnText;
    parent_cells->Strings[column++] = WideString(HideZeroText(comboEditItemParent->Text));
    strength_cells = ((TElTreeItem *)row)->ColumnText;
    strength_cells->Strings[column++] = WideString(HideZeroText(spinEditStr->Text));
    intelligence_cells = ((TElTreeItem *)row)->ColumnText;
    intelligence_cells->Strings[column++] = WideString(HideZeroText(spinEditInt->Text));
    wisdom_cells = ((TElTreeItem *)row)->ColumnText;
    wisdom_cells->Strings[column++] = WideString(HideZeroText(spinEditWis->Text));
    agility_cells = ((TElTreeItem *)row)->ColumnText;
    agility_cells->Strings[column++] = WideString(HideZeroText(spinEditAgi->Text));
    constitution_cells = ((TElTreeItem *)row)->ColumnText;
    constitution_cells->Strings[column++] = WideString(HideZeroText(spinEditCon->Text));
    charisma_cells = ((TElTreeItem *)row)->ColumnText;
    charisma_cells->Strings[column++] = WideString(HideZeroText(spinEditCha->Text));
    FormMain->classes_tab->grid_ui->is_ready = true;
    FormMain->classes_tab->grid_ui->ShowMessage("Class ( " + record_id + " ) " + editName->Text + " " + ResultText());
    ((TElTreeItem *)row)->Text = WideString(record_id);
    ((TElTreeItem *)row)->Hidden = false;
    grid->IsUpdating = false;
    ((TCustomForm *)this)->Close();
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormEditClass)

void __fastcall TFormEditClass::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another class already has it.
DEFINE_CHECK_DUPLICATE_NAME(TFormEditClass)

// Settles the typed stat group text on a list entry, by partial name or number, or an out-of-bounds tag.
DEFINE_COMBO_EDIT_ITEM_TYPE_EXIT(TFormEditClass)

void TFormEditClass::TypeChosen()
{
}

// Escape restores the combo's text from entry, Enter settles it; typing drops the list open.
DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_DOWN(TFormEditClass)

// Enter opens the list on the first presses, then settles the remembered text; other keys remember it.
DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_UP(TFormEditClass)

// Escape restores the combo's text from entry; typing drops the list open and [ closes it.
void __fastcall TFormEditClass::comboEditItemParentKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
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
    if (!Key && !((TElComboBox *)Sender)->DroppedDown)
        ((TElComboBox *)Sender)->DropDown();
}
