#include <vcl.h>
#pragma hdrstop

#include "UnitEditItem.h"
#include "UnitMain.h"
#include "UnitGfx.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"

#pragma package(smart_init)
#pragma link "ElSpin"
#pragma link "ElTreeGrids"
#pragma link "ElCombos"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormItemEdit *FormItemEdit;

__fastcall TFormItemEdit::TFormItemEdit(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    data_form_made = false;
    combos_filled = false;
    font_color_saved = false;
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and clears the quick-save cell.
DEFINE_T_FORM_ITEM_EDIT_FORM_SHOW(TFormItemEdit)

DEFINE_FORM_KEY_DOWN(TFormItemEdit)

// Opens the form over the main window to create, edit or duplicate the item, with its data grid and graphic.
void TFormItemEdit::Open(int operation, int index)
{
    pub_grid = (TCustomElTree *)FormMain->items_tab->main_grid;
    edit_grid = (TCustomElTree *)editStringGrid;
    editName->Hint = String("");
    imageWarnDupe->Visible = false;
    if (!data_form_made)
    {
        data_form = new DataForm(String("item"), FormMain->items_tab->grid_ui, (int)pub_grid, editStringGrid);
        data_form->SetGraphics(FormMain->gfx_vault->items_library, 102, 2);
        data_form_made = true;
    }
    gfx_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
    type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    subtype_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Subtype");
    unique_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Unique");
    weight_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Weight");
    size_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Size");
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
    DataComboItem *type_item = type_column->GetComboItem((short)comboEditItemType->ItemIndex);
    labelEditGridVisible->Caption =
        IntToStr(data_form->FillGrid(editStringGrid, type_item, operation, comboEditItemType->Text));
    data_form->ShowGraphic(panelImage, imageItem, Label11, spinEditGfx->Value);
    imageItem->Hint = FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_column->text_index);
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe item) and the current type, and lines up the captions to fit.
void TFormItemEdit::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new item";
    if (operation == 1)
        labelOperation->Caption = "edit item";
    if (operation == 2)
        labelOperation->Caption = "dupe item";
    captionCurrentType->Caption = comboEditItemType->Text;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new item's defaults, or with the item row at the index for an update.
void TFormItemEdit::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        editName->Text = "NoName";
        spinEditGfx->Value = 1;
        comboEditItemType->ItemIndex = 0;
        comboEditSubType->ItemIndex = 0;
        comboEditUnique->ItemIndex = 0;
        spinEditWeight->Value = 0;
        comboEditSize->ItemIndex = 0;
        itemNameOriginal->Caption = "New Item";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        comboEditItemType->Text = row->ColumnText->Strings[type_column->text_index];
        comboEditSubType->Text = row->ColumnText->Strings[subtype_column->text_index];
        comboEditUnique->Text = row->ColumnText->Strings[unique_column->text_index];
        comboEditSize->Text = row->ColumnText->Strings[size_column->text_index];
        editName->Text = row->ColumnText->Strings[name_column->text_index];
        spinEditGfx->Value = data_form->DigitsOnly(row->ColumnText->Strings[gfx_column->text_index]);
        spinEditWeight->Value = data_form->DigitsOnly(row->ColumnText->Strings[weight_column->text_index]);
        record_id = row->Text;
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

// Fills the type, subtype, unique and size lists, only the first time.
void TFormItemEdit::FillCombos()
{
    if (combos_filled)
        return;
    data_form->FillCombo(comboEditItemType, "Type");
    data_form->FillCombo(comboEditSubType, "Subtype");
    data_form->FillCombo(comboEditUnique, "Unique");
    data_form->FillCombo(comboEditSize, "Size");
    combos_filled = true;
}

// Takes the ID after the last item, or with recycling on, the first row with an empty or EOF name.
void TFormItemEdit::FindNewId()
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
            buttonOk->Hint = WideString("Create Recycled Item");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FORM_FIND_DUPE_ID(TFormItemEdit)

void __fastcall TFormItemEdit::Panel5Click()
{
    ModalResult = mrCancel;
}

void __fastcall TFormItemEdit::Panel4Click()
{
    ModalResult = mrOk;
}

