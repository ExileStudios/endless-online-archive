#include <vcl.h>
#pragma hdrstop

#include "UnitEditTrainerSkills.h"
#include "UnitMain.h"
#include "UnitWarningDelete.h"
#include "UnitSelectSkill.h"
#include "UnitEditTrainerSkillsReq.h"
#include "UnitEditTrainerSkillsStats.h"
#include "Datagrid.h"
#include "Learnvalue.h"
#include "Learnvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma link "ElTreeSpinEdit"
#pragma link "ElTreeComboBox"
#pragma resource "*.dfm"

TFormTrainerEditSkills *FormTrainerEditSkills;

__fastcall TFormTrainerEditSkills::TFormTrainerEditSkills(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the theme and its shop picture, clears the quick-save cell and selects the first row.
DEFINE_T_FORM_SHOP_EDIT_ITEMS_FORM_SHOW(TFormTrainerEditSkills)

// Opens the skills of the trainer at the index, titled with the trainer's name and ID.
void TFormTrainerEditSkills::Open(int index)
{
    pub_grid = FormMain->trainers_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    name_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    trainerNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    trainerNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    hint_row = -1;
    requirement_hint_row = -1;
    stats_hint_row = -1;
    rows_deleted = false;
    FillGrid();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    LayoutCaptions();
    ShowModal();
}

void TFormTrainerEditSkills::LayoutCaptions()
{
    trainerNameOriginalID->Left = trainerNameOriginal->Left + trainerNameOriginal->Width + 14;
    Caption = " " + labelOperation->Caption + " : " + trainerNameOriginal->Caption;
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8;
    trainerNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8;
    trainerNameOriginalID->Left = trainerNameOriginal->Left + trainerNameOriginal->Width + 8;
    buttonOk->Caption = WideString("UPDATE | ID: " + IntToStr(record_index + 1));
}

// Shows the row count in the list's caption, or the empty-list panel when there are none.
void TFormTrainerEditSkills::ShowCount()
{
    labelGridCaption->Caption = "Learnable Skills";
    panelNoAdds->Visible = false;
    if (dropGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(dropGrid->Items->Count) + "x";
}

DEFINE_RENUMBER(TFormTrainerEditSkills)

// Fills the list with the trainer's skills: price, level, class, required skills and stat requirements.
void TFormTrainerEditSkills::FillGrid()
{
    TElWideStrings *charisma_cells;
    TElWideStrings *skill_id_cells;
    TElWideStrings *skill_name_cells;
    TElWideStrings *level_cells;
    TElWideStrings *price_cells;
    TElWideStrings *class_cells;
    TElWideStrings *skill_count_cells;
    TElWideStrings *stats_cells;
    TElWideStrings *first_skill_cells;
    TElWideStrings *second_skill_cells;
    TElWideStrings *third_skill_cells;
    TElWideStrings *fourth_skill_cells;
    TElWideStrings *strength_cells;
    TElWideStrings *wisdom_cells;
    TElWideStrings *intelligence_cells;
    TElWideStrings *agility_cells;
    TElWideStrings *constitution_cells;
    LearnValue *trainer;
    int i;
    trainer = FormMain->trainers_tab->trainer_values->Insert(record_index);
    dropGrid->IsUpdating = true;
    dropGrid->Items->Clear();
    for (i = 0; trainer->skills.size() > i; i++)
    {
        LearnItemVal learn_item = trainer->skills[i];
        String skill_name = "[SKILL OOB]";
        TElTreeItem *skill_row = FindSkillRow(IntToStr(learn_item.id));
        if (skill_row)
        {
            DataGridCol *name_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
            skill_name = skill_row->ColumnText->Strings[name_grid_column->text_index];
        }
        TElTreeItem *row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        skill_id_cells = row->ColumnText;
        skill_id_cells->Add(WideString(IntToStr(learn_item.id)));
        skill_name_cells = row->ColumnText;
        skill_name_cells->Add(WideString(skill_name));
        level_cells = row->ColumnText;
        level_cells->Add(WideString(IntToStr(learn_item.level_requirement)));
        price_cells = row->ColumnText;
        price_cells->Add(WideString(IntToStr(learn_item.price)));
        class_cells = row->ColumnText;
        class_cells->Add(WideString(ClassName(learn_item.class_requirement)));
        skill_count_cells = row->ColumnText;
        skill_count_cells->Add(WideString(SkillRequirementCount(&learn_item)));
        stats_cells = row->ColumnText;
        stats_cells->Add(WideString(StatsSummary(&learn_item)));
        first_skill_cells = row->ColumnText;
        first_skill_cells->Add(WideString(IntToStr(learn_item.skill_requirement_1)));
        second_skill_cells = row->ColumnText;
        second_skill_cells->Add(WideString(IntToStr(learn_item.skill_requirement_2)));
        third_skill_cells = row->ColumnText;
        third_skill_cells->Add(WideString(IntToStr(learn_item.skill_requirement_3)));
        fourth_skill_cells = row->ColumnText;
        fourth_skill_cells->Add(WideString(IntToStr(learn_item.skill_requirement_4)));
        strength_cells = row->ColumnText;
        strength_cells->Add(WideString(IntToStr(learn_item.str_requirement)));
        wisdom_cells = row->ColumnText;
        wisdom_cells->Add(WideString(IntToStr(learn_item.wis_requirement)));
        intelligence_cells = row->ColumnText;
        intelligence_cells->Add(WideString(IntToStr(learn_item.int_requirement)));
        agility_cells = row->ColumnText;
        agility_cells->Add(WideString(IntToStr(learn_item.agi_requirement)));
        constitution_cells = row->ColumnText;
        constitution_cells->Add(WideString(IntToStr(learn_item.con_requirement)));
        charisma_cells = row->ColumnText;
        charisma_cells->Add(WideString(IntToStr(learn_item.cha_requirement)));
    }
    dropGrid->IsUpdating = false;
    ShowCount();
}

String TFormTrainerEditSkills::ClassName(int class_id)
{
    TElTreeStringGrid *class_grid = FormMain->ElTreeStringGrid3;
    if (class_id <= 0 || class_id > class_grid->Items->Count)
        return "-";
    DataGridCol *name_grid_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
    if (!name_grid_column)
        return "-";
    return class_grid->Items->Item[class_id - 1]->ColumnText->Strings[name_grid_column->text_index];
}

// How many required skill slots, from the first, are set.
String TFormTrainerEditSkills::SkillRequirementCount(LearnItemVal *learn_item)
{
    int count = 0;
    if (!learn_item->skill_requirement_1)
        return IntToStr(count);
    count++;
    if (!learn_item->skill_requirement_2)
        return IntToStr(count);
    count++;
    if (!learn_item->skill_requirement_3)
        return IntToStr(count);
    count++;
    if (!learn_item->skill_requirement_4)
        return IntToStr(count);
    count++;
    return IntToStr(count);
}

// The non-zero stat requirements as short text like 5s, 2w; three at most, then +.. for more.
String TFormTrainerEditSkills::StatsSummary(LearnItemVal *learn_item)
{
    String summary = "";
    int count = 0;
    if (learn_item->str_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->str_requirement) + "s, ";
        count++;
    }
    if (learn_item->wis_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->wis_requirement) + "w, ";
        count++;
    }
    if (learn_item->int_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->int_requirement) + "i, ";
        count++;
    }
    if (learn_item->agi_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->agi_requirement) + "a, ";
        count++;
    }
    if (learn_item->con_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->con_requirement) + "c, ";
        count++;
    }
    if (learn_item->cha_requirement > 0)
    {
        if (count < 3)
            summary += IntToStr(learn_item->cha_requirement) + "ch, ";
        count++;
    }
    if (summary.Length() > 2)
        summary.Delete(summary.Length() - 1, 2);
    if (count > 3)
        summary += " +..";
    return summary;
}

