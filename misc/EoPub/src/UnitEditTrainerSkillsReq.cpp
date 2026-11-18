#include <vcl.h>
#pragma hdrstop

#include "UnitEditTrainerSkillsReq.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include "UnitSelectSkill.h"
#include "UnitEditTrainerSkills.h"
#include "Datagrid.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElTreeSpinEdit"
#pragma link "ElMenus"
#pragma resource "*.dfm"

TFormTrainerEditSkillsReq *FormTrainerEditSkillsReq;

__fastcall TFormTrainerEditSkillsReq::TFormTrainerEditSkillsReq(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_NPC_EDIT_DROPS_FORM_SHOW(TFormTrainerEditSkillsReq)

// Opens the required skills of the trainer's skill row over the skills form, titled with its name and ID.
void TFormTrainerEditSkillsReq::Open(TFormTrainerEditSkills *skills_form, String skill_id, String skill_name,
    TElTreeItem *row)
{
    pub_grid = FormMain->shops_tab->main_grid;
    learn_row = row;
    itemNameOriginal->Caption = skill_name;
    itemNameOriginalID->Caption = "| ID: " + skill_id;
    hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = (skills_form->Width - Width) / 2 + skills_form->Left;
    Top = (skills_form->Height - Height) / 2 + skills_form->Top;
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
DEFINE_FORM_LAYOUT_CAPTIONS(TFormTrainerEditSkillsReq)

// Shows the required skill count out of four in the list's caption.
void TFormTrainerEditSkillsReq::ShowCount()
{
    labelGridCaption->Caption = "Pre Skill Requirements";
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + " / 4";
}

DEFINE_RENUMBER(TFormTrainerEditSkillsReq)

// Fills the list with the skill row's required skills that are set.
void TFormTrainerEditSkillsReq::FillGrid()
{
    TElWideStrings *skill_id_cells;
    TElWideStrings *skill_name_cells;
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    unsigned char first_column = 7;
    unsigned char requirement_count = 4;
    for (int i = 0; i < requirement_count; i++)
    {
        String skill_id(learn_row->ColumnText->Strings[first_column + i]);
        if (skill_id.Length() <= 0)
            continue;
        if (FormMain->NumberOr(skill_id, 0) < 1)
            continue;
        String skill_name = "[SKILL OOB]";
        TElTreeItem *skill_row = FindSkillRow(skill_id);
        if (skill_row)
        {
            DataGridCol *name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            skill_name = skill_row->ColumnText->Strings[name_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        skill_id_cells = row->ColumnText;
        skill_id_cells->Add(WideString(skill_id));
        skill_name_cells = row->ColumnText;
        skill_name_cells->Add(WideString(skill_name));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

DEFINE_FIND_SKILL_ROW(TFormTrainerEditSkillsReq)

// Shows the skill icon in the image, as large as the panel and the kind allow.
void TFormTrainerEditSkillsReq::ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id)
{
    HINSTANCE__ *library;
    Graphics::TBitmap *bitmap;
    int resource_id;
    TImage *shown_image = image;
    int resource_step = 1;
    int first_resource = 101;
    TPanel *image_panel = panel;
    library = FormMain->gfx_vault->icons_library;
    if (id < 1)
        id = 1;
    resource_id = first_resource + (id - 1) * resource_step;
    shown_image->Picture->Bitmap->TransparentColor = clBlack;
    shown_image->Picture->Bitmap->Transparent = true;
    try
    {
        shown_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id);
        bitmap = shown_image->Picture->Bitmap;
        bitmap->Width = shown_image->Picture->Bitmap->Width / 2;
    }
    catch (...)
    {
        if (image_panel)
            image_panel->Caption = "(n/a)";
        shown_image->Visible = false;
        return;
    }
    shown_image->Visible = true;
    if (3 * shown_image->Picture->Bitmap->Width < image_panel->Width &&
        3 * shown_image->Picture->Bitmap->Height < image_panel->Height && kind > 2)
    {
        shown_image->Width = 3 * shown_image->Picture->Bitmap->Width;
        shown_image->Height = 3 * shown_image->Picture->Bitmap->Height;
    }
    else if (2 * shown_image->Picture->Bitmap->Width < image_panel->Width &&
        2 * shown_image->Picture->Bitmap->Height < image_panel->Height && kind > 1)
    {
        shown_image->Width = 2 * shown_image->Picture->Bitmap->Width;
        shown_image->Height = 2 * shown_image->Picture->Bitmap->Height;
    }
    else if (image_panel->Width > shown_image->Picture->Bitmap->Width * 1.5 &&
        image_panel->Height > shown_image->Picture->Bitmap->Height * 1.5 && kind > 0)
    {
        shown_image->Width = (int)(shown_image->Picture->Bitmap->Width * 1.5);
        shown_image->Height = (int)(shown_image->Picture->Bitmap->Height * 1.5);
    }
    else if (image_panel->Width > shown_image->Picture->Bitmap->Width * 1.2 &&
        image_panel->Height > shown_image->Picture->Bitmap->Height * 1.2 && kind > 0)
    {
        shown_image->Width = (int)(shown_image->Picture->Bitmap->Width * 1.2);
        shown_image->Height = (int)(shown_image->Picture->Bitmap->Height * 1.2);
    }
    else
    {
        shown_image->Width = shown_image->Picture->Bitmap->Width;
        shown_image->Height = shown_image->Picture->Bitmap->Height;
    }
    if (image_panel)
    {
        image_panel->Caption = "";
        shown_image->Left = (image_panel->Width - shown_image->Width) / 2;
        shown_image->Top = (image_panel->Height - shown_image->Height) / 2;
        if (shown_image->Top < 0)
            shown_image->Top = shown_image->Top / 3;
    }
}

void __fastcall TFormTrainerEditSkillsReq::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
}

void __fastcall TFormTrainerEditSkillsReq::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowSkillHint(X, Y);
}

// Shows the hint with the name and icon of the required skill under the mouse.
void TFormTrainerEditSkillsReq::ShowSkillHint(int x, int y)
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
    TElTreeItem *skill_row = FindSkillRow(row->ColumnText->Strings[0]);
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
    String skill_type = "item file incomplete";
    String skill_name = "[SKILL OOB]";
    String skill_id(row->ColumnText->Strings[0]);
    String gfx_text = "64001";
    if (skill_row)
    {
        skill_type = skill_row->ColumnText->Strings[type_column->text_index];
        skill_name = skill_row->ColumnText->Strings[name_column->text_index];
        skill_id = skill_row->Text;
        if (skill_row->ColumnText->Strings[gfx_column->text_index].Length() > 0)
            gfx_text = skill_row->ColumnText->Strings[gfx_column->text_index];
    }
    hintCaption->Caption = skill_name;
    hintIDCaption->Caption = "| ID: " + skill_id;
    hintSubString->Caption = skill_type;
    hintIDCaption->Left = hintCaption->Left + hintCaption->Width + 8;
    hintPanel->Width = 337;
    int needed_width = hintIDCaption->Left + hintIDCaption->Width + 24;
    if (needed_width > hintPanel->Width)
        hintPanel->Width = needed_width;
    ShowIcon(hintImage, hintImagePanel, 3, StrToInt(gfx_text));
    hintPanel->Left = hint_x;
    hintPanel->Top = hint_y;
    hintPanel->Visible = true;
}

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormTrainerEditSkillsReq)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormTrainerEditSkillsReq)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormTrainerEditSkillsReq)

void __fastcall TFormTrainerEditSkillsReq::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then writes the required skills and their count into the skill row.
void __fastcall TFormTrainerEditSkillsReq::buttonOkClick(TObject *Sender)
{
    unsigned char i;
    TElWideStrings *count_cells;
    TElWideStrings *requirement_cells;
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
    unsigned char first_column = 7;
    unsigned char requirement_count = 4;
    for (i = 0; (int)dropGrid->Items->Count > i; i++)
        learn_row->ColumnText->Strings[first_column + i] = dropGrid->Items->Item[i]->ColumnText->Strings[0];
    // Slots past the last row are cleared to zero.
    for (; i < requirement_count; i++)
    {
        requirement_cells = learn_row->ColumnText;
        requirement_cells->Strings[first_column + i] = WideString("0");
    }
    count_cells = learn_row->ColumnText;
    count_cells->Strings[5] = WideString(IntToStr(dropGrid->Items->Count));
    ModalResult = mrOk;
}

DEFINE_DROP_GRID_ENTER(TFormTrainerEditSkillsReq)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormTrainerEditSkillsReq)

// Delete removes the selected row and Ctrl+N adds a new one.
DEFINE_DROP_GRID_KEY_UP(TFormTrainerEditSkillsReq)

// Clears every required skill once the warning is confirmed.
void __fastcall TFormTrainerEditSkillsReq::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Pre Skill List";
    String message = "The entire skill list will be cleared, are you sure?";
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

