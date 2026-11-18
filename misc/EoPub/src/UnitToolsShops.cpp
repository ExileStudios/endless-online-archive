#include <vcl.h>
#pragma hdrstop

#include "UnitToolsShops.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"

#pragma package(smart_init)
#pragma link "ElPgCtl"
#pragma link "ElCombos"
#pragma link "ElCheckCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormToolsShops *FormToolsShops;

__fastcall TFormToolsShops::TFormToolsShops(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Themes the form and lists the shop grid's number and choice columns that can be cleaned.
void __fastcall TFormToolsShops::FormShow(TObject *Sender)
{
    DataGridCol *column;
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    ElPageControl1->ActivePageIndex = 0;
    comboCleanColumn->SetFocus();
    comboCleanColumn->Items->Clear();
    comboCleanColumn->DropDownCount = 20;
    for (int column_index = 1; column_index < (short)FormMain->shops_tab->grid_ui->data_grid->ColumnCount();
        column_index++)
    {
        column = FormMain->shops_tab->grid_ui->data_grid->GetColumn(column_index);
        if (column->kind == ColumnKind_Number || column->kind == ColumnKind_Combo)
            comboCleanColumn->Items->Add(WideString(column->header_text));
    }
    checkCleanerOOB->Checked = false;
    checkCleanerAll->Checked = false;
}

void TFormToolsShops::ShowTools(TFormMain *parent)
{
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
    Top = Top + 70;
    ShowModal();
}

void __fastcall TFormToolsShops::buttonCancelClick(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

// Sets the column to zero, or its first choice, in out-of-bounds rows or all rows; returns how many.
int TFormToolsShops::CleanColumn(char out_of_bounds, char all_values)
{
    TElWideStrings *combo_cells;
    TElWideStrings *number_cells;
    if (!out_of_bounds && !all_values)
        return 0;
    cleaned_count = 0;
    TElTreeStringGrid *grid = FormMain->shops_tab->main_grid;
    DataGridCol *column = FormMain->shops_tab->grid_ui->data_grid->FindColumn(comboCleanColumn->Text);
    if (!grid || !column)
        return 0;
    DataComboItem *default_item = column->GetComboItem(0);
    grid->IsUpdating = true;
    FormMain->shops_tab->grid_ui->is_ready = false;
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        TElTreeItem *row = grid->Items->Item[row_index];
        if ((row->ColumnText->Strings[column->text_index].Pos("[OOB-") == 0) > 0)
        {
            if (!all_values)
                continue;
        }
        if (column->kind == ColumnKind_Combo)
        {
            if (default_item)
            {
                combo_cells = row->ColumnText;
                combo_cells->Strings[column->text_index] = WideString(default_item->text);
            }
        }
        else
        {
            number_cells = row->ColumnText;
            number_cells->Strings[column->text_index] = WideString("0");
        }
        cleaned_count++;
    }
    FormMain->shops_tab->grid_ui->is_ready = true;
    grid->IsUpdating = false;
    return cleaned_count;
}

// Cleans the chosen column and tells how many rows were set back, or that nothing was found.
DEFINE_BUTTON_OK_CLICK(TFormToolsShops)

void __fastcall TFormToolsShops::comboCleanColumnKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}

// Cleaning all values needs expert mode; without it the box is cleared with a warning.
DEFINE_CHECK_CLEANER_ALL_CLICK(TFormToolsShops)