DEFINE_FIND_SKILL_ROW(TFormTrainerEditSkills)

// Shows the skill icon in the image, as large as the panel and the kind allow.
void TFormTrainerEditSkills::ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id)
{
    int resource_id;
    Graphics::TBitmap *bitmap;
    HINSTANCE__ *library;
    int image_top;
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
        image_top = shown_image->Top;
        if (image_top < 0)
            shown_image->Top = image_top / 3;
    }
}

// Hides the hints once the mouse is off the list.
void __fastcall TFormTrainerEditSkills::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
    hintPanelReq1->Visible = false;
    hintPanelReq2->Visible = false;
}

void __fastcall TFormTrainerEditSkills::dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    ShowSkillHint(X, Y);
    ShowRequiredSkillsHint(X, Y);
    ShowStatsHint(X, Y);
}

// Shows the hint with the name, type and icon of the row's skill under the mouse.
void TFormTrainerEditSkills::ShowSkillHint(int x, int y)
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
    DataGridCol *name_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *target_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target");
    String skill_type = "skill file incomplete";
    String skill_name = "[SKILL OOB]";
    String skill_id(row->ColumnText->Strings[0]);
    String gfx_text = "64001";
    if (skill_row)
    {
        skill_type = skill_row->ColumnText->Strings[type_column->text_index];
        skill_name = skill_row->ColumnText->Strings[name_grid_column->text_index];
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
    ShowIcon(hintImage, hintImagePanel, 2, StrToInt(gfx_text));
    hintPanel->Left = hint_x;
    hintPanel->Top = hint_y;
    hintPanel->Visible = true;
}

