#include <vcl.h>
#pragma hdrstop

#include "Datagrid.h"
#include "UnitMain.h"
#include "Datacomboitem.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Datalinkitem.h"

#pragma package(smart_init)

DataGrid::DataGrid(String name, char show_all, String index_name)
{
    row_grid = 0;
    detail_grid = 0;
    table_name = name;
    view_all = show_all;
    has_out_of_bounds = false;
    has_link_out_of_bounds = false;
    index_column_name = index_name;
    index_column = 0;
    edit_row = 0;
    edit_detail_item = 0;
    store_row = 0;
}

DataGridCol *DataGrid::GetIndexColumn()
{
    if (index_column)
        return index_column;
    index_column = FindColumn(index_column_name);
    return index_column;
}

DataGrid::~DataGrid()
{
}

void DataGrid::SetGrids(TElTreeStringGrid *main_grid, TElTreeStringGrid *sub_grid)
{
    row_grid = main_grid;
    detail_grid = sub_grid;
}

// Adds a column at the end; its text index is one less than its place.
DataGridCol *DataGrid::AddColumn(String column_name, String column_caption, short column_width, char column_kind,
    String column_hint, int column_min, int column_max, char column_group, char column_alignment, char column_format)
{
    DataGridCol *column =
        new DataGridCol(column_name, column_caption, column_width, column_kind, column_hint, column_min, column_max,
            column_group, column_alignment, column_format);
    column->index = columns.size();
    column->text_index = column->index - 1;
    columns.insert(columns.end(), column);
    return column;
}

// Builds the grid's header from the columns: caption, hint, width, visibility, alignment and sort type.
void DataGrid::BuildHeader(TElTreeStringGrid *grid)
{
    TElHeaderSection *section;
    grid->HeaderSections->Clear();
    short count = columns.size();
    for (int i = 0; count > i; i++)
    {
        DataGridCol *column = columns[i];
        section = grid->HeaderSections->AddSection();
        section->Text = WideString(column->header_text);
        section->Hint = WideString(ColumnWidth(column));
        section->MinWidth = column->width;
        section->Width = column->width;
        section->MaxWidth = column->width + 100;
        if (view_all)
            section->Visible = true;
        else if (!column->group)
            section->Visible = true;
        else
            section->Visible = false;
        if (column->alignment == ColumnAlign_Center)
            section->Alignment = hsaCenter;
        if (column->alignment == ColumnAlign_Right)
            section->Alignment = hsaRight;
        if (column->kind == ColumnKind_Number || column->kind == ColumnKind_Index || column->kind == ColumnKind_Graphic)
            section->FieldType = sftNumber;
        else
            section->FieldType = sftText;
        if (column->kind == ColumnKind_Index)
        {
            section->MaxWidth = column->width;
            section->Resizable = false;
            section->AllowClick = true;
            section->ClickSelect = true;
            section->ShowSortMark = true;
            section->Editable = true;
        }
        else
        {
            section->MaxWidth = column->width + 100;
            section->Resizable = true;
            section->AllowClick = true;
            section->ClickSelect = true;
            section->ShowSortMark = true;
            section->Editable = true;
        }
    }
}

// Adds a line to the grid for each column, with its caption, its hint and an empty value.
void DataGrid::AddEmptyRow(TElTreeStringGrid *grid)
{
    TElTreeItem *item;
    TElWideStrings *cells;
    short count = columns.size();
    for (int i = 0; i < count; i++)
    {
        DataGridCol *column = columns[i];
        item = grid->Items->AddItem(0);
        item->Text = WideString(column->caption_text);
        item->Hint = WideString(ColumnWidth(column));
        item->Tag = column->index;
        cells = item->ColumnText;
        cells->Add(WideString(""));
    }
}

String DataGrid::ZeroIfEmpty(String text)
{
    if (text.Length() < 1)
        return "0";
    return text;
}

