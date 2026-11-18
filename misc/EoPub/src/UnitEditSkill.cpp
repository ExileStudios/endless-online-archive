#include <vcl.h>
#pragma hdrstop

#include "UnitEditSkill.h"
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

TFormSkillEdit *FormSkillEdit;

__fastcall TFormSkillEdit::TFormSkillEdit(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    data_form_made = false;
    combos_filled = false;
    font_color_saved = false;
}

// Applies the theme and its corner tile, keeps the name box's font colour once, and clears the quick-save cell.
void __fastcall TFormSkillEdit::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Color = (TColor)0x525252;
        imageCorner->Picture->Bitmap = FormMain->imageTile2->Picture->Bitmap;
    }
    else
        imageCorner->Picture->Bitmap = FormMain->imageTile->Picture->Bitmap;
    if (!font_color_saved)
    {
        name_font_color = editName->Font->Color;
        font_color_saved = true;
    }
    FormMain->combo_text = "";
    FormMain->edit_backup = "";
    FormMain->edit_grid = 0;
    FormMain->edit_row = 0;
    editName->SetFocus();
}

DEFINE_FORM_KEY_DOWN(TFormSkillEdit)

// Opens the form over the main window to create, edit or duplicate the skill, with its grid, icon and effect.
void TFormSkillEdit::Open(int operation, int index)
{
    pub_grid = (TCustomElTree *)FormMain->skills_tab->main_grid;
    edit_grid = (TCustomElTree *)editStringGrid;
    imageWarnDupe->Visible = false;
    editName->Hint = String("");
    if (!data_form_made)
    {
        data_form = new DataForm(String("skill"), FormMain->skills_tab->grid_ui, (int)pub_grid, editStringGrid);
        data_form->SetSecondGraphics(FormMain->gfx_vault->skills_library, 101, 3);
        data_form->SetGraphics(FormMain->gfx_vault->icons_library, 101, 1);
        data_form->SetTransparent(true);
        data_form_made = true;
    }
    icon_gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    effect_gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
    name_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    chant_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Chant");
    nature_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Nature");
    type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    target_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target");
    target_type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target Type");
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
    DataComboItem *type_item = type_column->GetComboItem((short)comboEditType->ItemIndex);
    labelEditGridVisible->Caption = IntToStr(data_form->FillGrid(editStringGrid, type_item, operation, ""));
    data_form->ShowGraphic(0, imageIcon, Label11, spinIconGfx->Value);
    imageIcon->Hint =
        FormGfx->GraphicHint(spinIconGfx->Value, (TElTreeStringGrid *)pub_grid, icon_gfx_column->text_index);
    data_form->ShowAnimation(panelImage, imageAnim, Label8, spinEditGfx->Value);
    imageAnim->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, effect_gfx_column->text_index);
    ShowOperation(open_operation);
    ShowModal();
}

// Shows the operation (new, edit or dupe skill) and the current type, and lines up the captions to fit.
void TFormSkillEdit::ShowOperation(int operation)
{
    if (!operation)
        labelOperation->Caption = "new skill";
    if (operation == 1)
        labelOperation->Caption = "edit skill";
    if (operation == 2)
        labelOperation->Caption = "dupe skill";
    captionCurrentType->Caption = comboEditType->Text;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8;
}

// Fills the fields with a new skill's defaults, or with the skill row at the index for an update.
void TFormSkillEdit::LoadFields(int index, unsigned char operation)
{
    if (!operation)
    {
        editName->Text = "NoName";
        editChant->Text = "";
        spinEditGfx->Value = 1;
        spinIconGfx->Value = 1;
        comboEditNature->ItemIndex = 0;
        comboEditType->ItemIndex = 0;
        comboEditTarget->ItemIndex = 0;
        comboEditTarget2->ItemIndex = 0;
        itemNameOriginal->Caption = "New Skill";
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("CREATE " + itemNameOriginalID->Caption);
    }
    if (operation == 1)
    {
        TElTreeItem *row = pub_grid->Items->Item[index];
        editName->Text = row->ColumnText->Strings[name_column->text_index];
        editChant->Text = row->ColumnText->Strings[chant_column->text_index];
        spinEditGfx->Value = data_form->DigitsOnly(row->ColumnText->Strings[effect_gfx_column->text_index]);
        spinIconGfx->Value = data_form->DigitsOnly(row->ColumnText->Strings[icon_gfx_column->text_index]);
        comboEditNature->Text = row->ColumnText->Strings[nature_column->text_index];
        comboEditType->Text = row->ColumnText->Strings[type_column->text_index];
        comboEditTarget->Text = row->ColumnText->Strings[target_column->text_index];
        comboEditTarget2->Text = row->ColumnText->Strings[target_type_column->text_index];
        record_id = row->Text;
        itemNameOriginal->Caption = editName->Text;
        itemNameOriginalID->Caption = "| ID: " + record_id;
        buttonOk->Caption = WideString("UPDATE " + itemNameOriginalID->Caption);
    }
}