// Over the required skills column, shows a hint listing each required skill's name and icon.
void TFormTrainerEditSkills::ShowRequiredSkillsHint(int x, int y)
{
    int row_index;
    int hint_left =
        dropGrid->HeaderSections->Item[0]->Width + dropGrid->HeaderSections->Item[1]->Width +
            dropGrid->HeaderSections->Item[2]->Width + dropGrid->HeaderSections->Item[3]->Width +
            dropGrid->HeaderSections->Item[4]->Width + dropGrid->HeaderSections->Item[5]->Width + 1;
    int hint_right = hint_left;
    hint_right += dropGrid->HeaderSections->Item[6]->Width;
    int row_count = dropGrid->Items->Count;
    if (hint_left > x)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (x > hint_right)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (y < 0)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (24 * row_count < y)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    int hover_column[1];
    hover_column[0] = 0;
    row_index = y / dropGrid->DefaultRowHeight;
    int visible_row = row_index;
    row_index += dropGrid->TopIndex;
    if (row_index >= row_count)
    {
        hintPanelReq1->Visible = false;
        return;
    }
    if (row_index == requirement_hint_row)
    {
        hintPanelReq1->Visible = true;
        return;
    }
    requirement_hint_row = row_index;
    TElTreeItem *row = dropGrid->Items->Item[row_index];
    TElTreeItem *skill_row = FindSkillRow(row->ColumnText->Strings[0]);
    int hint_x = Panel1->Left;
    int hint_y = Panel1->Top;
    hint_y += dropGrid->Top + dropGrid->DefaultRowHeight + 4;
    hint_y += dropGrid->DefaultRowHeight * visible_row;
    DataGridCol *name_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    labelCaptionIDReq1->Caption = "| ID: " + row->ColumnText->Strings[0];
    labelCaptionReq1->Caption = row->ColumnText->Strings[1];
    labelCaptionIDReq1->Left = labelCaptionReq1->Left + labelCaptionReq1->Width + 6;
    panelReq1->Visible = false;
    panelReq2->Visible = false;
    panelReq3->Visible = false;
    panelReq4->Visible = false;
    int shown_count = 0;
    int widest = 300;
    for (int i = 0; i < 4; i++)
    {
        int first_column = 7;
        String required_id_text(row->ColumnText->Strings[first_column + i]);
        if (!required_id_text.Length())
            continue;
        hint_left = StrToInt(required_id_text);
        if (!hint_left)
            continue;
        String skill_type = "item file incomplete";
        String skill_name = "[SKILL OOB]";
        String gfx_text = "64001";
        TElTreeItem *required_row = FindSkillRow(hint_left);
        if (required_row)
        {
            skill_type = required_row->ColumnText->Strings[type_column->text_index];
            skill_name = required_row->ColumnText->Strings[name_grid_column->text_index];
            required_id_text = required_row->Text;
            if (required_row->ColumnText->Strings[gfx_column->text_index].Length() > 0)
                gfx_text = required_row->ColumnText->Strings[gfx_column->text_index];
        }
        if (!shown_count)
        {
            panelReq1->Visible = true;
            hintCaptionReq1->Caption = skill_name;
            hintIDCaptionReq1->Caption = "| ID: " + required_id_text;
            hintSubStringReq1->Caption = skill_type;
            hintIDCaptionReq1->Left = hintCaptionReq1->Left + hintCaptionReq1->Width + 6;
            ShowIcon(hintImageReq1, hintImagePanelReq1, 1, StrToInt(gfx_text));
            int first_caption_right = hintIDCaptionReq1->Left + hintIDCaptionReq1->Width;
            if (first_caption_right > widest)
                widest = first_caption_right;
        }
        if (shown_count == 1)
        {
            panelReq2->Visible = true;
            hintCaptionReq2->Caption = skill_name;
            hintIDCaptionReq2->Caption = "| ID: " + required_id_text;
            hintSubStringReq2->Caption = skill_type;
            hintIDCaptionReq2->Left = hintCaptionReq2->Left + hintCaptionReq2->Width + 6;
            ShowIcon(hintImageReq2, hintImagePanelReq2, 1, StrToInt(gfx_text));
            int second_caption_right = hintIDCaptionReq2->Left + hintIDCaptionReq2->Width;
            if (second_caption_right > widest)
                widest = second_caption_right;
        }
        if (shown_count == 2)
        {
            panelReq3->Visible = true;
            hintCaptionReq3->Caption = skill_name;
            hintIDCaptionReq3->Caption = "| ID: " + required_id_text;
            hintSubStringReq3->Caption = skill_type;
            hintIDCaptionReq3->Left = hintCaptionReq3->Left + hintCaptionReq3->Width + 6;
            ShowIcon(hintImageReq3, hintImagePanelReq3, 1, StrToInt(gfx_text));
            int third_caption_right = hintIDCaptionReq3->Left + hintIDCaptionReq3->Width;
            if (third_caption_right > widest)
                widest = third_caption_right;
        }
        if (shown_count == 3)
        {
            panelReq4->Visible = true;
            hintCaptionReq4->Caption = skill_name;
            hintIDCaptionReq4->Caption = "| ID: " + required_id_text;
            hintSubStringReq4->Caption = skill_type;
            hintIDCaptionReq4->Left = hintCaptionReq4->Left + hintCaptionReq4->Width + 6;
            ShowIcon(hintImageReq4, hintImagePanelReq4, 1, StrToInt(gfx_text));
            int fourth_caption_right = hintIDCaptionReq4->Left + hintIDCaptionReq4->Width;
            if (fourth_caption_right > widest)
                widest = fourth_caption_right;
        }
        shown_count++;
    }
    hintPanelReq1->Width = widest + 36;
    panelReq1->Width = hintPanelReq1->Width - panelReq1->Left - 2;
    panelReq2->Width = hintPanelReq1->Width - panelReq2->Left - 2;
    panelReq3->Width = hintPanelReq1->Width - panelReq3->Left - 2;
    panelReq4->Width = hintPanelReq1->Width - panelReq4->Left - 2;
    // The hint grows to end below the last panel shown.
    hintPanelReq1->Height = 44;
    if (panelReq1->Visible)
        hintPanelReq1->Height = panelReq1->Top + panelReq1->Height + 10;
    if (panelReq2->Visible)
        hintPanelReq1->Height = panelReq2->Top + panelReq2->Height + 10;
    if (panelReq3->Visible)
        hintPanelReq1->Height = panelReq3->Top + panelReq3->Height + 10;
    if (panelReq4->Visible)
        hintPanelReq1->Height = panelReq4->Top + panelReq4->Height + 10;
    hintReq1AddHint->Visible = false;
    if (shown_count <= 0)
    {
        hintReq1AddHint->Visible = true;
        hintPanelReq1->Height = hintPanelReq1->Height + 32;
    }
    hintPanelReq1->Visible = false;
    hintPanelReq1->Left = hint_x + hint_right - hintPanelReq1->Width - 20;
    hintPanelReq1->Top = hint_y;
    if (hintPanelReq1->Top > 320)
    {
        hintPanelReq1->Top = hintPanelReq1->Top - hintPanelReq1->Height;
        hintPanelReq1->Top = hintPanelReq1->Top - 2 * dropGrid->DefaultRowHeight;
    }
    hintPanelReq1->Visible = true;
}