// The column's hint: its caption, the column it varies, its hint text, its kind and its highest value.
String DataGrid::ColumnWidth(DataGridCol *column)
{
    String text = "";
    text = column->caption_text;
    if (column->parent)
        text += "\nColumn: " + column->parent->header_text;
    if (column->hint_text != "")
        text += "\n" + column->hint_text;
    // The column's kind tells the user how its cells are edited.
    if (column->kind == ColumnKind_Combo)
        text += "\nType: selectable";
    if (column->kind == ColumnKind_SmartCombo)
        text += "\nType: smart-selectable";
    if (column->kind == ColumnKind_Link)
        text += "\nType: interlink";
    if (column->kind == ColumnKind_LinkSource)
        text += "\nType: inputsource";
    if (column->kind == ColumnKind_Graphic)
        text += "\nType: gfx-selectable";
    if (column->kind == ColumnKind_PaperdollGraphic)
        text += "\nType: gfx-selectable";
    if (column->kind >= ColumnKind_FirstEditorForm)
        text += "\nType: editor";
    if (column->kind == ColumnKind_Number || column->kind == ColumnKind_Text || column->kind == ColumnKind_LinkSource)
    {
        if (column->kind == ColumnKind_Number)
            text += "\nType: number";
        if (column->kind == ColumnKind_Text)
            text += "\nType: input";
        text += "\nmax: " + IntToStr((unsigned int)column->max_value);
        if (column->kind == ColumnKind_Text)
            text += " ch.";
    }
    return text;
}

// The text of the column's choice for the value; [OOB-n] when past its list, which marks the column.
String DataGrid::CellText(String column_name, int value)
{
    if (value < 0)
        return "";
    for (int i = 0; i < columns.size(); i++)
    {
        DataGridCol *column = columns[i];
        if (!column)
            return "[404]";
        if (!(column->header_text != column_name))
        {
            int item_count = column->ComboItemCount();
            if ((short)item_count <= value)
            {
                column->is_out_of_bounds = true;
                has_out_of_bounds = true;
                return "[OOB-" + IntToStr(value) + "]";
            }
            DataComboItem *item = column->GetComboItem(value);
            if (!item)
                return "[NIL]";
            return item->text;
        }
    }
    return "[CRC]";
}

// The linked grid's text for the value, - for 0; [IL-OOB] when past the linked rows, which marks the column.
String DataGrid::CellTextOrDash(String column_name, int value)
{
    if (value < 0)
        return "";
    if (!value)
        return "-";
    for (int i = 0; i < columns.size(); i++)
    {
        DataGridCol *column = columns[i];
        if (!column)
            return "[404]";
        if (!(column->header_text != column_name))
        {
            if (column->LinkCount() <= 0)
                return "[NOLNK]";
            DataLinkItem *link = column->GetLink(0);
            TElTreeStringGrid *grid = link->grid;
            short section = link->FindColumnIndex();
            if (section < 0)
                return "[NOLNK]";
            int row_count = grid->Items->Count;
            // A value past the linked grid's last row is marked out of bounds and remembered.
            if (grid->Items->Count <= value - 1)
            {
                has_link_out_of_bounds = true;
                column->is_link_out_of_bounds = true;
                if (column->largest_link_value < value)
                    column->largest_link_value = value;
                return "[IL-OOB]";
            }
            return grid->Items->Item[value - 1]->ColumnText->Strings[section];
        }
    }
    return "[LCRC]";
}

// The index of the first column whose name holds the text in any case; -1 when none does.
int DataGrid::ColumnIndexOf(String name)
{
    for (vector<DataGridCol *>::iterator entry = columns.begin(); entry != columns.end(); entry++)
    {
        String column_name = LowerCase((*entry)->header_text);
        name = LowerCase(name);
        if (column_name.Pos(name) > 0)
            return (*entry)->index;
    }
    return -1;
}

// The column whose name is the text in any case; none when no column matches.
DataGridCol *DataGrid::FindColumn(String name)
{
    for (vector<DataGridCol *>::iterator entry = columns.begin(); entry != columns.end(); entry++)
    {
        String column_name = LowerCase((*entry)->header_text);
        name = LowerCase(name);
        if (column_name.Pos(name) > 0)
        {
            if (name.Length() >= column_name.Length())
                return *entry;
        }
    }
    return 0;
}

DataGridCol *DataGrid::GetColumn(int index)
{
    if (index < 0)
        return 0;
    if (columns.size() <= index)
        return 0;
    return columns[index];
}

int DataGrid::ColumnCount()
{
    return columns.size();
}

DataGridView *DataGrid::AddView(String name)
{
    DataGridView *view = new DataGridView(name);
    views.insert(views.end(), view);
    return view;
}

DataGridView *DataGrid::GetView(int index)
{
    if (index < 0)
        return 0;
    if (views.size() <= index)
        return 0;
    return views[index];
}

int DataGrid::ViewCount()
{
    return views.size();
}