// Opens the graphics picker on the item library and takes the chosen graphic and size.
void __fastcall TFormItemEdit::panelImageClick(TObject *Sender)
{
    DataGridCol *gfx_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    FormGfx->SetLibrary(FormMain->gfx_vault->items_library, 102, 2, FormMain->items_tab->grid_ui->gfx_first,
        (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
    FormGfx->ShowCaptions(this, "Item Gfx", editName->Text, spinEditGfx->Value);
    imageItem->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
    comboEditSize->ItemIndex = FormGfx->chosen_size;
    spinEditGfx->Value = StrToInt(FormGfx->chosen_number);
    data_form->ShowGraphic(panelImage, imageItem, Label11, spinEditGfx->Value);
    imageItem->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_grid_column->text_index);
}

void __fastcall TFormItemEdit::Panel6Click(TObject *Sender)
{
    spinEditGfx->Value = FormMain->items_tab->grid_ui->gfx_first;
    spinEditGfxChange((TObject *)spinEditGfx);
}

void __fastcall TFormItemEdit::Panel7Click(TObject *Sender)
{
    spinEditGfx->Value = 1;
    spinEditGfxChange((TObject *)spinEditGfx);
}

// Shows the graphic of the number in the box, with its hint.
void __fastcall TFormItemEdit::spinEditGfxChange(TObject *Sender)
{
    if (String(spinEditGfx->Text) == "")
        return;
    data_form->ShowGraphic(panelImage, imageItem, Label11, spinEditGfx->Value);
    imageItem->Hint = FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, gfx_column->text_index);
}

// Blanks a zero in the edited cell when zeros are hidden.
DEFINE_EDIT_STRING_GRID_ITEM_CHANGE(TFormItemEdit)

// Opens the cell's editor; for equipment, the character graphic picker of the item's type and gender.
void __fastcall TFormItemEdit::editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    TElWideStrings *cells;
    char edit_kind = data_form->EditCell(Item, SectionIndex, Editor, comboEditItemType->Text);
    if ((unsigned char)edit_kind == ColumnKind_PaperdollGraphic)
    {
        String item_name = editName->Text;
        int item_type = comboEditItemType->ItemIndex;
        int gfx = data_form->DigitsOnly(Item->ColumnText->Strings[0]);
        DataGridCol *gender_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2");
        int gender_row = gender_column->index;
        String gender_text(data_form->form_grid->Items->Item[gender_row]->ColumnText->Strings[0]);
        int is_male = 1;
        int female_pos = gender_text.Pos("female");
        if (female_pos > 0)
            is_male = 0;
        int library = 0;
        int gfx_step = 0;
        String library_caption = "";
        if (item_type == EifItemType_Boots)
        {
            gfx_step = 40;
            library = 12;
            library_caption = "Boots";
        }
        if (item_type == EifItemType_Armor)
        {
            gfx_step = 50;
            library = 14;
            library_caption = "Armor";
        }
        if (item_type == EifItemType_Hat)
        {
            gfx_step = 10;
            library = 16;
            library_caption = "Hat";
        }
        if (item_type == EifItemType_Weapon)
        {
            gfx_step = 100;
            library = 18;
            library_caption = "Weapon";
        }
        if (item_type == EifItemType_Shield)
        {
            gfx_step = 50;
            library = 20;
            library_caption = "Shield";
        }
        if (!library)
            return;
        library_caption = "Character " + library_caption + " Gfx";
        if (is_male > 1)
            is_male = 1;
        library = library - is_male;
        if (item_type == EifItemType_Armor && !is_male)
            library_caption = "Fem. " + library_caption;
        if (item_type == EifItemType_Armor && is_male == 1)
            library_caption = "Male " + library_caption;
        if (!FormGfx->OpenLibrary(library, 101, gfx_step, data_form->form_grid, 0))
        {
            FormWarning->ShowWarning(this, "Warning EGF",
                "Cannot find gfx file [ " + FormGfx->library_file +
                    " ] in the Read Gfx Path. Please fix your files and try again.");
            return;
        }
        FormGfx->ShowCaptions(this, library_caption, item_name, gfx);
        if (FormGfx->ModalResult == mrOk)
        {
            cells = Item->ColumnText;
            cells->Strings[0] = WideString(FormGfx->chosen_number);
        }
    }
}

void __fastcall TFormItemEdit::editStringGridEnter(TObject *Sender)
{
    data_form->SelectFirstRow();
}

// Shows the chosen type and refills the data grid with that type's fields.
void __fastcall TFormItemEdit::comboEditItemTypeChange(TObject *Sender)
{
    captionCurrentType->Caption = comboEditItemType->Text;
    DataComboItem *type_item = type_column->GetComboItem((short)comboEditItemType->ItemIndex);
    labelEditGridVisible->Caption =
        IntToStr(data_form->FillGrid(editStringGrid, type_item, (char)open_operation, comboEditItemType->Text));
}

void __fastcall TFormItemEdit::spinEditDuraEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