// Over the stats column, shows a hint with the row's stat requirements.
void TFormTrainerEditSkills::ShowStatsHint(int x, int y)
{
    int row_index;
    int columns_width = dropGrid->HeaderSections->Item[0]->Width;
    columns_width += dropGrid->HeaderSections->Item[1]->Width;
    columns_width += dropGrid->HeaderSections->Item[2]->Width;
    columns_width += dropGrid->HeaderSections->Item[3]->Width;
    columns_width += dropGrid->HeaderSections->Item[4]->Width;
    columns_width += dropGrid->HeaderSections->Item[5]->Width;
    int hint_left = columns_width + dropGrid->HeaderSections->Item[6]->Width + 2;
    int hint_right = hint_left;
    hint_right += dropGrid->HeaderSections->Item[7]->Width;
    int row_count = dropGrid->Items->Count;
    if (hint_left > x)
    {
        hintPanelReq2->Visible = false;
        return;
    }
    if (x > hint_right)
    {
        hintPanelReq2->Visible = false;
        return;
    }
    if (y < 0)
    {
        hintPanelReq2->Visible = false;
        return;
    }
    if (24 * row_count < y)
    {
        hintPanelReq2->Visible = false;
        return;
    }
    row_index = y / dropGrid->DefaultRowHeight;
    int visible_row = row_index;
    row_index += dropGrid->TopIndex;
    if (row_index >= row_count)
    {
        hintPanel->Visible = false;
        return;
    }
    if (row_index == stats_hint_row)
    {
        hintPanelReq2->Visible = true;
        return;
    }
    stats_hint_row = row_index;
    TElTreeItem *row = dropGrid->Items->Item[row_index];
    if (dropGrid->HeaderSections->Item[0]->Width == x)
    {
    }
    if (dropGrid->HeaderSections->Item[0]->Width + dropGrid->HeaderSections->Item[1]->Width == x)
    {
    }
    int hint_x = Panel1->Left;
    int hint_y = Panel1->Top;
    hint_x += 20;
    hint_y += dropGrid->Top + dropGrid->DefaultRowHeight + 4;
    hint_y += dropGrid->DefaultRowHeight * visible_row;
    String skill_name(row->ColumnText->Strings[1]);
    String skill_id(row->ColumnText->Strings[0]);
    hintReq2Caption->Caption = skill_name;
    hintReq2ID->Caption = "| ID: " + skill_id;
    hintReq2ID->Left = hintReq2Caption->Left + hintReq2Caption->Width + 8;
    hintStr->Caption = "-";
    hintWis->Caption = "-";
    hintInt->Caption = "-";
    hintAgi->Caption = "-";
    hintCon->Caption = "-";
    hintCha->Caption = "-";
    int shown_count = 0;
    int column = 11;
    String stat_text(row->ColumnText->Strings[column]);
    column++;
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *strength_panel = hintStr;
        strength_panel->Visible = true;
        strength_panel->Caption = stat_text;
        shown_count++;
    }
    stat_text = row->ColumnText->Strings[column];
    column++;
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *wisdom_panel = hintWis;
        wisdom_panel->Visible = true;
        wisdom_panel->Caption = stat_text;
        shown_count++;
    }
    stat_text = row->ColumnText->Strings[column];
    column++;
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *intelligence_panel = hintInt;
        intelligence_panel->Visible = true;
        intelligence_panel->Caption = stat_text;
        shown_count++;
    }
    stat_text = row->ColumnText->Strings[column];
    column++;
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *agility_panel = hintAgi;
        agility_panel->Visible = true;
        agility_panel->Caption = stat_text;
        shown_count++;
    }
    stat_text = row->ColumnText->Strings[column];
    column++;
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *constitution_panel = hintCon;
        constitution_panel->Visible = true;
        constitution_panel->Caption = stat_text;
        shown_count++;
    }
    stat_text = row->ColumnText->Strings[column];
    if (stat_text.Length() > 0 && stat_text != "0")
    {
        TPanel *charisma_panel = hintCha;
        charisma_panel->Visible = true;
        charisma_panel->Caption = stat_text;
        shown_count++;
    }
    // With no stat requirement the hint only tells how to add one.
    if (!shown_count)
    {
        hintReq2Holder->Visible = false;
        hintReq2AddHint->Visible = true;
        hintPanelReq2->Width = 337;
        hintPanelReq2->Height = 76;
    }
    else
    {
        hintReq2Holder->Visible = true;
        hintReq2AddHint->Visible = true;
        hintPanelReq2->Width = 377;
        hintPanelReq2->Height = 140;
    }
    hintPanelReq2->Left = hint_x + hint_right - hintPanelReq2->Width - 36;
    hintPanelReq2->Top = hint_y;
    hintPanelReq2->Visible = true;
}

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_POPUP_DELETE_CLICK(TFormTrainerEditSkills)