// Fills the nature, type, target and target type lists, only the first time.
void TFormSkillEdit::FillCombos()
{
    if (combos_filled)
        return;
    data_form->FillCombo(comboEditNature, "Nature");
    data_form->FillCombo(comboEditType, "Type");
    data_form->FillCombo(comboEditTarget, "Target");
    data_form->FillCombo(comboEditTarget2, "Target Type");
    combos_filled = true;
}

// Takes the ID after the last skill, or with recycling on, the first row with an empty or EOF name.
void TFormSkillEdit::FindNewId()
{
    int row;
    TCustomElTree *grid = pub_grid;
    record_id = (int)(grid->Items->Count + 1);
    buttonOk->Hint = WideString("Create New Skill");
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
            buttonOk->Hint = WideString("Create Recycled Skill");
            return;
        }
    }
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
DEFINE_FORM_FIND_DUPE_ID(TFormSkillEdit)

// Blanks a zero in the edited cell when zeros are hidden.
DEFINE_EDIT_STRING_GRID_ITEM_CHANGE(TFormSkillEdit)

void __fastcall TFormSkillEdit::editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    char edited = data_form->EditCell(Item, SectionIndex, Editor, comboEditType->Text);
}

void __fastcall TFormSkillEdit::editStringGridEnter(TObject *Sender)
{
    data_form->SelectFirstRow();
}

// Opens the graphics picker on the icon library and takes the chosen icon.
void __fastcall TFormSkillEdit::imageIconClick(TObject *Sender)
{
    DataGridCol *icon_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    FormGfx->SetLibrary(FormMain->gfx_vault->icons_library, 101, 1, FormMain->skills_tab->grid_ui->icon_first,
        (TElTreeStringGrid *)pub_grid, icon_grid_column->text_index);
    FormGfx->ShowCaptions(this, "Icon Gfx", editName->Text, spinIconGfx->Value);
    spinIconGfx->Value = StrToInt(FormGfx->chosen_number);
    data_form->ShowGraphic(0, imageIcon, Label11, spinIconGfx->Value);
    imageIcon->Hint =
        FormGfx->GraphicHint(spinIconGfx->Value, (TElTreeStringGrid *)pub_grid, icon_grid_column->text_index);
}

// Opens the graphics picker on the effect library and takes the chosen effect animation.
void __fastcall TFormSkillEdit::imageAnimClick(TObject *Sender)
{
    DataGridCol *effect_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
    FormGfx->SetLibrary(FormMain->gfx_vault->skills_library, 101, 3, FormMain->gfx_vault->skill_graphic_count,
        (TElTreeStringGrid *)pub_grid, effect_grid_column->text_index);
    FormGfx->SetDrawMode(true);
    FormGfx->ShowCaptions(this, "Effect Gfx", editName->Text, spinEditGfx->Value);
    spinEditGfx->Value = StrToInt(FormGfx->chosen_number);
    data_form->ShowAnimation(panelImage, imageAnim, Label8, spinEditGfx->Value);
    imageAnim->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, effect_grid_column->text_index);
}

// Plays the effect animation of the number in the box, with its hint.
void __fastcall TFormSkillEdit::spinEditGfxChange(TObject *Sender)
{
    if (String(spinEditGfx->Text) == "")
        return;
    data_form->ShowAnimation(panelImage, imageAnim, Label8, spinEditGfx->Value);
    imageAnim->Hint =
        FormGfx->GraphicHint(spinEditGfx->Value, (TElTreeStringGrid *)pub_grid, effect_gfx_column->text_index);
}

// Shows the icon of the number in the box, with its hint.
void __fastcall TFormSkillEdit::spinIconGfxChange(TObject *Sender)
{
    if (String(spinIconGfx->Text) == "")
        return;
    data_form->ShowGraphic(0, imageIcon, Label11, spinIconGfx->Value);
    imageIcon->Hint =
        FormGfx->GraphicHint(spinIconGfx->Value, (TElTreeStringGrid *)pub_grid, icon_gfx_column->text_index);
}

// Shows the chosen type and refills the data grid with that type's fields.
void __fastcall TFormSkillEdit::comboEditTypeChange(TObject *Sender)
{
    captionCurrentType->Caption = comboEditType->Text;
    DataComboItem *type_item = type_column->GetComboItem((short)comboEditType->ItemIndex);
    labelEditGridVisible->Caption =
        IntToStr(data_form->FillGrid(editStringGrid, type_item, (char)open_operation, comboEditType->Text));
}

void __fastcall TFormSkillEdit::spinEditGfxEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

void __fastcall TFormSkillEdit::comboEditNatureEnter(TObject *Sender)
{
    ((TElComboBox *)Sender)->Tag = 0;
    FormMain->combo_text = ((TElComboBox *)Sender)->Text;
}