void Datagrid_UnusedLiterals1()
{
    ": ";
    "0";
    ": ";
    "00";
    ": ";
    "000";
    ": ";
    "0000";
    ": ";
    "err: ";
}

// Fills the detail grid from the selected row; a smart column's choice text in the row becomes its number.
bool DataGrid::ShowRowDetails()
{
    int section;
    TElTreeItem *caption_item;
    TElWideStrings *id_cells;
    TElWideStrings *row_cells;
    TElWideStrings *detail_cells;
    if (!row_grid)
        return false;
    if (!detail_grid)
        return false;
    if (row_grid->Selected == NULL)
        return false;
    TElTreeItem *row = row_grid->Selected;
    // The row an edit began in is shown rather than the one now selected.
    if (edit_row)
    {
        row = edit_row;
        edit_row = 0;
    }
    String row_id(row->Text);
    int section_count = row_grid->HeaderSections->Count;
    for (section = 0; section < section_count; section++)
    {
        if (section > row_grid->Selected->ColumnText->Count)
            return true;
        if (section > (short)ColumnCount())
            return true;
        DataGridCol *column = GetColumn(section);
        if (FormMain->config->GetSmartEditors())
        {
            DataGridCol *index_grid_column = GetIndexColumn();
            if (index_grid_column)
            {
                String index_text(row_grid->Selected->ColumnText->Strings[index_grid_column->text_index]);
                DataGridCol *sub_column = column->FindSubColumn(index_text);
                if (sub_column)
                    column = sub_column;
            }
        }
        caption_item = detail_grid->Items->Item[section];
        caption_item->Text = WideString(column->caption_text);
        detail_grid->Items->Item[section]->Hint = WideString(ColumnWidth(column));
        TElTreeItem *detail_item = detail_grid->Items->Item[section];
        if (!section)
        {
            String id_text(row->Text);
            if (!FormMain->config->GetShowZeros() && id_text == "0" && column->text_format != ColumnFormat_KeepZero)
                id_text = "";
            id_cells = detail_item->ColumnText;
            id_cells->Strings[0] = WideString(id_text);
            row->Text = WideString(id_text);
        }
        else
        {
            String cell_text(row->ColumnText->Strings[section - 1]);
            if (FormMain->config->GetSmartEditors() && column->kind == ColumnKind_SmartCombo)
            {
                if (cell_text == "")
                    cell_text = "0";
                DataComboItem *combo_item = column->FindComboItem(cell_text);
                if (combo_item)
                {
                    cell_text = combo_item->text;
                    row->ColumnText->Strings[section - 1] = WideString(IntToStr(combo_item->index));
                }
            }
            if (!FormMain->config->GetShowZeros() && String(row->ColumnText->Strings[section - 1]) == "0" &&
                column->text_format != ColumnFormat_KeepZero)
            {
                row_cells = row->ColumnText;
                row_cells->Strings[section - 1] = WideString("");
            }
            if (!FormMain->config->GetShowZeros() && cell_text == "0" && column->text_format != ColumnFormat_KeepZero)
                cell_text = "";
            detail_cells = detail_item->ColumnText;
            detail_cells->Strings[0] = WideString(cell_text);
        }
    }
    return true;
}