// Swaps the selected row's cells with the previous row's.
DEFINE_POPUP_MOVE_UP_CLICK(TFormTrainerEditSkills)

// Swaps the selected row's cells with the next row's.
DEFINE_POPUP_MOVE_DOWN_CLICK(TFormTrainerEditSkills)

void __fastcall TFormTrainerEditSkills::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then stores the rows as the trainer's skills and their count.
void __fastcall TFormTrainerEditSkills::buttonOkClick(TObject *Sender)
{
    TElTreeItem *row;
    LearnValue *trainer;
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
    trainer = FormMain->trainers_tab->trainer_values->Insert(record_index);
    // The rows replace the trainer's skill list; rows without a skill are left out.
    trainer->skills.clear();
    for (int i = 0; dropGrid->Items->Count > i; i++)
    {
        row = dropGrid->Items->Item[i];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        int skill_id = StrToInt(row->ColumnText->Strings[0]);
        int column = 2;
        int level = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int price = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        String class_name(row->ColumnText->Strings[column++]);
        int class_id = 0;
        DataGridCol *class_name_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
        for (int j = 0; FormMain->classes_tab->main_grid->Items->Count > j; j++)
        {
            TElTreeItem *class_row = FormMain->classes_tab->main_grid->Items->Item[j];
            if (String(class_row->ColumnText->Strings[class_name_column->text_index]) == class_name)
                class_id = StrToInt(class_row->Text);
        }
        column = 7;
        int first_required_skill = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int second_required_skill = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int third_required_skill = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int fourth_required_skill = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int strength = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int wisdom = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int intelligence = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int agility = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int constitution = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        int charisma = FormMain->NumberOr(row->ColumnText->Strings[column++], 0);
        trainer->AddSkill(skill_id, level, class_id, price, first_required_skill, second_required_skill,
            third_required_skill, fourth_required_skill, strength, wisdom, intelligence, agility, constitution,
            charisma);
    }
    DataGridCol *learn_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Learn");
    pub_grid->Selected->ColumnText->Strings[learn_column->text_index] =
        WideString(FormMain->trainers_tab->NumberText(trainer->skills.size()));
    ((TCustomForm *)this)->Close();
}

