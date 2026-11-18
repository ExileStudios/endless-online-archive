#include <vcl.h>
#pragma hdrstop

#include "UnitToolsItems.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "Datacomboitem.h"
#include "Datagrid.h"

#pragma package(smart_init)
#pragma link "ElPgCtl"
#pragma link "ElSpin"
#pragma link "ElCheckCtl"
#pragma link "ElPopBtn"
#pragma link "ElCombos"
#pragma resource "*.dfm"

TFormToolsItems *FormToolsItems;

__fastcall TFormToolsItems::TFormToolsItems(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Themes the form, sets up the uniquifier with suggested steps and lists the columns that can be cleaned.
void __fastcall TFormToolsItems::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    ElPageControl1->ActivePageIndex = 0;
    comboCleanColumn->SetFocus();
    String hint = "decide uniqueness based on # of attached stats ( str , int , wis , agi , con , cha )\n";
    hint = hint + "note: each present stat will count as [1] , the value of the stat is ignored (e.g 20STR )";
    ElCheckBox1->Hint = WideString(hint);
    ElCheckBox1->Checked = false;
    ElCheckBox2->Checked = true;
    comboItemType->ItemIndex = 0;
    comboTargetItems->ItemIndex = 0;
    SuggestPowerSteps("Dmg-lo");
    UniquifyItems(true);
    comboCleanColumn->Items->Clear();
    comboCleanColumn->DropDownCount = 20;
    for (int column_index = 1; column_index < (short)FormMain->items_tab->grid_ui->data_grid->ColumnCount();
        column_index++)
    {
        DataGridCol *column = FormMain->items_tab->grid_ui->data_grid->GetColumn(column_index);
        if (column->kind == ColumnKind_Number || column->kind == ColumnKind_Combo)
            comboCleanColumn->Items->Add(WideString(column->header_text));
    }
    checkCleanerOOB->Checked = false;
    checkCleanerAll->Checked = false;
}

// Shows the tools a little below the middle of the main window.
void TFormToolsItems::Open(TFormMain *main)
{
    Left = (main->Width - Width) / 2 + main->Left;
    Top = (main->Height - Height) / 2 + main->Top;
    Top = Top + 70;
    ShowModal();
}

// Gives weapons or armours the highest rarity their stats and power earn, or only counts them per rarity.
void TFormToolsItems::UniquifyItems(char count_only)
{
    int type_id;
    if (!Visible)
        return;
    TElTreeStringGrid *grid = FormMain->items_tab->main_grid;
    str_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Str");
    int_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Int");
    wis_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Wis");
    agi_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Agi");
    con_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Con");
    cha_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Cha");
    damage_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Dmg-lo");
    armor_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Armor");
    DataGridCol *unique_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Unique");
    int unique_index = unique_column->text_index;
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    int type_index = type_column->text_index;
    int lowest_type = 10;
    int highest_type = 10;
    if (comboItemType->ItemIndex == 1)
    {
        lowest_type = 11;
        highest_type = 21;
    }
    int highest_rarity = 10;
    if (comboTargetItems->ItemIndex == 1)
        highest_rarity = 0;
    if (comboTargetItems->ItemIndex == 2)
        highest_rarity = 3;
    changed_count = 0;
    unique_count = 0;
    legendary_count = 0;
    rare_count = 0;
    normal_count = 0;
    if (!count_only)
    {
        grid->IsUpdating = true;
        FormMain->items_tab->grid_ui->is_ready = false;
    }
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        TElTreeItem *row = grid->Items->Item[row_index];
        String type_text(row->ColumnText->Strings[type_index]);
        DataComboItem *type_item = type_column->FindComboItem(type_text);
        if (!type_item)
            continue;
        type_id = type_item->index;
        if (type_id < lowest_type || type_id > highest_type)
            continue;
        String unique_text(row->ColumnText->Strings[unique_index]);
        int rarity = unique_column->FindComboItem(unique_text)->index;
        if (rarity > highest_rarity)
            continue;
        // The highest rarity the item earns wins; lower ones are tried only when it fails.
        if (TryRarity(count_only, row, unique_column, ElCheckBox1, spinEditStats4, ElCheckBox2, spinEditDmg4, 3))
            continue;
        if (TryRarity(count_only, row, unique_column, ElCheckBox1, spinEditStats3, ElCheckBox2, spinEditDmg3, 2))
            continue;
        if (TryRarity(count_only, row, unique_column, ElCheckBox1, spinEditStats2, ElCheckBox2, spinEditDmg2, 1))
            continue;
        if (TryRarity(count_only, row, unique_column, ElCheckBox1, spinEditStats1, ElCheckBox2, spinEditDmg1, 0))
            continue;
    }
    if (!count_only)
    {
        FormMain->items_tab->grid_ui->is_ready = true;
        grid->IsUpdating = false;
    }
    if (count_only)
    {
        labelUniqueTargets1->Caption = IntToStr(normal_count);
        labelUniqueTargets2->Caption = IntToStr(rare_count);
        labelUniqueTargets3->Caption = IntToStr(legendary_count);
        labelUniqueTargets4->Caption = IntToStr(unique_count);
    }
}