// Fills the detail grid from the selected row, showing the choice text of each smart column.
bool DataGrid::RefreshRowDetails()
{
    int section;
    TElTreeItem *caption_item;
    TElWideStrings *id_cells;
    TElWideStrings *detail_cells;
    DataComboItem *combo_item;
    if (!row_grid)
        return false;
    if (!detail_grid)
        return false;
    if (row_grid->Selected == NULL)
        return false;
    TElTreeItem *row = row_grid->Selected;
    // A row just edited is shown in place of the selected one, once.
    if (edit_row)
    {
        row = edit_row;
        edit_row = 0;
    }
    String row_id(row->Text);
    int section_count = row_grid->HeaderSections->Count;
    for (section = 0; section < section_count; section++)
    {
        if (section > row_grid->Selected->ColumnText->Count)
            return true;
        if (section > (short)ColumnCount())
            return true;
        DataGridCol *column = GetColumn(section);
        if (FormMain->config->GetSmartEditors())
        {
            DataGridCol *index_grid_column = GetIndexColumn();
            if (index_grid_column)
            {
                String index_text(row_grid->Selected->ColumnText->Strings[index_grid_column->text_index]);
                DataGridCol *sub_column = column->FindSubColumn(index_text);
                if (sub_column)
                    column = sub_column;
            }
        }
        caption_item = detail_grid->Items->Item[section];
        caption_item->Text = WideString(column->caption_text);
        detail_grid->Items->Item[section]->Hint = WideString(ColumnWidth(column));
        TElTreeItem *detail_item = detail_grid->Items->Item[section];
        if (!section)
        {
            String id_text(row->Text);
            if (!FormMain->config->GetShowZeros() && id_text == "0" && column->text_format != ColumnFormat_KeepZero)
                id_text = "";
            id_cells = detail_item->ColumnText;
            id_cells->Strings[0] = WideString(id_text);
            row->Text = WideString(id_text);
        }
        else
        {
            String cell_text(row->ColumnText->Strings[section - 1]);
            if (FormMain->config->GetSmartEditors() && column->kind == ColumnKind_SmartCombo)
            {
                combo_item = column->GetComboItem(DigitsOnly(cell_text));
                if (combo_item)
                    cell_text = combo_item->text;
            }
            if (!FormMain->config->GetShowZeros() && cell_text == "0" && column->text_format != ColumnFormat_KeepZero)
                cell_text = "";
            detail_cells = detail_item->ColumnText;
            detail_cells->Strings[0] = WideString(cell_text);
        }
    }
    return true;
}

// Writes the edited value of the detail grid into its cell of the row, as a number for a smart column.
bool DataGrid::StoreDetailValue()
{
    TElWideStrings *detail_cells;
    TElWideStrings *row_cells;
    DataComboItem *combo_item;
    if (!row_grid)
        return false;
    if (!detail_grid)
        return false;
    if (detail_grid->Selected == NULL)
        return false;
    if (row_grid->Selected == NULL)
        return false;
    TElTreeItem *row = row_grid->Selected;
    TElTreeItem *detail_item = detail_grid->Selected;
    if (edit_row)
    {
        row = edit_row;
        edit_row = 0;
    }
    // The value goes back to the row its edit began in.
    if (store_row)
    {
        row = store_row;
        store_row = 0;
    }
    if (edit_detail_item)
    {
        detail_item = edit_detail_item;
        edit_detail_item = 0;
    }
    String row_id(row->Text);
    String caption(detail_item->Text);
    int column_index = detail_item->Tag;
    String detail_text(detail_item->ColumnText->Strings[0]);
    String stored_text(detail_item->ColumnText->Strings[0]);
    DataGridCol *column = GetColumn(column_index);
    DataGridCol *index_grid_column = GetIndexColumn();
    if (index_grid_column)
    {
        String index_text(detail_grid->Items->Item[index_grid_column->index]->ColumnText->Strings[0]);
        DataGridCol *sub_column = column->FindSubColumn(index_text);
        if (sub_column && sub_column->kind == ColumnKind_SmartCombo)
        {
            column = sub_column;
            combo_item = column->FindComboItem(detail_text);
            if (combo_item)
                stored_text = DigitsOnly(combo_item->index);
        }
    }
    if (!FormMain->config->GetShowZeros() && detail_text == "0" && column->text_format != ColumnFormat_KeepZero)
    {
        detail_cells = detail_item->ColumnText;
        detail_cells->Strings[0] = WideString("");
    }
    if (!FormMain->config->GetShowZeros() && stored_text == "0" && column->text_format != ColumnFormat_KeepZero)
        stored_text = "";
    if (!column_index)
        row->Text = WideString(stored_text);
    else
    {
        row_cells = row->ColumnText;
        row_cells->Strings[column_index - 1] = WideString(stored_text);
    }
    return true;
}

// The names of the columns holding a value past their list, each after the grid's name.
String DataGrid::VisibleColumnNames()
{
    String names = "";
    for (int i = 0; i < (short)ColumnCount(); i++)
    {
        DataGridCol *column = GetColumn(i);
        if (column->is_out_of_bounds)
            names += table_name + "." + column->header_text + "  ";
    }
    return names;
}

DataGridCol *DataGrid::GetKeyColumn()
{
    DataGridCol *column;
    for (int i = 0; i < (short)ColumnCount(); i++)
    {
        column = GetColumn(i);
        if (column->is_link_out_of_bounds)
            return column;
    }
    return 0;
}

// The digits of the text as a number; 0 when it has none.
DEFINE_DIGITS_ONLY(DataGrid)