DEFINE_DROP_GRID_ENTER(TFormTrainerEditSkills)

void __fastcall TFormTrainerEditSkills::ElTreeInplaceComboBox1ValidateResult(TObject *Sender, bool &InputValid)
{
    InputValid = true;
}

// Settles the typed class text on a list entry, by name, partial name or number; a dash when out of range.
void __fastcall TFormTrainerEditSkills::ElTreeInplaceComboBox1AfterOperation(TObject *Sender, bool &Accepted,
    bool &DefaultConversion)
{
    int i;
    if (FormMain->config->GetMouseQuickSave())
        Accepted = true;
    String text(((TElTreeInplaceComboBox *)Sender)->Editor->Text);
    String lower_text = LowerCase(text);
    if (text.Length() > 2)
    {
        for (i = 0; ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Count > i; i++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)Sender)->Editor->Items->Strings[i]) == lower_text)
            {
                ((TElTreeInplaceComboBox *)Sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Strings[i];
                Accepted = true;
                return;
            }
        }
        for (int j = 0; ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Count > j; j++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)Sender)->Editor->Items->Strings[j]).Pos(lower_text) > 0)
            {
                ((TElTreeInplaceComboBox *)Sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Strings[j];
                Accepted = true;
                return;
            }
        }
    }
    int number = FormMain->NumberFromDigits(text);
    if (number >= 0)
    {
        if (number < ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Count)
        {
            ((TElTreeInplaceComboBox *)Sender)->Editor->ItemIndex = number;
            Accepted = true;
            return;
        }
        ((TElTreeInplaceComboBox *)Sender)->Editor->Text = WideString("-");
        Accepted = true;
        return;
    }
    int out_of_bounds = FormMain->OutOfBoundsValue(text);
    if (out_of_bounds >= 0)
    {
        ((TElTreeInplaceComboBox *)Sender)->Editor->Text = WideString("-");
        Accepted = true;
        return;
    }
    for (int k = 0; ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Count > k; k++)
    {
        if (((TElTreeInplaceComboBox *)Sender)->Editor->Text ==
            ((TElTreeInplaceComboBox *)Sender)->Editor->Items->Strings[k])
        {
            Accepted = true;
            return;
        }
    }
    Accepted = false;
}