// Whether the row has the stats and power the rarity asks; then counts it or sets its rarity.
bool TFormToolsItems::TryRarity(char count_only, TElTreeItem *row, DataGridCol *unique_column, TElCheckBox *stats_check,
    TElSpinEdit *stats_needed, TElCheckBox *power_check, TElSpinEdit *power_needed, unsigned char rarity)
{
    TElWideStrings *cells;
    String item_name(row->ColumnText->Strings[0]);
    if (stats_check->Checked)
    {
        int stat_count = 0;
        String str_text(row->ColumnText->Strings[str_column->text_index]);
        if (row->ColumnText->Strings[str_column->text_index].Length() > 0)
            stat_count++;
        if (row->ColumnText->Strings[int_column->text_index].Length() > 0)
            stat_count++;
        if (row->ColumnText->Strings[wis_column->text_index].Length() > 0)
            stat_count++;
        if (row->ColumnText->Strings[agi_column->text_index].Length() > 0)
            stat_count++;
        if (row->ColumnText->Strings[con_column->text_index].Length() > 0)
            stat_count++;
        if (row->ColumnText->Strings[cha_column->text_index].Length() > 0)
            stat_count++;
        if (stats_needed->Value > stat_count)
            return false;
    }
    if (power_check->Checked)
    {
        int power = 0;
        if (!comboItemType->ItemIndex)
        {
            if (row->ColumnText->Strings[damage_column->text_index].Length() > 0)
                power = FormMain->NumberOr(row->ColumnText->Strings[damage_column->text_index], 0);
        }
        if (comboItemType->ItemIndex == 1)
        {
            if (row->ColumnText->Strings[armor_column->text_index].Length() > 0)
                power = FormMain->NumberOr(row->ColumnText->Strings[armor_column->text_index], 0);
        }
        if (power_needed->Value > power)
            return false;
    }
    if (count_only)
    {
        String rarity_text = "";
        if (rarity == 3)
            unique_count++;
        if (rarity == 2)
            legendary_count++;
        if (rarity == 1)
            rare_count++;
        if (!rarity)
            normal_count++;
    }
    else
    {
        DataComboItem *rarity_item = unique_column->GetComboItem(rarity);
        if (rarity_item)
        {
            cells = row->ColumnText;
            cells->Strings[unique_column->text_index] = WideString(rarity_item->text);
            changed_count++;
        }
    }
    return true;
}

// Suggests the power steps of the rarities from the spread of the column's values.
void TFormToolsItems::SuggestPowerSteps(String column_name)
{
    int number;
    int cell_value;
    TElTreeStringGrid *grid;
    int column_index;
    int highest_value;
    int three_quarters;
    grid = FormMain->items_tab->main_grid;
    DataGridCol *found_column = FormMain->items_tab->grid_ui->data_grid->FindColumn(column_name);
    column_index = found_column->text_index;
    highest_value = 0;
    three_quarters = 0;
    for (int i = 0; i < grid->Items->Count; i++)
    {
        String cell_text(grid->Items->Item[i]->ColumnText->Strings[column_index]);
        if (cell_text.Length() < 1)
            continue;
        three_quarters++;
        number = FormMain->NumberOr(cell_text, 0);
        if (number > highest_value)
            highest_value = number;
    }
    // The steps aim to cover three in four of the filled cells.
    three_quarters = three_quarters / 4;
    three_quarters = 3 * three_quarters;
    for (int tenth = 1; tenth <= 10; tenth++)
    {
        int count_below = 0;
        int limit = highest_value / 10 * tenth;
        int highest_below = 0;
        for (int j = 0; j < grid->Items->Count; j++)
        {
            String row_text(grid->Items->Item[j]->ColumnText->Strings[column_index]);
            if (row_text.Length() < 1)
                continue;
            cell_value = StrToInt(row_text);
            if (cell_value > limit)
                continue;
            count_below++;
            if (cell_value > highest_below)
                highest_below = cell_value;
        }
        if (count_below > three_quarters || tenth >= 9)
        {
            highest_value = highest_below;
            break;
        }
    }
    int power_step = highest_value / 3;
    if (power_step <= 0)
        power_step = 1;
    spinEditDmg1->Value = 0;
    spinEditDmg2->Value = power_step;
    spinEditDmg3->Value = 2 * power_step;
    spinEditDmg4->Value = 3 * power_step;
    spinEditStats1->Value = 0;
    spinEditStats2->Value = 1;
    spinEditStats3->Value = 2;
    spinEditStats4->Value = 3;
}