void __fastcall TFormItemEdit::comboEditItemTypeEnter(TObject *Sender)
{
    ((TElComboBox *)Sender)->Tag = 0;
    FormMain->combo_text = ((TElComboBox *)Sender)->Text;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormItemEdit)

void __fastcall TFormItemEdit::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses short or EOF names, then writes the fields and the data grid into a new or the chosen item row.
void __fastcall TFormItemEdit::buttonOkClick(TObject *Sender)
{
    TElWideStrings *name_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *type_cells;
    TElWideStrings *subtype_cells;
    TElWideStrings *unique_cells;
    TElWideStrings *weight_cells;
    TElWideStrings *size_cells;
    DataComboItem *combo_item;
    TElWideStrings *data_cells;
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
    FormMain->items_tab->grid_ui->is_ready = false;
    if (!save_mode)
    {
        row[0] = (int)grid->Items->AddItem(0);
        for (int i = 0; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count - 1; i++)
            ((TElTreeItem *)row[0])->ColumnText->Add(WideString(""));
    }
    if (save_mode == 1)
        row[0] = (int)grid->Items->Item[record_index];
    int column = 0;
    name_cells = ((TElTreeItem *)row[0])->ColumnText;
    name_cells->Strings[column++] = WideString(data_form->HideZeroText(editName->Text));
    gfx_cells = ((TElTreeItem *)row[0])->ColumnText;
    gfx_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditGfx->Text));
    type_cells = ((TElTreeItem *)row[0])->ColumnText;
    type_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditItemType->Text));
    subtype_cells = ((TElTreeItem *)row[0])->ColumnText;
    subtype_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditSubType->Text));
    unique_cells = ((TElTreeItem *)row[0])->ColumnText;
    unique_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditUnique->Text));
    weight_cells = ((TElTreeItem *)row[0])->ColumnText;
    weight_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditWeight->Text));
    size_cells = ((TElTreeItem *)row[0])->ColumnText;
    size_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditSize->Text));
    for (int column_count = ((TElTreeItem *)row[0])->ColumnText->Count; column < column_count; column++)
    {
        String cell_text(edit_grid->Items->Item[column + 1]->ColumnText->Strings[0]);
        DataGridCol *grid_column = data_form->grid_ui->data_grid->GetColumn(column + 1);
        DataGridCol *sub_column = grid_column->FindSubColumn(comboEditItemType->Text);
        // A list field of the item's type is stored as its entry number, not its text.
        if (sub_column && sub_column->kind == ColumnKind_SmartCombo)
        {
            combo_item = sub_column->FindComboItem(cell_text);
            if (combo_item)
                cell_text = data_form->NumberText(combo_item->index);
            else
                cell_text = data_form->HideZeroText(cell_text);
        }
        data_cells = ((TElTreeItem *)row[0])->ColumnText;
        data_cells->Strings[column] = WideString(cell_text);
    }
    FormMain->items_tab->grid_ui->is_ready = true;
    FormMain->items_tab->grid_ui->ShowMessage("Item ( " + record_id + " ) " + editName->Text + " " + ResultText());
    ((TElTreeItem *)row[0])->Text = WideString(record_id);
    ((TElTreeItem *)row[0])->Hidden = false;
    grid->IsUpdating = false;
    ((TCustomForm *)this)->Close();
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormItemEdit)

void __fastcall TFormItemEdit::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another item already has it.
DEFINE_FORM_CHECK_DUPLICATE_NAME(TFormItemEdit)

void __fastcall TFormItemEdit::editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item, TElHeaderSection *Section,
    TElFString &Text, THintWindow *HintWindow, TPoint &MousePos, bool &DoShowHint)
{
    DoShowHint = false;
}

// Settles the typed type text on a list entry, by partial name or number, or an out-of-bounds tag.
DEFINE_COMBO_EDIT_ITEM_TYPE_EXIT(TFormItemEdit)

void TFormItemEdit::TypeChosen()
{
    comboEditItemTypeChange((TObject *)comboEditItemType);
}

// Escape restores the combo's text from entry, Enter settles it; typing drops the list open.
DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_DOWN(TFormItemEdit)

// Enter opens the list on the first presses, then settles the remembered text; other keys remember it.
DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_UP(TFormItemEdit)

// Escape puts back the cell value kept for quick save, when quick save is on.
DEFINE_EDIT_STRING_GRID_KEY_UP(TFormItemEdit)

// Forgets the cell value kept for quick save.
void __fastcall TFormItemEdit::editStringGridItemFocused(TObject *Sender)
{
    FormMain->edit_backup = "";
    FormMain->edit_grid = 0;
    FormMain->edit_row = 0;
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_GFX_EXIT(TFormItemEdit)