// Fills the skill name for a typed ID; refuses an ID with no skill.
void __fastcall TFormTrainerEditSkills::ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted,
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
            DataGridCol *name_grid_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
            dropGrid->Selected->ColumnText->Strings[1] = skill_row->ColumnText->Strings[name_grid_column->text_index];
            hint_row = -1;
            hintPanel->Visible = false;
        }
    }
}

// Gives each column its editor: spins, the skill and class pickers, or the requirement and stat forms.
void __fastcall TFormTrainerEditSkills::dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item,
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
            FormSelectSkill->Open(this, "select : craftable Skill",
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
        ElTreeInplaceSpinEdit1->Editor->MinValue = 0;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 9999;
        ElTreeInplaceSpinEdit1->Editor->Tag = 252;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
    if (SectionIndex == 4)
    {
        ElTreeInplaceSpinEdit1->Editor->MinValue = 0;
        ElTreeInplaceSpinEdit1->Editor->MaxValue = 999999999;
        ElTreeInplaceSpinEdit1->Editor->Tag = 0xF71AE3;
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceSpinEdit1;
        return;
    }
    if (SectionIndex == 5)
    {
        ElTreeInplaceComboBox1->Editor->Items->Clear();
        TElWideStrings *combo_items = ElTreeInplaceComboBox1->Editor->Items;
        combo_items->Add(WideString("-"));
        ElTreeInplaceComboBox1->Editor->AutoCompletion = true;
        ElTreeInplaceComboBox1->InitiallyDropped = true;
        ElTreeInplaceComboBox1->Editor->DropDownCount = 28;
        DataGridCol *name_grid_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
        if (!name_grid_column)
            return;
        for (int i = 0; i < FormMain->ElTreeStringGrid3->Items->Count; i++)
        {
            TElTreeItem *class_row = FormMain->ElTreeStringGrid3->Items->Item[i];
            ElTreeInplaceComboBox1->Editor->Items->Add(class_row->ColumnText->Strings[name_grid_column->text_index]);
        }
        Editor = (TElTreeInplaceEditor *)ElTreeInplaceComboBox1;
        return;
    }
    if (SectionIndex == 6)
    {
        Editor = 0;
        if (dropGrid->Selected->ColumnText->Strings[0].Length() > 0)
        {
            // Required skills and stats are edited in their own forms, on this row.
            FormTrainerEditSkillsReq->Open(this, dropGrid->Selected->ColumnText->Strings[0],
                trainerNameOriginal->Caption + " : " + dropGrid->Selected->ColumnText->Strings[1], Item);
            if (FormTrainerEditSkillsReq->ModalResult == mrOk)
            {
                hint_row = -1;
                requirement_hint_row = -1;
                stats_hint_row = -1;
                hintPanel->Visible = false;
                hintPanelReq1->Visible = false;
                hintPanelReq2->Visible = false;
            }
        }
        return;
    }
    if (SectionIndex == 7)
    {
        Editor = 0;
        if (dropGrid->Selected->ColumnText->Strings[0].Length() > 0)
        {
            FormTrainerEditSkillsStats->Open(this, dropGrid->Selected->ColumnText->Strings[0],
                trainerNameOriginal->Caption + " : " + dropGrid->Selected->ColumnText->Strings[1], Item);
            if (FormTrainerEditSkillsStats->ModalResult == mrOk)
            {
                hint_row = -1;
                requirement_hint_row = -1;
                stats_hint_row = -1;
                hintPanel->Visible = false;
                hintPanelReq1->Visible = false;
                hintPanelReq2->Visible = false;
            }
        }
        return;
    }
}

