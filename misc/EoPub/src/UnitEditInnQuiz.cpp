#include <vcl.h>
#pragma hdrstop

#include "UnitEditInnQuiz.h"
#include "UnitMain.h"
#include "UnitWarningDelete.h"
#include "Datagrid.h"
#include "Innvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElTreeGrids"
#pragma link "ElTreeStdEditors"
#pragma link "ElMenus"
#pragma resource "*.dfm"

TFormEditInnQuiz *FormEditInnQuiz;

__fastcall TFormEditInnQuiz::TFormEditInnQuiz(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first line.
DEFINE_T_FORM_NPC_EDIT_CHATS_FORM_SHOW(TFormEditInnQuiz)

// Opens the quiz of the inn at the index over the main window, titled with the inn's name and ID.
void TFormEditInnQuiz::Open(int index)
{
    pub_grid = (TCustomElTree *)FormMain->inns_tab->main_grid;
    TElTreeItem *row = pub_grid->Selected;
    name_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Name");
    record_index = index;
    itemNameOriginal->Caption = row->ColumnText->Strings[name_column->text_index];
    itemNameOriginalID->Caption = "| ID: " + IntToStr(index + 1);
    Panel2->Visible = true;
    rows_deleted = false;
    FillGrid();
    ShowCount();
    Left = FormMain->Left + (FormMain->Width - Width) / 2;
    Top = FormMain->Top + (FormMain->Height - Height) / 2;
    LayoutCaptions();
    ShowModal();
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
DEFINE_LAYOUT_CAPTIONS(TFormEditInnQuiz)

// Numbers the lines by their position.
DEFINE_FORM_RENUMBER(TFormEditInnQuiz)

// Fills the list with the inn's quiz questions and answers.
void TFormEditInnQuiz::FillGrid()
{
    TElTreeItem *row;
    TElWideStrings *answer_cells;
    TElWideStrings *question_cells;
    InnValue *inn = FormMain->inns_tab->inn_values->GetInn(record_index);
    lineGrid->IsUpdating = true;
    lineGrid->Items->Clear();
    for (int i = 0; i < inn->questions.size(); i++)
    {
        row = lineGrid->Items->AddItem(0);
        row->Text = WideString(IntToStr(i + 1));
        question_cells = row->ColumnText;
        question_cells->Add(WideString(inn->questions[i].question));
        answer_cells = row->ColumnText;
        answer_cells->Add(WideString(inn->questions[i].answer));
    }
    lineGrid->IsUpdating = false;
    ShowCount();
}

// Shows the line count in the list's caption, or the empty-list panel when there are none.
void TFormEditInnQuiz::ShowCount()
{
    labelGridCaption->Caption = "Quiz lines";
    panelNoAdds->Visible = false;
    if (lineGrid->Items->Count <= 0)
    {
        panelNoAdds->Visible = true;
        return;
    }
    labelGridCaption->Caption = labelGridCaption->Caption + ", " + IntToStr(lineGrid->Items->Count) + "x";
}

// Clears every line once the warning is confirmed.
void __fastcall TFormEditInnQuiz::buttonCleanListClick(TObject *Sender)
{
    String title = "Clear Quiz Lines";
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

// Adds an empty line at the end and selects it.
void __fastcall TFormEditInnQuiz::buttonNewQuizClick(TObject *Sender)
{
    TElTreeItem *row = lineGrid->Items->AddItem(0);
    row->Text = WideString(IntToStr(row->Index + 1));
    TElWideStrings *question_cells = row->ColumnText;
    question_cells->Add(WideString(""));
    TElWideStrings *answer_cells = row->ColumnText;
    answer_cells->Add(WideString(""));
    ShowCount();
    lineGrid->Selected = row;
    if (!(char)((int __fastcall (*)(int))FindDynaInst((void *)lineGrid, -71))((int)lineGrid) &&
        (char)((int __fastcall (*)(int))FindDynaInst((void *)lineGrid, -68))((int)lineGrid))
        lineGrid->SetFocus();
}

// Swaps the selected line's cells with the next line's.
DEFINE_FORM_POPUP_MOVE_DOWN_CLICK(TFormEditInnQuiz)

// Swaps the selected line's cells with the previous line's.
DEFINE_FORM_POPUP_MOVE_UP_CLICK(TFormEditInnQuiz)

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
DEFINE_FORM_POPUP_DELETE_CLICK(TFormEditInnQuiz)

void __fastcall TFormEditInnQuiz::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Confirms deletions when asked to, then stores the filled lines as the inn's quiz and its count.
void __fastcall TFormEditInnQuiz::buttonOkClick(TObject *Sender)
{
    TElTreeItem *row;
    TElTreeStringGrid *grid;
    InnValue *inn;
    grid = lineGrid;
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
    inn = FormMain->inns_tab->inn_values->GetInn(record_index);
    // The rows replace the inn's whole quiz; rows without a question are left out.
    inn->questions.clear();
    for (int i = 0; grid->Items->Count > i; i++)
    {
        row = grid->Items->Item[i];
        if (row->ColumnText->Strings[0].Length() < 1)
            continue;
        InnQuizItem quiz_line;
        quiz_line.question = String(row->ColumnText->Strings[0]);
        quiz_line.answer = String(row->ColumnText->Strings[1]);
        inn->questions.insert(inn->questions.end(), quiz_line);
    }
    DataGridCol *quiz_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Quiz");
    pub_grid->Selected->ColumnText->Strings[quiz_column->text_index] =
        WideString(FormMain->inns_tab->NumberText(inn->questions.size()));
    ((TCustomForm *)this)->Close();
}

DEFINE_LINE_GRID_ENTER(TFormEditInnQuiz)

// Trims the edited text, and accepts it at once when quick save is on.
DEFINE_EL_TREE_INPLACE_EDIT1_AFTER_OPERATION(TFormEditInnQuiz)

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
DEFINE_LINE_GRID_KEY_DOWN(TFormEditInnQuiz)

void __fastcall TFormEditInnQuiz::lineGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_DELETE)
        popupDeleteClick((TObject *)this);
    if (Shift.Contains(ssCtrl) && Key == 'N')
        buttonNewQuizClick((TObject *)this);
}

void __fastcall TFormEditInnQuiz::ElTreeInplaceEdit1BeforeOperation(TObject *Sender, bool &DefaultConversion)
{
    ElTreeInplaceEdit1->Editor->MaxLength = 160;
}