// Adds a required skill picked in the skill picker; four at most.
void __fastcall TFormTrainerEditSkillsReq::buttonNewShopCraftClick(TObject *Sender)
{
    TElTreeItem *row;
    if (dropGrid->Items->Count >= 4)
    {
        FormWarning->ShowWarning(this, "Full - Cannot add",
            "The pub file can only store a maximum of 4 requirements (skills)");
        return;
    }
    FormSelectSkill->Open(this, "add : required Skill", -1);
    if (FormSelectSkill->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *cells = row->ColumnText;
        cells->Add(WideString(IntToStr(FormSelectSkill->selected_id)));
        cells = row->ColumnText;
        cells->Add(WideString(FormSelectSkill->selected_name));
        cells = row->ColumnText;
        cells->Add(WideString("1"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Gives each column its editor: an ID spin, the skill picker, or a spin.
void __fastcall TFormTrainerEditSkillsReq::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item,
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
            FormSelectSkill->Open(this, "select : required Skill",
                StrToInt(dropGrid->Selected->ColumnText->Strings[0]));
            if (FormSelectSkill->ModalResult == mrOk)
            {
                TElWideStrings *skill_id_cells = dropGrid->Selected->ColumnText;
                skill_id_cells->Strings[0] = WideString(IntToStr(FormSelectSkill->selected_id));
                TElWideStrings *skill_name_cells = dropGrid->Selected->ColumnText;
                skill_name_cells->Strings[1] = WideString(FormSelectSkill->selected_name);
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

// Fills the skill name for a typed ID; refuses an ID with no skill.
void __fastcall TFormTrainerEditSkillsReq::ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted,
    bool &DefaultConversion)
{
    if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Text.Length() <= 0)
        ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value = 0;
    if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag > 0)
    {
        if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag < ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value)
            ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value = ((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag;
    }
    if (FormMain->config->GetMouseQuickSave())
        Accepted = true;
    if (dropGrid->Selected == NULL)
        Accepted = false;
    if (Accepted)
    {
        if (edit_column == 1)
        {
            TElTreeItem *skill_row = FindSkillRow((int)((TElTreeInplaceSpinEdit *)Sender)->Editor->Value);
            if (!skill_row)
            {
                Accepted = false;
                return;
            }
            DataGridCol *name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
            dropGrid->Selected->ColumnText->Strings[1] = skill_row->ColumnText->Strings[name_column->text_index];
            hint_row = -1;
            hintPanel->Visible = false;
        }
    }
}

// Blanks the zero cells of the changed row when zeros are hidden.
DEFINE_DROP_GRID_ITEM_CHANGE(TFormTrainerEditSkillsReq)