// Switches the uniquifier between weapon damage and armour, with new suggested steps.
void __fastcall TFormToolsItems::comboItemTypeChange(TObject *Sender)
{
    if (!((TElComboBox *)Sender)->ItemIndex)
    {
        Label22->Caption = "dmg.lo >";
        Label23->Caption = "dmg.lo >";
        Label24->Caption = "dmg.lo >";
        Label25->Caption = "dmg.lo >";
        SuggestPowerSteps("Dmg-lo");
    }
    if (((TElComboBox *)Sender)->ItemIndex == 1)
    {
        Label22->Caption = "Armor >";
        Label23->Caption = "Armor >";
        Label24->Caption = "Armor >";
        Label25->Caption = "Armor >";
        SuggestPowerSteps("Armor");
    }
    UniquifyItems(true);
}

void __fastcall TFormToolsItems::comboTargetItemsChange(TObject *Sender)
{
    UniquifyItems(true);
}

void __fastcall TFormToolsItems::spinEditDmg1Change(TObject *Sender)
{
    UniquifyItems(true);
}

void __fastcall TFormToolsItems::ElCheckBox1Click(TObject *Sender)
{
    UniquifyItems(true);
}

void __fastcall TFormToolsItems::buttonCancelClick(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

// Sets the column to zero, or its first choice, in out-of-bounds rows or all rows; returns how many.
int TFormToolsItems::CleanColumn(char out_of_bounds, char all_values)
{
    TElWideStrings *combo_cells;
    TElWideStrings *number_cells;
    if (!out_of_bounds && !all_values)
        return 0;
    changed_count = 0;
    TElTreeStringGrid *grid = FormMain->items_tab->main_grid;
    DataGridCol *column = FormMain->items_tab->grid_ui->data_grid->FindColumn(comboCleanColumn->Text);
    if (!grid || !column)
        return 0;
    DataComboItem *default_item = column->GetComboItem(0);
    grid->IsUpdating = true;
    FormMain->items_tab->grid_ui->is_ready = false;
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        TElTreeItem *row = grid->Items->Item[row_index];
        if ((row->ColumnText->Strings[column->text_index].Pos("[OOB-") == 0) > 0)
        {
            if (!all_values)
                continue;
        }
        // A list column falls back to its first entry, any other column to zero.
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
            number_cells->Strings[column->text_index] = WideString(FormMain->items_tab->NumberText(0));
        }
        changed_count++;
    }
    FormMain->items_tab->grid_ui->is_ready = true;
    grid->IsUpdating = false;
    return changed_count;
}

// Runs the uniquifier or the cleaner on the open page and tells how many items changed.
void __fastcall TFormToolsItems::buttonOkClick(TObject *Sender)
{
    if (ElPageControl1->ActivePageIndex == tabUniquifier->PageIndex)
    {
        if (!ElCheckBox1->Checked && !ElCheckBox2->Checked)
            return;
        UniquifyItems(false);
        String uniquified_message =
            IntToStr(FormToolsItems->changed_count) +
                " items uniqified based on your input. Please check the results before saving.";
        FormWarning->ShowWarning(this, " Items Uniqified", uniquified_message);
    }
    if (ElPageControl1->ActivePageIndex == tabCleaner->PageIndex)
    {
        if (!CleanColumn(checkCleanerOOB->Checked, checkCleanerAll->Checked))
        {
            FormWarning->ShowWarning(this, "Nothing To Clean",
                "No fields were found to be cleaned based on your current column and target.");
            return;
        }
        String cleaned_message =
            IntToStr(changed_count) + " items set to default (zero). Please check the results before saving.";
        FormWarning->ShowWarning(this, " Column Wiped", cleaned_message);
    }
    ModalResult = mrOk;
}

void __fastcall TFormToolsItems::ElCheckBox1KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}

// Cleaning all values needs expert mode; without it the box is cleared with a warning.
DEFINE_CHECK_CLEANER_ALL_CLICK(TFormToolsItems)