// Clears every trainer skill once the warning is confirmed.
void __fastcall TFormTrainerEditSkills::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Trainer Skills";
    String message = "The entire trainer list will be cleared, are you sure?";
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

// Adds a skill picked in the skill picker, at level 1 for 100 gold with no requirements.
void __fastcall TFormTrainerEditSkills::buttonNewTrainerSkillClick(TObject *Sender)
{
    TElTreeItem *row;
    FormSelectSkill->Open(this, "add : craftable Skill", -1);
    if (FormSelectSkill->ModalResult == mrOk)
    {
        row = dropGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(dropGrid->Items->Count + 1));
        TElWideStrings *skill_id_cells = row->ColumnText;
        skill_id_cells->Add(WideString(IntToStr(FormSelectSkill->selected_id)));
        TElWideStrings *skill_name_cells = row->ColumnText;
        skill_name_cells->Add(WideString(FormSelectSkill->selected_name));
        TElWideStrings *level_cells = row->ColumnText;
        level_cells->Add(WideString("1"));
        TElWideStrings *price_cells = row->ColumnText;
        price_cells->Add(WideString("100"));
        TElWideStrings *class_cells = row->ColumnText;
        class_cells->Add(WideString("-"));
        TElWideStrings *skill_count_cells = row->ColumnText;
        skill_count_cells->Add(WideString(""));
        TElWideStrings *stats_cells = row->ColumnText;
        stats_cells->Add(WideString(""));
        TElWideStrings *first_skill_cells = row->ColumnText;
        first_skill_cells->Add(WideString("0"));
        TElWideStrings *second_skill_cells = row->ColumnText;
        second_skill_cells->Add(WideString("0"));
        TElWideStrings *third_skill_cells = row->ColumnText;
        third_skill_cells->Add(WideString("0"));
        TElWideStrings *fourth_skill_cells = row->ColumnText;
        fourth_skill_cells->Add(WideString("0"));
        TElWideStrings *strength_cells = row->ColumnText;
        strength_cells->Add(WideString("0"));
        TElWideStrings *wisdom_cells = row->ColumnText;
        wisdom_cells->Add(WideString("0"));
        TElWideStrings *intelligence_cells = row->ColumnText;
        intelligence_cells->Add(WideString("0"));
        TElWideStrings *agility_cells = row->ColumnText;
        agility_cells->Add(WideString("0"));
        TElWideStrings *constitution_cells = row->ColumnText;
        constitution_cells->Add(WideString("0"));
        TElWideStrings *charisma_cells = row->ColumnText;
        charisma_cells->Add(WideString("0"));
        ShowCount();
        dropGrid->Selected = row;
        if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -71))((int)dropGrid) &&
            (char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid))
            dropGrid->SetFocus();
    }
}

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_DROP_GRID_KEY_DOWN(TFormTrainerEditSkills)

// Delete removes the selected row and Ctrl+N adds a new one.
void __fastcall TFormTrainerEditSkills::buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        popupDeleteClick((TObject *)this);
    if (Shift.Contains(ssCtrl) && Key == 'N')
        buttonNewTrainerSkillClick((TObject *)this);
}

// Blanks the zero cells of the changed row when zeros are hidden.
DEFINE_DROP_GRID_ITEM_CHANGE(TFormTrainerEditSkills)
