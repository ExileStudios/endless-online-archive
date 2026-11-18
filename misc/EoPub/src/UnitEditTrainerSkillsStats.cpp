#include <vcl.h>
#pragma hdrstop

#include "UnitEditTrainerSkillsStats.h"
#include "UnitMain.h"
#include "UnitEditTrainerSkills.h"

#pragma package(smart_init)
#pragma link "ElSpin"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormTrainerEditSkillsStats *FormTrainerEditSkillsStats;

__fastcall TFormTrainerEditSkillsStats::TFormTrainerEditSkillsStats(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen and focuses the strength box.
void __fastcall TFormTrainerEditSkillsStats::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Color = (TColor)0x525252;
    }
    if ((char)((int __fastcall (*)(int))FindDynaInst((void *)spinEditStr, -68))((int)spinEditStr))
        spinEditStr->SetFocus();
}

// Shows the stat requirements of the skill row in a dialog centred on the skills form.
void TFormTrainerEditSkillsStats::Open(TFormTrainerEditSkills *skills_form, String skill_id, String skill_name,
    TElTreeItem *row)
{
    learn_row = row;
    itemNameOriginal->Caption = skill_name;
    itemNameOriginalID->Caption = "| ID: " + skill_id;
    LoadStats();
    Left = (skills_form->Width - Width) / 2 + skills_form->Left;
    Top = (skills_form->Height - Height) / 2 + skills_form->Top;
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form's caption.
DEFINE_FORM_LAYOUT_CAPTIONS(TFormTrainerEditSkillsStats)

// Reads the six stat requirements from the skill row into their boxes.
void TFormTrainerEditSkillsStats::LoadStats()
{
    int column = 11;
    String stat;
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditStr->Text = WideString("0");
    spinEditStr->Value = StrToInt(stat);
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditWis->Text = WideString("0");
    spinEditWis->Value = StrToInt(stat);
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditInt->Text = WideString("0");
    spinEditInt->Value = StrToInt(stat);
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditAgi->Text = WideString("0");
    spinEditAgi->Value = StrToInt(stat);
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditCon->Text = WideString("0");
    spinEditCon->Value = StrToInt(stat);
    stat = FormMain->TrimLeadingZeros(learn_row->ColumnText->Strings[column++]);
    spinEditCha->Text = WideString("0");
    spinEditCha->Value = StrToInt(stat);
}

void __fastcall TFormTrainerEditSkillsStats::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Writes the six stats back into the skill row, with their short summary in the stats column.
void __fastcall TFormTrainerEditSkillsStats::buttonOkClick(TObject *Sender)
{
    int column = 11;
    TElWideStrings *strength_cells = learn_row->ColumnText;
    strength_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditStr->Text));
    TElWideStrings *wisdom_cells = learn_row->ColumnText;
    wisdom_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditWis->Text));
    TElWideStrings *intelligence_cells = learn_row->ColumnText;
    intelligence_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditInt->Text));
    TElWideStrings *agility_cells = learn_row->ColumnText;
    agility_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditAgi->Text));
    TElWideStrings *constitution_cells = learn_row->ColumnText;
    constitution_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditCon->Text));
    TElWideStrings *charisma_cells = learn_row->ColumnText;
    charisma_cells->Strings[column++] = WideString(FormMain->TrimLeadingZeros(spinEditCha->Text));
    TElWideStrings *summary_cells = learn_row->ColumnText;
    summary_cells->Strings[6] = WideString(StatsSummary());
    ModalResult = mrOk;
}

// The non-zero stats as short text like 5s, 2w; three at most, then +.. for more.
String TFormTrainerEditSkillsStats::StatsSummary()
{
    String summary = "";
    int count = 0;
    if (spinEditStr->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditStr->Value) + "s, ";
        count++;
    }
    if (spinEditWis->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditWis->Value) + "w, ";
        count++;
    }
    if (spinEditInt->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditInt->Value) + "i, ";
        count++;
    }
    if (spinEditAgi->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditAgi->Value) + "a, ";
        count++;
    }
    if (spinEditCon->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditCon->Value) + "c, ";
        count++;
    }
    if (spinEditCha->Value > 0)
    {
        if (count < 3)
            summary += IntToStr(spinEditCha->Value) + "ch, ";
        count++;
    }
    if (summary.Length() > 2)
        summary.Delete(summary.Length() - 1, 2);
    // Only three stats fit in the summary; more are marked +..
    if (count > 3)
        summary += " +..";
    return summary;
}

void __fastcall TFormTrainerEditSkillsStats::spinEditStrKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE)
        ((TCustomForm *)this)->Close();
}

void __fastcall TFormTrainerEditSkillsStats::spinEditStrEnter(TObject *Sender)
{
    ((TElSpinEdit *)Sender)->SelectAll();
}

// Drops the number's leading zeros and caps it at the most the box allows.
DEFINE_SPIN_EDIT_STR_EXIT(TFormTrainerEditSkillsStats)