// The word for how the save went: created, duplicated, recycled or updated.
DEFINE_RESULT_TEXT(TFormSkillEdit)

void __fastcall TFormSkillEdit::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Refuses short or EOF names, then writes the fields and the data grid into a new or the chosen skill row.
void __fastcall TFormSkillEdit::buttonOkClick(TObject *Sender)
{
    TElWideStrings *name_cells;
    TElWideStrings *icon_gfx_cells;
    TElWideStrings *effect_gfx_cells;
    TElWideStrings *chant_cells;
    TElWideStrings *nature_cells;
    TElWideStrings *type_cells;
    TElWideStrings *target_cells;
    TElWideStrings *target_type_cells;
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
    FormMain->skills_tab->grid_ui->is_ready = false;
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
    icon_gfx_cells = ((TElTreeItem *)row[0])->ColumnText;
    icon_gfx_cells->Strings[column++] = WideString(data_form->HideZeroText(spinIconGfx->Text));
    effect_gfx_cells = ((TElTreeItem *)row[0])->ColumnText;
    effect_gfx_cells->Strings[column++] = WideString(data_form->HideZeroText(spinEditGfx->Text));
    chant_cells = ((TElTreeItem *)row[0])->ColumnText;
    chant_cells->Strings[column++] = WideString(data_form->HideZeroText(editChant->Text));
    nature_cells = ((TElTreeItem *)row[0])->ColumnText;
    nature_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditNature->Text));
    type_cells = ((TElTreeItem *)row[0])->ColumnText;
    type_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditType->Text));
    target_cells = ((TElTreeItem *)row[0])->ColumnText;
    target_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditTarget->Text));
    target_type_cells = ((TElTreeItem *)row[0])->ColumnText;
    target_type_cells->Strings[column++] = WideString(data_form->HideZeroText(comboEditTarget2->Text));
    for (int column_count = ((TElTreeItem *)row[0])->ColumnText->Count; column < column_count; column++)
    {
        String cell_text(edit_grid->Items->Item[column + 1]->ColumnText->Strings[0]);
        TElWideStrings *data_cells = ((TElTreeItem *)row[0])->ColumnText;
        data_cells->Strings[column] = WideString(cell_text);
    }
    FormMain->skills_tab->grid_ui->is_ready = true;
    FormMain->skills_tab->grid_ui->ShowMessage("Skill ( " + record_id + " ) " + editName->Text + " " + ResultText());
    ((TElTreeItem *)row[0])->Text = WideString(record_id);
    ((TElTreeItem *)row[0])->Hidden = false;
    grid->IsUpdating = false;
    ((TCustomForm *)this)->Close();
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
DEFINE_EDIT_NAME_EXIT(TFormSkillEdit)

void __fastcall TFormSkillEdit::editNameChange(TObject *Sender)
{
    if (Visible)
        CheckDuplicateName();
}

// Shows the name in yellow with a warning when another skill already has it.
DEFINE_FORM_CHECK_DUPLICATE_NAME(TFormSkillEdit)

void __fastcall TFormSkillEdit::editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item,
    TElHeaderSection *Section, TElFString &Text, THintWindow *HintWindow, TPoint &MousePos, bool &DoShowHint)
{
    DoShowHint = false;
}

// Settles the typed combo text on a list entry, by partial name or number, or an out-of-bounds tag.
void __fastcall TFormSkillEdit::comboEditNatureExit(TObject *Sender)
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

void TFormSkillEdit::TypeChosen()
{
    comboEditTypeChange((TObject *)comboEditType);
}

// Escape restores the combo's text from entry, Enter settles it; typing drops the list open.
void __fastcall TFormSkillEdit::comboEditNatureKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
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
        comboEditNatureExit(Sender);
        FormMain->combo_text = ((TElComboBox *)Sender)->Text;
        ((TElComboBox *)Sender)->SelectAll();
    }
    if (!Key && !((TElComboBox *)Sender)->DroppedDown)
        ((TElComboBox *)Sender)->DropDown();
}

// Enter opens the list on the first presses, then settles the remembered text; other keys remember it.
void __fastcall TFormSkillEdit::comboEditNatureKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
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
            comboEditNatureExit(Sender);
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
DEFINE_EDIT_STRING_GRID_KEY_UP(TFormSkillEdit)

// Forgets the cell value kept for quick save.
void __fastcall TFormSkillEdit::editStringGridItemFocused(TObject *Sender)
{
    FormMain->edit_backup = "";
    FormMain->edit_grid = 0;
    FormMain->edit_row = 0;
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_GFX_EXIT(TFormSkillEdit)

void __fastcall TFormSkillEdit::editChantExit(TObject *Sender)
{
    String chant = ((TEdit *)Sender)->Text;
    chant = chant.Trim();
    ((TEdit *)Sender)->Text = chant;
}
