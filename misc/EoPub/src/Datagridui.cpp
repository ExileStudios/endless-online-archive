#include <vcl.h>
#pragma hdrstop

#include "Datagridui.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitWarningDelete.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datagridview.h"
#include "Datalinkitem.h"
#include <Graphics.hpp>
#include "Inndataadapter.h"
#include "Learndataadapter.h"
#include <mem.h>
#include "Npcdataadapter.h"
#include "Shopdataadapter.h"

#pragma package(smart_init)

// A view of one pub over its main and sub grids, with its editors made and no controls attached yet.
DataGridUI::DataGridUI(String name, String caption, TElTreeStringGrid *grid, TElTreeStringGrid *detail_grid,
    String index_name)
{
    table_name = name;
    record_name = caption;
    main_grid = grid;
    sub_grid = detail_grid;
    data_grid = new DataGrid(name, FormMain->config->GetViewAll(), index_name);
    data_grid->SetGrids(main_grid, sub_grid);
    edit_panel = 0;
    search_edit = 0;
    view_label = 0;
    view_menu = 0;
    filter_label = 0;
    filter_image = 0;
    filter_menu = 0;
    info_value_label = 0;
    info_id_label = 0;
    info_name_label = 0;
    info_image = 0;
    gfx_image = 0;
    gfx_id_label = 0;
    gfx_name_label = 0;
    gfx_panel = 0;
    icon_image = 0;
    column_label = 0;
    message_panel = 0;
    edit_column = 0;
    filter_text = "";
    copied_row = 0;
    has_copied_row = false;
    CreateEditors();
    is_syncing = false;
    is_ready = false;
    gfx_collapsed = false;
    has_changes = false;
    source_edited = false;
    source_column = 0;
    gfx_base = 101;
    icon_base = 102;
    header_locked = false;
    first_column_locked = false;
    is_searching = false;
}

// Sets the resource library and numbering of the graphics the gfx viewer shows.
int DataGridUI::SetMainGraphics(HINSTANCE__ *library, String name, int width, int first, int step, char transparent)
{
    if (width > 100)
        gfx_base = width;
    gfx_column_name = name;
    gfx_library = library;
    gfx_first = first;
    gfx_step = step;
    gfx_transparent = transparent;
    return 0;
}

// Sets the library and numbering of the icons; without a library the gfx viewer's library is used.
int DataGridUI::SetSubGraphics(HINSTANCE__ *library, String name, int width, int first, int step, char transparent)
{
    if (width > 100)
        icon_base = width;
    icon_column_name = name;
    icon_first = first;
    icon_step = step;
    icon_transparent = transparent;
    if (!library)
    {
        icon_library = gfx_library;
        icon_first = gfx_first;
        return icon_first;
    }
    icon_library = library;
    return 0;
}

// Holds the main grid's drawing and the selection handling until EndUpdate.
void DataGridUI::BeginUpdate()
{
    if (main_grid)
    {
        if (!main_grid->IsUpdating)
            main_grid->IsUpdating = true;
        is_ready = false;
    }
}

// Lets the main grid draw again and its selection changes be handled.
void DataGridUI::EndUpdate()
{
    if (main_grid)
    {
        if (main_grid->IsUpdating)
            main_grid->IsUpdating = false;
        is_ready = true;
    }
}

void DataGridUI::SetHeaderLock(int unused, char locked)
{
    if (!locked && first_column_locked)
    {
        if (main_grid)
            main_grid->LockedHeaderSection = 0;
    }
    header_locked = locked;
    first_column_locked = false;
}

// With the option on, locks the Name column once the grid scrolls past the ID column, and frees it again.
void DataGridUI::LockFirstColumnOnScroll()
{
    if (FormMain->config->GetLockedIndexColumn())
    {
        if (main_grid)
        {
            main_grid->IsUpdating = true;
            is_ready = false;
            int left_position = main_grid->LeftPosition;
            if (left_position > main_grid->HeaderSections->Item[0]->Width)
            {
                if (!first_column_locked)
                {
                    main_grid->LockedHeaderSection = main_grid->HeaderSections->Item[1];
                    first_column_locked = true;
                }
            }
            else if (first_column_locked)
            {
                main_grid->LockedHeaderSection = main_grid->HeaderSections->Item[0];
                main_grid->LockedHeaderSection = 0;
                ((TCustomElTree *)main_grid)->EnsureVisible(main_grid->Selected);
                first_column_locked = false;
            }
            is_ready = true;
            main_grid->IsUpdating = false;
        }
    }
}

// Shows or hides the rows whose name is blank.
void DataGridUI::ShowColumns(char all, int unused)
{
    if (!main_grid)
        return;
    if (is_searching)
        return;
    main_grid->FilteredVisibility = true;
    for (int section = 1; section < main_grid->HeaderSections->Count; section++)
    {
        if (String(main_grid->HeaderSections->Item[section]->Text) == "Name")
        {
            unused = section - 1;
            break;
        }
    }
    for (int i = 0; i < main_grid->Items->Count; i++)
    {
        TElTreeItem *item = main_grid->Items->Item[i];
        String item_name(item->ColumnText->Strings[unused]);
        if (item_name.Length() < 2)
        {
            if (all)
                item->Hidden = false;
            if (!all)
                item->Hidden = true;
        }
    }
}

void DataGridUI::SetItemHints(char on, int unused)
{
    TElTreeItem *item;
    if (!main_grid)
        return;
    for (int i = 0; i < main_grid->Items->Count; i++)
    {
        item = main_grid->Items->Item[i];
        if (on)
            item->Hint = item->Text + " : " + item->ColumnText->Strings[unused];
        else
            item->Hint = WideString("");
    }
}

// Puts 0 in the empty number cells of both grids, or blanks the cells holding 0.
void DataGridUI::FillSubGrid(char clear)
{
    int section;
    int line;
    TElWideStrings *zero_cells;
    TElWideStrings *blank_cells;
    TElWideStrings *sub_zero_cells;
    TElWideStrings *sub_blank_cells;
    if (!main_grid)
        return;
    int cell_count = 0;
    for (int i = 0; main_grid->Items->Count > i; i++)
    {
        for (section = 0; section < main_grid->HeaderSections->Count - 1; section++)
        {
            if (data_grid->GetColumn(section + 1)->text_format != 4)
            {
                if (clear)
                {
                    if (String(main_grid->Items->Item[i]->ColumnText->Strings[section]) == "")
                    {
                        zero_cells = main_grid->Items->Item[i]->ColumnText;
                        zero_cells->Strings[section] = WideString("0");
                    }
                }
                else if (String(main_grid->Items->Item[i]->ColumnText->Strings[section]) == "0")
                {
                    blank_cells = main_grid->Items->Item[i]->ColumnText;
                    blank_cells->Strings[section] = WideString("");
                }
                cell_count++;
                if (cell_count > 120000)
                    return;
            }
        }
    }
    if (!sub_grid)
        return;
    for (line = 0; line < sub_grid->Items->Count; line++)
    {
        if (data_grid->GetColumn(line)->text_format != 4)
        {
            if (clear)
            {
                if (String(sub_grid->Items->Item[line]->ColumnText->Strings[0]) == "")
                {
                    sub_zero_cells = sub_grid->Items->Item[line]->ColumnText;
                    sub_zero_cells->Strings[0] = WideString("0");
                }
            }
            else if (String(sub_grid->Items->Item[line]->ColumnText->Strings[0]) == "0")
            {
                sub_blank_cells = sub_grid->Items->Item[line]->ColumnText;
                sub_blank_cells->Strings[0] = WideString("");
            }
            cell_count++;
            if (cell_count > 120000)
                return;
        }
    }
}

bool DataGridUI::IsChanged()
{
    return has_changes;
}

// Sorts the main grid by ID, ascending.
void DataGridUI::SortById()
{
    if (main_grid)
    {
        is_ready = false;
        main_grid->IsUpdating = true;
        main_grid->SortSection = 0;
        main_grid->SortDir = sdAscend;
        main_grid->Sort(true);
        is_ready = true;
        main_grid->IsUpdating = false;
    }
}

void DataGridUI::ClearChanged()
{
    has_changes = false;
}

DataGridUI::~DataGridUI()
{
}

void DataGridUI::SetEditPanel(TPanel *panel)
{
    edit_panel = panel;
}

void DataGridUI::SetSearchEdit(TEdit *edit)
{
    if (!search_edit)
    {
        search_edit = edit;
        search_edit->OnChange = SearchChange;
    }
}

void DataGridUI::SetViewMenu(TElPopupMenu *menu, TLabel *label, int left, int top)
{
    TElMenuItem *item;
    TElMenuItem *last_item;
    if (view_label)
        return;
    view_label = label;
    view_menu_left = left;
    view_menu_top = top;
    view_menu = menu;
    view_label->OnMouseUp = ViewLabelMouseUp;
    view_menu->Font->Size = 16;
    view_menu->Font->Name = "MS Sans Serif";
    view_menu->AutoPopup = false;
    view_menu->OwnerDraw = true;
    for (int i = 0; i < data_grid->ViewCount(); i++)
    {
        DataGridView *view = data_grid->GetView(i);
        item = new TElMenuItem((TComponent *)view_menu);
        item->Caption = WideString(view->name);
        item->OnClick = ViewMenuClick;
        view_menu->Items->Add(item);
    }
    unsigned char item_count = view_menu->Items->Count;
    if (!item_count)
        return;
    if (FormMain->config->GetViewAll())
    {
        last_item = view_menu->Items->Items[item_count - 1];
        view_label->Caption = last_item->Caption;
        last_item->Checked = true;
        if (filter_image)
            filter_image->Left = view_label->Left + view_label->Width + 8;
        if (filter_label)
            filter_label->Left = filter_image->Left + filter_image->Width + 8;
    }
    else
        view_menu->Items->Items[0]->Checked = true;
}

// Attaches the filter menu, its label and image: an all line, then a line for each choice of the column.
void DataGridUI::SetFilterMenu(TElPopupMenu *menu, TLabel *label, TImage *image, int left, int top, String column_name,
    String caption)
{
    if (filter_label)
        return;
    filter_label = label;
    filter_image = image;
    filter_menu_left = left;
    filter_menu_top = top;
    filter_column_name = column_name;
    filter_menu = menu;
    filter_label->Caption = caption;
    filter_label->OnMouseUp = FilterLabelMouseUp;
    filter_menu->Font->Size = 16;
    filter_menu->Font->Name = "MS Sans Serif";
    filter_menu->AutoPopup = false;
    filter_menu->OwnerDraw = true;
    TElMenuItem *all_item = new TElMenuItem((TComponent *)filter_menu);
    all_item->Caption = WideString(caption);
    all_item->OnClick = FilterMenuClick;
    all_item->Checked = true;
    filter_menu->Items->Add(all_item);
    if (column_name.Length() > 0)
    {
        DataGridCol *column = data_grid->FindColumn(filter_column_name);
        for (int i = 0; i < column->ComboItemCount(); i++)
        {
            TElMenuItem *item = new TElMenuItem((TComponent *)filter_menu);
            DataComboItem *combo_item = column->GetComboItem(i);
            item->Caption = WideString(combo_item->text);
            item->OnClick = FilterMenuClick;
            item->Checked = false;
            filter_menu->Items->Add(item);
        }
    }
    if (view_label)
    {
        filter_image->Left = view_label->Left + view_label->Width + 8;
        filter_label->Left = filter_image->Left + filter_image->Width + 8;
    }
}

// Attaches the labels and image that show the selected row's name, ID and info value.
void DataGridUI::SetInfoLabels(TLabel *name_label, TLabel *id_label, TLabel *value_label, TImage *image,
    String column_name)
{
    info_value_label = value_label;
    info_id_label = id_label;
    info_name_label = name_label;
    info_image = image;
    info_column_name = column_name;
}

void DataGridUI::SetGfxViewer(TImage *image, TPanel *panel, TLabel *id_label, TLabel *name_label)
{
    gfx_image = image;
    gfx_id_label = id_label;
    gfx_name_label = name_label;
    gfx_panel = panel;
}

void DataGridUI::SetIconImage(TImage *image)
{
    icon_image = image;
}

void DataGridUI::SetColumnLabel(TLabel *label, String column_name)
{
    column_label = label;
    label_column_name = column_name;
}

void DataGridUI::SetMessagePanel(TPanel *back, TPanel *panel)
{
    if (message_panel)
        return;
    message_panel = panel;
    message_back = back;
    message_panel->Left = 0;
    message_panel->Top = 26;
    message_panel->Width = message_back->Width;
    message_panel->Height = message_back->Height;
    message_timer = new TTimer(NULL);
    message_timer->Interval = 10;
    message_timer->Enabled = false;
    message_timer->OnTimer = MessageTimerTick;
}

// Slides the message panel down for 25 ticks and hides it after 250.
void __fastcall DataGridUI::MessageTimerTick(TObject *sender)
{
    message_ticks++;
    if (message_ticks < 25)
        message_panel->Top = 25 - message_ticks;
    if (message_ticks > 250)
    {
        message_back->Visible = false;
        message_timer->Enabled = false;
    }
}

// Shows the text on the message panel and starts its timer.
void DataGridUI::ShowMessage(String text)
{
    if (!message_panel)
        return;
    message_panel->Caption = text;
    message_panel->Top = 25;
    message_back->Visible = true;
    message_ticks = 0;
    message_timer->Enabled = true;
}

// Makes the text, spin and list editors of both grids, dark-themed when set, and hooks their events.
void DataGridUI::CreateEditors()
{
    main_text_editor = new TElTreeInplaceEdit((TComponent *)main_grid);
    main_spin_editor = new TElTreeInplaceSpinEdit((TComponent *)main_grid);
    main_combo_editor = new TElTreeInplaceComboBox((TComponent *)main_grid);
    if (FormMain->config->GetTheme() == "dark")
    {
        main_text_editor->Editor->UseXPThemes = false;
        main_text_editor->Editor->Flat = true;
        main_text_editor->Editor->ParentColor = true;
        main_text_editor->Editor->ParentFont = true;
        main_text_editor->Editor->BorderStyle = bsNone;
        main_text_editor->Editor->Font->Size = sub_grid->Font->Size;
        main_text_editor->Editor->Font->Color = (TColor)0xDDD164;
        main_combo_editor->Editor->UseXPThemes = false;
        main_combo_editor->Editor->Flat = true;
        main_combo_editor->Editor->BorderStyle = bsNone;
        main_combo_editor->Editor->ListColor = (TColor)0x3D3D3D;
        main_combo_editor->Editor->ButtonColor = (TColor)0x555555;
        main_combo_editor->Editor->ButtonWidth = 24;
        main_combo_editor->Editor->ButtonFlat = true;
        main_combo_editor->Editor->ListTransparentSelection = true;
        main_combo_editor->Editor->Color = (TColor)0x5E5E5E;
        main_spin_editor->Editor->UseXPThemes = false;
        main_spin_editor->Editor->Flat = true;
        main_spin_editor->Editor->BorderStyle = bsNone;
        main_spin_editor->Editor->ParentColor = true;
        main_spin_editor->Editor->ParentFont = true;
        main_spin_editor->Editor->Font->Size = sub_grid->Font->Size;
        main_spin_editor->Editor->Font->Color = (TColor)0xDDD164;
    }
    main_text_editor->Tree = (TCustomElTree *)main_grid;
    main_spin_editor->Tree = (TCustomElTree *)main_grid;
    main_combo_editor->Tree = (TCustomElTree *)main_grid;
    main_combo_editor->InitiallyDropped = true;
    main_combo_editor->Editor->DropDownCount = 28;
    main_combo_editor->Editor->AutoCompletion = true;
    sub_text_editor = new TElTreeInplaceEdit((TComponent *)sub_grid);
    sub_spin_editor = new TElTreeInplaceSpinEdit((TComponent *)sub_grid);
    sub_combo_editor = new TElTreeInplaceComboBox((TComponent *)sub_grid);
    if (FormMain->config->GetTheme() == "dark")
    {
        sub_text_editor->Editor->UseXPThemes = false;
        sub_text_editor->Editor->Flat = true;
        sub_text_editor->Editor->ParentColor = true;
        sub_text_editor->Editor->ParentFont = true;
        sub_text_editor->Editor->BorderStyle = bsNone;
        sub_text_editor->Editor->Font->Size = sub_grid->Font->Size;
        sub_text_editor->Editor->Font->Color = (TColor)0xDDD164;
        sub_combo_editor->Editor->UseXPThemes = false;
        sub_combo_editor->Editor->Flat = true;
        sub_combo_editor->Editor->BorderStyle = bsNone;
        sub_combo_editor->Editor->ListColor = (TColor)0x3D3D3D;
        sub_combo_editor->Editor->ButtonColor = (TColor)0x555555;
        sub_combo_editor->Editor->ButtonWidth = 24;
        sub_combo_editor->Editor->ButtonFlat = true;
        sub_combo_editor->Editor->ListTransparentSelection = true;
        sub_combo_editor->Editor->Color = (TColor)0x5E5E5E;
        sub_spin_editor->Editor->UseXPThemes = false;
        sub_spin_editor->Editor->Flat = true;
        sub_spin_editor->Editor->BorderStyle = bsNone;
        sub_spin_editor->Editor->ParentColor = true;
        sub_spin_editor->Editor->ParentFont = true;
        sub_spin_editor->Editor->Font->Size = sub_grid->Font->Size;
        sub_spin_editor->Editor->Font->Color = (TColor)0xDDD164;
    }
    sub_text_editor->Tree = (TCustomElTree *)sub_grid;
    sub_spin_editor->Tree = (TCustomElTree *)sub_grid;
    sub_combo_editor->Tree = (TCustomElTree *)sub_grid;
    sub_combo_editor->InitiallyDropped = true;
    sub_combo_editor->Editor->DropDownCount = 28;
    sub_combo_editor->Editor->AutoCompletion = true;
    main_text_editor->OnValidateResult = TextEditorAfter;
    main_text_editor->OnAfterOperation = TextEditorValidate;
    main_spin_editor->OnValidateResult = SpinEditorAfter;
    main_spin_editor->OnAfterOperation = SpinEditorValidate;
    main_combo_editor->OnValidateResult = ComboEditorAfter;
    main_combo_editor->OnAfterOperation = ComboEditorValidate;
    main_text_editor->OnBeforeOperation = EditorKeyDown;
    main_spin_editor->OnBeforeOperation = EditorKeyDown;
    main_combo_editor->OnBeforeOperation = EditorKeyDown;
    main_spin_editor->Editor->OnKeyPress = SpinEditorKeyPress;
    sub_text_editor->OnValidateResult = TextEditorAfter;
    sub_text_editor->OnAfterOperation = TextEditorValidate;
    sub_spin_editor->OnValidateResult = SpinEditorAfter;
    sub_spin_editor->OnAfterOperation = SpinEditorValidate;
    sub_combo_editor->OnValidateResult = ComboEditorAfter;
    sub_combo_editor->OnAfterOperation = ComboEditorValidate;
    sub_text_editor->OnBeforeOperation = EditorKeyDown;
    sub_spin_editor->OnBeforeOperation = EditorKeyDown;
    sub_combo_editor->OnBeforeOperation = EditorKeyDown;
    sub_spin_editor->Editor->OnKeyPress = SpinEditorKeyPress;
}

// Picks the editor for a main-grid cell by its column's kind; returns the kind of a column edited elsewhere.
unsigned char DataGridUI::EditMainCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor)
{
    bool loop_entered = false;
    TElWideStrings *error_items;
    TElWideStrings *link_items;
    DataLinkItem *link;
    DataGridCol *index_column;
    if (!main_grid)
        return 0;
    if (main_grid->Selected == NULL)
        return 0;
    if (edit_panel)
        edit_panel->Visible = true;
    if (first_column_locked && section <= 1)
        main_grid->LeftPosition = 0;
    main_section = section;
    main_row = main_grid->Selected->Index;
    edit_grid = main_grid;
    edit_column = data_grid->GetColumn(main_section);
    data_grid->edit_row = main_grid->Selected;
    // With smart editors the row's index value picks the column's variant to edit.
    if (FormMain->config->GetSmartEditors())
    {
        index_column = data_grid->GetIndexColumn();
        if (index_column)
        {
            String index_text(main_grid->Selected->ColumnText->Strings[index_column->text_index]);
            DataGridCol *sub_column = edit_column->FindSubColumn(index_text);
            if (sub_column)
                edit_column = sub_column;
        }
    }
    if (edit_column->kind == ColumnKind_Number)
    {
        main_spin_editor->Editor->MinValue = edit_column->min_value;
        if (edit_column->max_value == 252)
        {
            main_spin_editor->Editor->MaxValue = 9999;
            main_spin_editor->Editor->Tag = 252;
        }
        if (edit_column->max_value == 64008)
        {
            main_spin_editor->Editor->MaxValue = 999999;
            main_spin_editor->Editor->Tag = 64008;
        }
        if (edit_column->max_value == 0xF71AE3)
        {
            main_spin_editor->Editor->MaxValue = 999999999;
            main_spin_editor->Editor->Tag = 0xF71AE3;
        }
        editor = main_spin_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Combo || edit_column->kind == ColumnKind_SmartCombo)
    {
        main_combo_editor->Editor->Items->Clear();
        for (int i = 0; i < edit_column->ComboItemCount(); i++)
        {
            loop_entered = true;
            DataComboItem *combo_item = edit_column->GetComboItem(i);
            main_combo_editor->Editor->Items->Add(WideString(combo_item->text));
        }
        editor = main_combo_editor;
        for (int j = 0; main_combo_editor->Editor->Items->Count > j; j++)
        {
            loop_entered = true;
            if (main_combo_editor->Editor->Items->Strings[j] == edit_column->header_text)
                main_combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    // A linked column offers the names of the linked grid's rows.
    if (edit_column->kind == ColumnKind_Link)
    {
        main_combo_editor->Editor->Items->Clear();
        if (edit_column->LinkCount() > 0)
        {
            link = edit_column->GetLink(0);
            TElTreeStringGrid *link_grid = link->grid;
            short link_section = link->FindColumnIndex();
            if (link_section < 0)
            {
                error_items = main_combo_editor->Editor->Items;
                error_items->Add(WideString("[LCERR]"));
            }
            else
            {
                link_items = main_combo_editor->Editor->Items;
                link_items->Add(WideString("-"));
                for (int row = 0; link_grid->Items->Count > row; row++)
                {
                    loop_entered = true;
                    edit_column->GetComboItem(row);
                    main_combo_editor->Editor->Items->Add(WideString(String(link_grid->Items->Item[row]->ColumnText->Strings[link_section])));
                }
            }
        }
        editor = main_combo_editor;
        for (int k = 0; main_combo_editor->Editor->Items->Count > k; k++)
        {
            loop_entered = true;
            if (main_combo_editor->Editor->Items->Strings[k] == edit_column->header_text)
                main_combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    if (edit_column->kind == ColumnKind_LinkSource)
    {
        source_edited = true;
        source_column = edit_column;
        if (!section)
            source_text = item->Text;
        if (section > 0)
            source_text = item->ColumnText->Strings[section - 1];
        main_text_editor->Editor->MaxLength = edit_column->max_value;
        editor = main_text_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Text)
    {
        main_text_editor->Editor->MaxLength = edit_column->max_value;
        editor = main_text_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Index)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return ColumnKind_Index;
    }
    if (edit_column->kind >= ColumnKind_FirstEditorForm)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return edit_column->kind;
    }
    if (edit_column->kind == ColumnKind_Graphic)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        int gfx_index = data_grid->ColumnIndexOf("Gfx");
        if (gfx_index)
        {
        }
        return ColumnKind_Graphic;
    }
    if (edit_column->kind == ColumnKind_PaperdollGraphic)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return ColumnKind_PaperdollGraphic;
    }
    editor = NULL;
    return 0;
}

// Picks the editor for a sub-grid line by its column's kind; returns the kind of a column edited elsewhere.
unsigned char DataGridUI::EditSubCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor)
{
    TElWideStrings *error_items;
    TElWideStrings *link_items;
    DataGridCol *index_column;
    if (!sub_grid)
        return 0;
    if (sub_grid->Selected == NULL)
        return 0;
    if (edit_panel)
        edit_panel->Visible = true;
    sub_section = section;
    sub_row = sub_grid->Selected->Index;
    edit_grid = sub_grid;
    edit_column = data_grid->GetColumn(sub_grid->Selected->Tag);
    String column_name = edit_column->header_text;
    data_grid->edit_detail_item = sub_grid->Selected;
    if (main_grid->Selected != NULL)
    {
        data_grid->edit_row = main_grid->Selected;
        data_grid->store_row = main_grid->Selected;
    }
    // With smart editors the row's index value picks the column's variant to edit.
    if (FormMain->config->GetSmartEditors())
    {
        index_column = data_grid->GetIndexColumn();
        if (index_column)
        {
            String index_text(sub_grid->Items->Item[index_column->index]->ColumnText->Strings[0]);
            DataGridCol *sub_column = edit_column->FindSubColumn(index_text);
            if (sub_column)
                edit_column = sub_column;
        }
    }
    if (edit_column->disabled)
    {
        editor = NULL;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Number)
    {
        sub_spin_editor->Editor->MinValue = edit_column->min_value;
        if (edit_column->max_value == 252)
        {
            sub_spin_editor->Editor->MaxValue = 9999;
            sub_spin_editor->Editor->Tag = 252;
        }
        if (edit_column->max_value == 64008)
        {
            sub_spin_editor->Editor->MaxValue = 999999;
            sub_spin_editor->Editor->Tag = 64008;
        }
        if (edit_column->max_value == 0xF71AE3)
        {
            sub_spin_editor->Editor->MaxValue = 999999999;
            sub_spin_editor->Editor->Tag = 0xF71AE3;
        }
        editor = sub_spin_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Combo || edit_column->kind == ColumnKind_SmartCombo)
    {
        sub_combo_editor->Editor->Items->Clear();
        for (int i = 0; i < edit_column->ComboItemCount(); i++)
        {
            DataComboItem *combo_item = edit_column->GetComboItem(i);
            sub_combo_editor->Editor->Items->Add(WideString(combo_item->text));
        }
        editor = sub_combo_editor;
        for (int j = 0; sub_combo_editor->Editor->Items->Count > j; j++)
        {
            if (sub_combo_editor->Editor->Items->Strings[j] == edit_column->header_text)
                sub_combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    if (edit_column->kind == ColumnKind_Link)
    {
        main_combo_editor->Editor->Items->Clear();
        if (edit_column->LinkCount() > 0)
        {
            DataLinkItem *link = edit_column->GetLink(0);
            TElTreeStringGrid *link_grid = link->grid;
            short link_section = link->FindColumnIndex();
            if (link_section < 0)
            {
                error_items = main_combo_editor->Editor->Items;
                error_items->Add(WideString("[LCERR]"));
            }
            else
            {
                link_items = main_combo_editor->Editor->Items;
                link_items->Add(WideString("-"));
                for (int row = 0; link_grid->Items->Count > row; row++)
                {
                    DataComboItem *link_combo_item = edit_column->GetComboItem(row);
                    main_combo_editor->Editor->Items->Add(WideString(String(link_grid->Items->Item[row]->ColumnText->Strings[link_section])));
                }
            }
        }
        editor = main_combo_editor;
        for (int k = 0; main_combo_editor->Editor->Items->Count > k; k++)
        {
            if (main_combo_editor->Editor->Items->Strings[k] == edit_column->header_text)
                main_combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    if (edit_column->kind == ColumnKind_LinkSource)
    {
        source_edited = true;
        source_column = edit_column;
        if (!section)
            source_text = item->Text;
        if (section > 0)
            source_text = item->ColumnText->Strings[section - 1];
        main_text_editor->Editor->MaxLength = edit_column->max_value;
        editor = main_text_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Text)
    {
        sub_text_editor->Editor->MaxLength = edit_column->max_value;
        editor = sub_text_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Index)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return ColumnKind_Index;
    }
    if (edit_column->kind >= ColumnKind_FirstEditorForm)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return edit_column->kind;
    }
    if (edit_column->kind == ColumnKind_Graphic)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return ColumnKind_Graphic;
    }
    if (edit_column->kind == ColumnKind_PaperdollGraphic)
    {
        editor = NULL;
        if (edit_panel)
            edit_panel->Visible = false;
        return ColumnKind_PaperdollGraphic;
    }
    editor = NULL;
    return 0;
}

void __fastcall DataGridUI::SpinEditorKeyPress(TObject *sender, char &key)
{
}

// With mouse quick save on, keeps the grid, row, column and text being edited for a later save.
void __fastcall DataGridUI::EditorKeyDown(TObject *sender, bool &handled)
{
    if (!FormMain->config->GetMouseQuickSave())
        return;
    if (!main_grid)
        return;
    if (main_grid->Selected == NULL)
        return;
    if (main_grid->Items->Count > edit_column->text_index)
    {
        FormMain->edit_grid = main_grid;
        FormMain->edit_row = main_grid->Selected;
        FormMain->edit_column = (char)edit_column->text_index;
        FormMain->edit_backup = main_grid->Selected->ColumnText->Strings[edit_column->text_index];
    }
}

// Trims the text; with mouse quick save, refuses a text too short or EOF and capitalizes when set.
void __fastcall DataGridUI::TextEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
    if (edit_panel)
        edit_panel->Visible = false;
    TElTreeInplaceEdit *editor = (TElTreeInplaceEdit *)sender;
    String text(editor->Editor->Text);
    text = text.Trim();
    editor->Editor->Text = WideString(text);
    if (FormMain->config->GetMouseQuickSave())
    {
        TElTreeInplaceEdit *checked_editor = (TElTreeInplaceEdit *)sender;
        String checked_text(checked_editor->Editor->Text);
        if (checked_text.Length() <= 1)
        {
            if (!FormMain->config->GetExpertMode())
            {
                accept = false;
                return;
            }
        }
        if (FormMain->config->GetAutoCapitalization() && checked_text.Length() > 0)
        {
            checked_text[1] = toupper(checked_text[1]);
            checked_editor->Editor->Text = WideString(checked_text);
        }
        if (LowerCase(checked_text) == "eof")
            accept = false;
        else
            accept = true;
    }
}

// Keeps the number within the editor's limit and hides the edit panel.
void __fastcall DataGridUI::SpinEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
    if (FormMain->config->GetMouseQuickSave())
        accept = true;
    TElTreeInplaceSpinEdit *editor = (TElTreeInplaceSpinEdit *)sender;
    if (editor->Editor->Tag < editor->Editor->Value)
        editor->Editor->Value = editor->Editor->Tag;
    if (edit_panel)
        edit_panel->Visible = false;
}

// Takes the typed text as a choice: by name, by part of a name, by number or as [OOB-n]; else refuses it.
void __fastcall DataGridUI::ComboEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
    if (edit_panel)
        edit_panel->Visible = false;
    if (!FormMain->config->GetMouseQuickSave() && !accept)
        return;
    if (edit_column->kind != ColumnKind_Combo && edit_column->kind != ColumnKind_SmartCombo &&
        edit_column->kind != ColumnKind_Link)
    {
        accept = false;
        return;
    }
    String text(((TElTreeInplaceComboBox *)sender)->Editor->Text);
    String lower_text = LowerCase(text);
    if (edit_column->kind == ColumnKind_SmartCombo)
    {
        if (lower_text.Length() <= 0)
        {
            ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString("");
            accept = true;
            return;
        }
    }
    // Three letters or more match a choice by its whole text, then by part of it.
    if (text.Length() > 2)
    {
        for (int i = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > i; i++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i]) == lower_text)
            {
                if (edit_column->kind == ColumnKind_SmartCombo && edit_grid == main_grid)
                {
                    ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(HideZeroText(IntToStr(i)));
                    accept = true;
                    return;
                }
                ((TElTreeInplaceComboBox *)sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i];
                accept = true;
                return;
            }
        }
        for (int j = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > j; j++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[j]).Pos(lower_text) > 0)
            {
                if (edit_column->kind == ColumnKind_SmartCombo && edit_grid == main_grid)
                {
                    ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(HideZeroText(IntToStr(j)));
                    accept = true;
                    return;
                }
                ((TElTreeInplaceComboBox *)sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[j];
                accept = true;
                return;
            }
        }
    }
    String prefix = "[OOB-";
    String suffix = "]";
    if (edit_column->kind == ColumnKind_SmartCombo)
    {
        prefix = "";
        suffix = "";
    }
    // A number picks the choice at that place; past the list it is kept as a mark.
    int number = FormMain->NumberFromDigits(text);
    if (number >= 0)
    {
        if (number < ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count)
        {
            ((TElTreeInplaceComboBox *)sender)->Editor->ItemIndex = number;
            accept = true;
            return;
        }
        if ((unsigned int)edit_column->max_value < number)
            number = edit_column->max_value;
        ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(prefix + IntToStr(number) + suffix);
        accept = true;
        return;
    }
    int out_of_bounds = FormMain->OutOfBoundsValue(text);
    if (out_of_bounds >= 0)
    {
        if ((unsigned int)edit_column->max_value < out_of_bounds)
            out_of_bounds = edit_column->max_value;
        ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(prefix + IntToStr(out_of_bounds) + suffix);
        accept = true;
        return;
    }
    for (int k = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > k; k++)
    {
        if (((TElTreeInplaceComboBox *)sender)->Editor->Text ==
            ((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[k])
        {
            accept = true;
            return;
        }
    }
    accept = false;
}

// Applies the column's letter case, refuses EOF, and renames an edited source in every linked grid.
void __fastcall DataGridUI::TextEditorAfter(TObject *sender, bool &changed)
{
    TElWideStrings *cells;
    TElTreeInplaceEdit *editor = (TElTreeInplaceEdit *)sender;
    String text(editor->Editor->Text);
    if (text.Length() <= 1 && !FormMain->config->GetExpertMode())
        changed = false;
    if (LowerCase(text) == "eof")
    {
        String message = "EOF tags are not needed. The Endless parser supports EOF automatically once needed.";
        FormWarning->ShowWarning(FormMain, "About EOF..", message);
        changed = false;
    }
    if (edit_column->text_format == 1)
    {
        if (text.Length() > 0)
        {
            if (FormMain->config->GetAutoCapitalization())
            {
                text[1] = toupper(text[1]);
                editor->Editor->Text = WideString(text);
            }
        }
    }
    if (edit_column->text_format == 3 && text.Length() > 0)
    {
        text = LowerCase(text);
        editor->Editor->Text = WideString(text);
    }
    if (edit_column->text_format == 2 && text.Length() > 0)
    {
        text = UpperCase(text);
        editor->Editor->Text = WideString(text);
    }
    // A renamed source text is renamed in every grid that links to it.
    if (source_edited)
    {
        if (source_column->kind == ColumnKind_LinkSource)
        {
            unsigned char link_count = source_column->LinkCount();
            for (int i = 0; link_count > i; i++)
            {
                DataLinkItem *link = source_column->GetLink(i);
                short section = link->FindColumnIndex();
                for (int row = 0; row < link->grid->Items->Count; row++)
                {
                    int item = (int)link->grid->Items->Item[row];
                    if (((TElTreeItem *)item)->ColumnText->Strings[section] == source_text)
                    {
                        cells = ((TElTreeItem *)item)->ColumnText;
                        cells->Strings[section] = editor->Editor->Text;
                    }
                }
            }
        }
    }
}

// An empty number becomes 0 and a number past the limit becomes the limit.
DEFINE_SPIN_EDITOR_AFTER(DataGridUI)

// Settles the chosen text: by name, by part of a name, by number or as [OOB-n]; else no change.
void __fastcall DataGridUI::ComboEditorAfter(TObject *sender, bool &changed)
{
    ((TElTreeInplaceComboBox *)sender)->Editor->ItemIndex =
        ((TElTreeInplaceComboBox *)sender)->Editor->SelectedItemIndex;
    String text(((TElTreeInplaceComboBox *)sender)->Editor->Text);
    if (text.Length() < 0)
    {
        changed = false;
        return;
    }
    String lower_text = LowerCase(text);
    if (edit_column->kind == ColumnKind_SmartCombo && lower_text.Length() <= 0)
    {
        ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString("");
        changed = true;
        return;
    }
    // Three letters or more match a choice by its whole text, then by part of it.
    if (text.Length() > 2)
    {
        for (int i = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > i; i++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i]) == lower_text)
            {
                if (edit_column->kind == ColumnKind_SmartCombo && edit_grid == main_grid)
                {
                    ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(HideZeroText(IntToStr(i)));
                    changed = true;
                    return;
                }
                ((TElTreeInplaceComboBox *)sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i];
                changed = true;
                return;
            }
        }
        for (int j = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > j; j++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[j]).Pos(lower_text) > 0)
            {
                if (edit_column->kind == ColumnKind_SmartCombo && edit_grid == main_grid)
                {
                    ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(HideZeroText(IntToStr(j)));
                    changed = true;
                    return;
                }
                ((TElTreeInplaceComboBox *)sender)->Editor->Text =
                    ((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[j];
                changed = true;
                return;
            }
        }
    }
    String prefix = "[OOB-";
    String suffix = "]";
    if (edit_column->kind == ColumnKind_SmartCombo)
    {
        prefix = "";
        suffix = "";
    }
    int number = FormMain->NumberFromDigits(text);
    if (number >= 0)
    {
        if (number < ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count)
        {
            ((TElTreeInplaceComboBox *)sender)->Editor->ItemIndex = number;
            changed = true;
            return;
        }
        ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(prefix + IntToStr(number) + suffix);
        changed = true;
        return;
    }
    int out_of_bounds = FormMain->OutOfBoundsValue(text);
    if (out_of_bounds >= 0)
    {
        if ((unsigned int)edit_column->max_value < out_of_bounds)
            out_of_bounds = edit_column->max_value;
        ((TElTreeInplaceComboBox *)sender)->Editor->Text = WideString(prefix + IntToStr(out_of_bounds) + suffix);
        changed = true;
        return;
    }
    for (int k = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > k; k++)
    {
        if (((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[k] == text)
        {
            changed = true;
            return;
        }
    }
    changed = false;
}

// Searches for the typed text, then without its third letter, then without its first.
void __fastcall DataGridUI::SearchChange(TObject *sender)
{
    TEdit *search_edit = (TEdit *)sender;
    String text = search_edit->Text;
    if (Search(filter_text, text) > 0)
        return;
    String shortened = text;
    if (shortened.Length() > 3)
        shortened.Delete(3, 1);
    if (Search(filter_text, shortened) > 0)
        return;
    shortened = text;
    if (shortened.Length() > 3)
        shortened.Delete(1, 1);
    if (Search(filter_text, shortened) > 0)
        return;
}

// Shows the rows of the filter value whose name or number holds the text; returns the rows shown.
int DataGridUI::Search(String filter, String text)
{
    int found = 0;
    if (filter.Length() > 0 || text.Length() > 0)
        is_searching = true;
    else
        is_searching = false;
    main_grid->IsUpdating = true;
    text = LowerCase(text);
    String shortened = text;
    if (shortened.Length() > 2)
        shortened.Delete(2, 1);
    int name_index = 0;
    for (int section = 1; section < main_grid->HeaderSections->Count; section++)
    {
        if (String(main_grid->HeaderSections->Item[section]->Text) == "Name")
        {
            name_index = section - 1;
            break;
        }
    }
    for (int i = 0; main_grid->Items->Count > i; i++)
    {
        TElTreeItem *item = main_grid->Items->Item[i];
        String filter_value = "";
        int filter_index = data_grid->ColumnIndexOf(filter_column_name);
        if (filter_index > 0)
            filter_value = item->ColumnText->Strings[filter_index - 1];
        if (filter_value == filter || filter == "")
        {
            String item_name = LowerCase(item->ColumnText->Strings[name_index]);
            int name_pos = item_name.Pos(text);
            int shortened_pos = item_name.Pos(shortened);
            int id_pos = item->Text.Pos(text);
            // Rows without a name are recycled records, hidden unless they are shown.
            if (!FormMain->config->GetShowRecycled() && item_name.Length() < 2)
            {
                item->Hidden = true;
                continue;
            }
            if (text != "")
            {
                if (name_pos > 0 || id_pos > 0)
                {
                    item->Hidden = false;
                    found++;
                }
                else if (shortened_pos > 0 || id_pos > 0)
                {
                    item->Hidden = false;
                    found++;
                }
                else
                    item->Hidden = true;
            }
            else
            {
                item->Hidden = false;
                found++;
            }
        }
        else
            item->Hidden = true;
    }
    main_grid->FilteredVisibility = true;
    main_grid->IsUpdating = false;
    return found;
}

// Checks the view's menu line and shows only the columns of the view's groups.
void DataGridUI::ShowView(DataGridView *view)
{
    TElMenuItem *item;
    int i;
    TLabel *label;
    char was_updating;
    int column_index;
    DataGridCol *column;
    if (!main_grid)
        return;
    if (!view)
        return;
    if (view_menu)
    {
        i = 0;
        while (i < view_menu->Items->Count)
        {
            item = view_menu->Items->Items[i];
            if (item->Caption == view->name)
            {
                item->Checked = true;
                label = view_label;
                if (label)
                    label->Caption = view->name;
            }
            else
                item->Checked = false;
            i++;
        }
    }
    was_updating = main_grid->IsUpdating;
    if (!was_updating)
        main_grid->IsUpdating = true;
    column_index = 0;
    while ((short)data_grid->ColumnCount() > column_index)
    {
        column = data_grid->GetColumn(column_index);
        main_grid->HeaderSections->Item[column_index]->Visible = false;
        for (int j = 0; j < (int)view->group_count; j++)
        {
            char view_group = view->groups[j];
            view_group = view_group;
            char column_group = column->group;
            column_group = column_group;
            if (view_group == column_group)
                main_grid->HeaderSections->Item[column_index]->Visible = true;
        }
        column_index++;
    }
    String view_label_name = view_label->Name;
    String filter_label_name = filter_label->Name;
    String filter_image_name = filter_image->Name;
    if (filter_label && filter_image)
    {
        filter_image->Left = view_label->Left + view_label->Width + 8;
        filter_label->Left = filter_image->Left + filter_image->Width + 8;
    }
    if (!was_updating)
        main_grid->IsUpdating = false;
}

// Pops up the view menu beside the view label.
void __fastcall DataGridUI::ViewLabelMouseUp(TObject *sender, TMouseButton button, Set<Classes__1, 0, 6> shift, int x,
    int y)
{
    if (view_menu)
        view_menu->Popup(FormMain->Left + view_menu_left, FormMain->Top + view_menu_top);
}

void __fastcall DataGridUI::ViewMenuClick(TObject *sender)
{
    TElMenuItem *item = (TElMenuItem *)sender;
    ShowView(data_grid->GetView(item->MenuIndex));
}

void __fastcall DataGridUI::FilterLabelMouseUp(TObject *sender, TMouseButton button, Set<Classes__1, 0, 6> shift, int x,
    int y)
{
    if (filter_menu)
        filter_menu->Popup(FormMain->Left + filter_menu_left, FormMain->Top + filter_menu_top);
}

// Checks the chosen filter line, shows it on the label and searches again with it.
void __fastcall DataGridUI::FilterMenuClick(TObject *sender)
{
    TElMenuItem *item;
    if (!filter_menu)
        return;
    for (int i = 0; i < filter_menu->Items->Count; i++)
        filter_menu->Items->Items[i]->Checked = false;
    item = (TElMenuItem *)sender;
    item->Checked = true;
    if (filter_label)
        filter_label->Caption = item->Caption;
    if (item->MenuIndex > 0)
        filter_text = item->Caption;
    else
        filter_text = "";
    if (search_edit)
    {
        Search(filter_text, search_edit->Text);
        return;
    }
    Search(filter_text, "");
}

// Syncs the main and sub grids after a selection, then the info labels, graphic, icon and name hint.
void DataGridUI::SelectionChange(TObject *sender, bool reset)
{
    DataGridCol *gfx_column;
    DataGridCol *icon_column;
    if (reset)
    {
        FormMain->edit_grid = 0;
        FormMain->edit_row = 0;
    }
    if (!is_ready)
        return;
    if (is_syncing)
        return;
    is_syncing = true;
    has_changes = true;
    // A new row fills the details; an edited detail is written back to the row.
    if (sender == main_grid)
    {
        if (!sub_grid)
            return;
        if (sub_grid->IsUpdating)
            return;
        sub_grid->IsUpdating = true;
        data_grid->ShowRowDetails();
        sub_grid->IsUpdating = false;
    }
    if (sender == sub_grid)
    {
        if (!main_grid)
            return;
        if (main_grid->IsUpdating)
            return;
        main_grid->IsUpdating = true;
        data_grid->StoreDetailValue();
        main_grid->IsUpdating = false;
        sub_grid->IsUpdating = true;
        data_grid->RefreshRowDetails();
        sub_grid->IsUpdating = false;
    }
    ShowInfo(main_grid);
    if (main_grid->Selected != NULL && gfx_image)
    {
        DataGridCol *name_column = data_grid->FindColumn("Name");
        gfx_column = data_grid->FindColumn(gfx_column_name);
        if (main_grid->Selected->ColumnText->Strings[gfx_column->text_index].Length() > 0)
        {
            int gfx_number = StrToInt(main_grid->Selected->ColumnText->Strings[gfx_column->text_index]);
            ShowGfxInfo(main_grid, name_column, gfx_column);
            if (gfx_transparent == 2)
                ShowFrames(main_grid, gfx_number);
            else
                ShowGraphic(main_grid, gfx_number);
        }
    }
    if (main_grid->Selected != NULL && icon_image)
    {
        icon_column = data_grid->FindColumn(icon_column_name);
        if (main_grid->Selected->ColumnText->Strings[icon_column->text_index].Length() > 0)
        {
            int icon_number = StrToInt(main_grid->Selected->ColumnText->Strings[icon_column->text_index]);
            ShowIcon(main_grid, icon_number);
        }
    }
    if (main_grid->Selected != NULL)
    {
        int name_index = 0;
        for (int section = 1; section < main_grid->HeaderSections->Count; section++)
        {
            if (String(main_grid->HeaderSections->Item[section]->Text) == "Name")
            {
                name_index = section - 1;
                break;
            }
        }
        if (FormMain->config->GetNameHints())
            main_grid->Selected->Hint =
                main_grid->Selected->Text + " : " + main_grid->Selected->ColumnText->Strings[name_index];
    }
    ShowColumnText(main_grid);
    is_syncing = false;
}

// Shows the selected row's name, ID and info value on the labels and filters the sub grid by that value.
void DataGridUI::ShowInfo(TElTreeStringGrid *grid)
{
    if (!is_ready)
        return;
    if (!info_value_label)
        return;
    DataGridCol *name_column = data_grid->FindColumn("Name");
    DataGridCol *info_column = data_grid->FindColumn(info_column_name);
    if (grid->Selected != NULL && name_column && info_column)
    {
        String info_text(grid->Selected->ColumnText->Strings[info_column->index - 1]);
        String item_name(grid->Selected->ColumnText->Strings[name_column->index - 1]);
        info_name_label->Caption = info_text;
        info_value_label->Caption = item_name;
        info_id_label->Caption = "| ID: " + grid->Selected->Text;
        DataComboItem *filter = info_column->FindComboItem(info_text);
        ApplyFilter(filter);
    }
    else
    {
        if (gfx_image)
            gfx_image->Visible = false;
        if (icon_image)
            icon_image->Visible = false;
        info_name_label->Caption = LowerCase(record_name);
        if (grid->Selected != NULL)
        {
            String selected_name(grid->Selected->ColumnText->Strings[name_column->index - 1]);
            info_value_label->Caption = selected_name;
            info_id_label->Caption = "| ID: " + grid->Selected->Text;
        }
        else
        {
            info_value_label->Caption = "Not Selected";
            info_id_label->Caption = "";
        }
        ApplyFilter(0);
    }
    info_image->Left = info_name_label->Left + info_name_label->Width + 8;
    info_value_label->Left = info_image->Left + info_image->Width + 8;
    info_id_label->Left = info_value_label->Left + info_value_label->Width + 8;
}

// Hides the sub-grid lines outside the filter choice's groups; without a choice only the basics option counts.
void DataGridUI::ApplyFilter(DataComboItem *filter)
{
    int j;
    DataGridCol *column;
    DataGridCol *item_column;
    if (!filter)
    {
        if (!sub_grid->IsUpdating)
            sub_grid->IsUpdating = true;
        for (int i = 0; i < sub_grid->Items->Count; i++)
        {
            TElTreeItem *item = sub_grid->Items->Item[i];
            bool hidden = false;
            if (!FormMain->config->GetShowBasics())
            {
                item_column = data_grid->GetColumn(item->Tag);
                if (item_column && !item_column->group)
                    hidden = true;
            }
            item->Hidden = hidden;
        }
        sub_grid->FilteredVisibility = true;
        sub_grid->IsUpdating = false;
    }
    else
    {
        if (!sub_grid->IsUpdating)
            sub_grid->IsUpdating = true;
        for (j = 0; j < (short)data_grid->ColumnCount(); j++)
        {
            // With smart filtering each column shows its variant that matches the filter.
            column = data_grid->GetColumn(j);
            DataGridCol *sub_column = column->FindSubColumn(filter->text);
            if (sub_column && FormMain->config->GetSmartFilter())
                column = sub_column;
            if (!FormMain->config->GetShowBasics() && !column->group)
                sub_grid->Items->Item[j]->Hidden = true;
            else if (column->disabled)
                sub_grid->Items->Item[j]->Hidden = true;
            else if (!FormMain->config->GetSmartFilter())
                sub_grid->Items->Item[j]->Hidden = false;
            else if (filter->HasGroup((unsigned char)column->group))
                sub_grid->Items->Item[j]->Hidden = false;
            else
                sub_grid->Items->Item[j]->Hidden = true;
        }
        sub_grid->FilteredVisibility = true;
        sub_grid->IsUpdating = false;
    }
}

// Shows the row's name and graphic number, with a hint listing the other rows using that graphic.
void DataGridUI::ShowGfxInfo(TElTreeStringGrid *grid, DataGridCol *gfx_column, DataGridCol *id_column)
{
    if (!gfx_id_label)
        return;
    if (!gfx_name_label)
        return;
    if (!gfx_column)
        return;
    if (grid->Selected == NULL)
    {
        gfx_id_label->Caption = "n/a";
        gfx_name_label->Caption = "";
        return;
    }
    int gfx_number = StrToInt(grid->Selected->ColumnText->Strings[id_column->text_index]);
    String item_name(grid->Selected->ColumnText->Strings[gfx_column->text_index]);
    gfx_id_label->Caption = item_name;
    gfx_name_label->Caption = "| GFX : " + IntToStr(gfx_number);
    gfx_name_label->Left = gfx_id_label->Left + gfx_id_label->Width + 8;
    String hint = "Gfx also found in :";
    for (int i = 0; grid->Items->Count > i; i++)
    {
        TElTreeItem *item = grid->Items->Item[i];
        String table = table_name;
        int cell_count = item->ColumnText->Count;
        if (item->ColumnText->Count < 2)
            continue;
        if (item->ColumnText->Strings[id_column->text_index] == IntToStr(gfx_number))
            hint += "\n " + item->Text + " : " + item->ColumnText->Strings[gfx_column->text_index];
    }
    gfx_image->Hint = hint;
}

// Draws one frame of the animation into the gfx image, over what is drawn when asked; false when not found.
bool DataGridUI::DrawFrame(int index, int frame, char drawn)
{
    Graphics::TBitmap *bitmap;
    Graphics::TBitmap *overlay;
    char has_first_frame = FormMain->magic_mapper->base_layer[index - 1];
    char has_second_frame = FormMain->magic_mapper->blend_layer[index - 1];
    char has_third_frame = FormMain->magic_mapper->solid_layer[index - 1];
    int frame_count = FormMain->magic_mapper->GetFrameCount(index - 1);
    if ((!frame && has_first_frame) || (frame == 1 && has_second_frame) || (frame == 2 && has_third_frame))
    {
        try
        {
            int offset = (index - 1) * gfx_step;
            int resource_id = offset + gfx_base;
            HINSTANCE__ *library = gfx_library;
            if (!drawn)
            {
                gfx_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id + frame);
                bitmap = gfx_image->Picture->Bitmap;
                bitmap->Width = gfx_image->Picture->Bitmap->Width / frame_count;
                return true;
            }
            overlay = new Graphics::TBitmap();
            overlay->LoadFromResourceID((unsigned int)library, resource_id + frame);
            overlay->Width = overlay->Width / frame_count;
            // The corner pixel is the frame's background, left out when drawing over.
            int back_color = overlay->Canvas->Pixels[0][0];
            for (int x = 0; x < overlay->Width; x++)
            {
                for (int y = 0; y < overlay->Height; y++)
                {
                    if (overlay->Canvas->Pixels[x][y] != back_color)
                        gfx_image->Picture->Bitmap->Canvas->Pixels[x][y] = overlay->Canvas->Pixels[x][y];
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return true;
    }
    return false;
}

// Draws the three frames of the animation into the gfx image and shows it.
void DataGridUI::ShowFrames(TElTreeStringGrid *grid, int index)
{
    if (!gfx_collapsed && gfx_image)
    {
        if (grid->Selected == NULL)
            gfx_image->Visible = false;
        else
        {
            char drawn = false;
            if (DrawFrame(index, 0, drawn))
                drawn = true;
            if (DrawFrame(index, 1, drawn))
                drawn = true;
            if (DrawFrame(index, 2, drawn))
                drawn = true;
            if (drawn)
            {
            }
            gfx_image->Picture->Bitmap->TransparentColor = clBlack;
            gfx_image->Picture->Bitmap->Transparent = true;
            gfx_image->Top = 70;
            gfx_image->Visible = true;
        }
    }
}

// Loads the graphic into the gfx viewer at three times its size, fitted to the panel and centred.
void DataGridUI::ShowGraphic(TElTreeStringGrid *grid, int index)
{
    int resource_id;
    if (index == shown_graphic)
        return;
    shown_graphic = index;
    if (gfx_collapsed)
        return;
    if (!gfx_image)
        return;
    if (grid->Selected == NULL)
    {
        gfx_image->Visible = false;
        return;
    }
    gfx_image->Visible = true;
    gfx_image->Picture->Bitmap->TransparentColor = clBlack;
    gfx_image->Picture->Bitmap->Transparent = true;
    HINSTANCE__ *library = gfx_library;
    try
    {
        int offset = (index - 1) * gfx_step;
        resource_id = gfx_base + offset;
        gfx_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id);
    }
    catch (...)
    {
        gfx_image->Visible = false;
        gfx_panel->Caption = "( not found in resource )";
        return;
    }
    gfx_panel->Caption = "";
    gfx_image->Visible = true;
    gfx_image->Width = 3 * gfx_image->Picture->Bitmap->Width;
    gfx_image->Height = 3 * gfx_image->Picture->Bitmap->Height;
    if (gfx_image->Height > gfx_panel->Height - 60)
    {
        float scale = (gfx_panel->Height - 60.0) / gfx_image->Height;
        gfx_image->Height = gfx_panel->Height - 60;
        gfx_image->Width = (int)(gfx_image->Width * scale);
    }
    gfx_image->Left = (gfx_panel->Width - gfx_image->Width) / 2;
    gfx_image->Top = (gfx_panel->Height - gfx_image->Height) / 2;
    gfx_image->Top = gfx_image->Top + 24;
}

// Loads the icon at twice its size, the gold icon for item 35, and centres it.
void DataGridUI::ShowIcon(TElTreeStringGrid *grid, int index)
{
    int resource_id;
    Graphics::TBitmap *bitmap;
    int center_y;
    if (gfx_collapsed)
        return;
    if (!icon_image)
        return;
    if (grid->Selected == NULL)
    {
        icon_image->Visible = false;
        return;
    }
    icon_image->Visible = true;
    icon_image->Picture->Bitmap->TransparentColor = clBlack;
    icon_image->Picture->Bitmap->Transparent = true;
    HINSTANCE__ *library = icon_library;
    try
    {
        if (table_name == "items" && index == 35)
            icon_image->Picture->Bitmap = FormMain->imageGold->Picture->Bitmap;
        else
        {
            int offset = (index - 1) * icon_step;
            resource_id = icon_base + offset;
            icon_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id);
        }
    }
    catch (...)
    {
        icon_image->Visible = false;
        return;
    }
    if (icon_transparent == 1)
    {
        bitmap = icon_image->Picture->Bitmap;
        bitmap->Width = icon_image->Picture->Bitmap->Width / 2;
    }
    icon_image->Width = 2 * icon_image->Picture->Bitmap->Width;
    icon_image->Height = 2 * icon_image->Picture->Bitmap->Height;
    center_y = 350;
    if (icon_transparent == 1)
        center_y -= 10;
    icon_image->Left = 360 - icon_image->Width / 2;
    icon_image->Top = center_y - icon_image->Height / 2;
}

void DataGridUI::ShowColumnText(TElTreeStringGrid *grid)
{
    if (!column_label)
        return;
    if (grid->Selected == NULL)
    {
        column_label->Visible = false;
        return;
    }
    for (int section = 1; section < grid->HeaderSections->Count; section++)
    {
        if (grid->HeaderSections->Item[section]->Text == label_column_name)
        {
            column_label->Caption = grid->Selected->ColumnText->Strings[section - 1];
            column_label->Visible = true;
            break;
        }
    }
}

void DataGridUI::ToggleGfxViewer(TControl *button, TPanel *panel)
{
    button->ShowHint = true;
    panel->ShowHint = true;
    if (panel->Height == 385)
    {
        panel->Height = 40;
        button->Hint = String("Expand Gfx Viewer");
        panel->Hint = String("Expand Gfx Viewer");
        gfx_collapsed = true;
    }
    else
    {
        panel->Height = 385;
        button->Hint = String("Collapse Gfx Viewer");
        panel->Hint = String("Collpase Gfx Viewer");
        gfx_collapsed = false;
    }
}

// Deletes the selected row and renumbers the rows after it and the pub data; the grid must be in ID order.
void DataGridUI::DestroyRow(TElTreeStringGrid *grid, int index)
{
    // Renumbering needs the rows in ID order, so a grid sorted otherwise is refused.
    int previous_id = 0;
    for (int i = 0; i < grid->Items->Count; i++)
    {
        TElTreeItem *item = grid->Items->Item[i];
        int id = StrToInt(item->Text);
        if (id < previous_id)
        {
            FormWarning->ShowWarning(FormMain, "Cannot Destroy", "Please sort the maingrid on ID, and try again.");
            return;
        }
        previous_id = id;
    }
    if (grid->Selected == NULL)
        return;
    is_ready = false;
    if (!grid->IsUpdating)
        grid->IsUpdating = true;
    int row = grid->Selected->Index;
    grid->Selected->Delete();
    // Each row after the deleted one moves down one ID.
    for (; grid->Items->Count > row; row++)
    {
        TElTreeItem *next_item = grid->Items->Item[row];
        int next_id = StrToInt(next_item->Text);
        next_id--;
        next_item->Text = WideString(IntToStr(next_id));
    }
    if (table_name == "npcs")
        FormMain->npcs_tab->adapter->DeleteAt(index);
    if (table_name == "shops")
        FormMain->shops_tab->adapter->DeleteAt(index);
    if (table_name == "trainers")
        FormMain->trainers_tab->adapter->DeleteAt(index);
    if (table_name == "inns")
        FormMain->inns_tab->adapter->DeleteAt(index);
    is_ready = true;
    grid->Selected = 0;
    grid->IsUpdating = false;
    ShowMessage(record_name + " ID ( " + IntToStr(index + 1) + " ) destroyed");
}

// Inserts an empty row at the index and renumbers the rows after it and the pub data; needs ID order.
void DataGridUI::InjectRow(TElTreeStringGrid *grid, int index)
{
    TElWideStrings *input_cells;
    TElWideStrings *number_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *source_cells;
    TElWideStrings *link_cells;
    DataComboItem *combo_item;
    TElWideStrings *other_cells;
    DataGridCol *column;
    // Renumbering needs the rows in ID order, so a grid sorted otherwise is refused.
    int previous_id = 0;
    for (int i = 0; i < grid->Items->Count; i++)
    {
        TElTreeItem *item = grid->Items->Item[i];
        int id = StrToInt(item->Text);
        if (id < previous_id)
        {
            FormWarning->ShowWarning(FormMain, "Cannot Inject", "Please sort the maingrid on ID, and try again.");
            return;
        }
        previous_id = id;
    }
    if (grid->Selected == NULL)
        return;
    is_ready = false;
    if (!grid->IsUpdating)
        grid->IsUpdating = true;
    TElTreeItem *new_item = grid->Items->InsertItem(index, 0);
    new_item->Text = WideString(IntToStr(index + 1));
    // The new row gets each column's empty value: blank, -, 1 for a graphic or the first choice.
    for (int j = 1; j < (short)data_grid->ColumnCount(); j++)
    {
        column = data_grid->GetColumn(j);
        if (column->kind == ColumnKind_Text)
        {
            input_cells = new_item->ColumnText;
            input_cells->Add(WideString(""));
        }
        else if (column->kind == ColumnKind_Number)
        {
            number_cells = new_item->ColumnText;
            number_cells->Add(WideString(""));
        }
        else if (column->kind == ColumnKind_Graphic)
        {
            gfx_cells = new_item->ColumnText;
            gfx_cells->Add(WideString("1"));
        }
        else if (column->kind == ColumnKind_LinkSource)
        {
            source_cells = new_item->ColumnText;
            source_cells->Add(WideString(""));
        }
        else if (column->kind == ColumnKind_Link)
        {
            link_cells = new_item->ColumnText;
            link_cells->Add(WideString("-"));
        }
        else if (column->kind == ColumnKind_Combo)
        {
            combo_item = column->GetComboItem(0);
            new_item->ColumnText->Add(WideString(combo_item->text));
        }
        else
        {
            other_cells = new_item->ColumnText;
            other_cells->Add(WideString(""));
        }
    }
    for (int k = index + 1; k < grid->Items->Count; k++)
    {
        TElTreeItem *next_item = grid->Items->Item[k];
        int next_id = StrToInt(next_item->Text);
        next_id++;
        next_item->Text = WideString(IntToStr(next_id));
    }
    if (table_name == "npcs")
        FormMain->npcs_tab->adapter->InsertAt(index);
    if (table_name == "shops")
        FormMain->shops_tab->adapter->InsertAt(index);
    if (table_name == "trainers")
        FormMain->trainers_tab->adapter->InsertAt(index);
    if (table_name == "inns")
        FormMain->inns_tab->adapter->InsertAt(index);
    is_ready = true;
    grid->IsUpdating = false;
    ShowMessage(record_name + " ID ( " + IntToStr(index + 1) + " ) injected to dataset");
}

// Makes the copy buffer, a line for each column.
void DataGridUI::CreateColumnOrder()
{
    if (copied_row)
        return;
    copied_row = new TStringList();
    for (int i = 0; i < (short)data_grid->ColumnCount(); i++)
        copied_row->Add(IntToStr(i));
}

void DataGridUI::CopyRow()
{
    if (!copied_row)
        return;
    if (copied_row->Count < 1)
        return;
    if (main_grid->Selected == NULL)
        return;
    String text = "";
    copied_row->Count;
    String table = table_name;
    for (int section = 0; section < main_grid->HeaderSections->Count; section++)
    {
        if (!section)
            copied_row->Strings[0] = main_grid->Selected->Text;
        if (section >= 1)
            copied_row->Strings[section] = main_grid->Selected->ColumnText->Strings[section - 1];
        DataGridCol *column = data_grid->GetColumn(section);
        text += column->header_text;
        text += " : ";
        text += data_grid->ZeroIfEmpty(copied_row->Strings[section]);
        text += "\n";
    }
    CopyToClipboard(text);
    has_copied_row = true;
    // The message names the copied row by its Name column.
    int name_index = 0;
    for (int i = 1; i < main_grid->HeaderSections->Count; i++)
    {
        if (String(main_grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_index = i - 1;
            break;
        }
    }
    ShowMessage(record_name + " " + String(main_grid->Selected->ColumnText->Strings[name_index]) + " copied to " +
        table_name + " clipboard");
}

void DataGridUI::CopyToClipboard(String text)
{
    OpenClipboard(GetDesktopWindow());
    EmptyClipboard();
    void *memory = GlobalAlloc(GMEM_MOVEABLE, (unsigned int)(text.Length() + 1));
    if (!memory)
    {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(memory), (void *)text.c_str(), (unsigned int)(text.Length() + 1));
    GlobalUnlock(memory);
    SetClipboardData(CF_TEXT, memory);
    CloseClipboard();
    GlobalFree(memory);
}

// Overwrites the selected row and its pub lists with the copied row, after a warning when it has a name.
void DataGridUI::PasteRow()
{
    if (!copied_row)
        return;
    if (main_grid->Selected == NULL)
        return;
    if (!has_copied_row)
        return;
    TElTreeItem *row = main_grid->Selected;
    int name_index = 0;
    for (int section = 1; section < main_grid->HeaderSections->Count - 1; section++)
    {
        if (String(main_grid->HeaderSections->Item[section]->Text) == "Name")
        {
            name_index = section - 1;
            break;
        }
    }
    if (row->ColumnText->Strings[name_index].Length() > 0 && FormMain->config->GetPasteWarning())
    {
        String question =
            "You are about to paste and overwrite " + String(row->ColumnText->Strings[name_index]) + "  with  " +
                copied_row->Strings[name_index + 1] + ", are you sure?";
        FormDeleteWarning->PlaceWarning(FormMain, main_grid->Width, "Paste Overwrite", question, "OVERWRITE");
        if (FormDeleteWarning->ShowModal() != mrOk)
            return;
    }
    if (!main_grid->IsUpdating)
        main_grid->IsUpdating = true;
    is_ready = false;
    for (int i = 0; main_grid->HeaderSections->Count > i; i++)
    {
        if (i >= 1)
            row->ColumnText->Strings[i - 1] = WideString(copied_row->Strings[i]);
    }
    // The lists kept outside the grid go with the row: drops, trades, skills or questions.
    if (table_name == "npcs")
    {
        String npc_source_id = copied_row->Strings[0];
        String npc_target_id(row->Text);
        FormMain->npcs_tab->adapter->CopyLists(npc_source_id, npc_target_id);
    }
    if (table_name == "shops")
    {
        String shop_source_id = copied_row->Strings[0];
        String shop_target_id(row->Text);
        FormMain->shops_tab->adapter->CopyLists(shop_source_id, shop_target_id);
    }
    if (table_name == "trainers")
    {
        String trainer_source_id = copied_row->Strings[0];
        String trainer_target_id(row->Text);
        FormMain->trainers_tab->adapter->CopyLists(trainer_source_id, trainer_target_id);
    }
    if (table_name == "inns")
    {
        String inn_source_id = copied_row->Strings[0];
        String inn_target_id(row->Text);
        FormMain->inns_tab->adapter->CopyLists(inn_source_id, inn_target_id);
    }
    is_ready = true;
    main_grid->IsUpdating = false;
    ShowMessage(record_name + " ID: " + String(row->Text) + " overwritten with " + copied_row->Strings[name_index + 1]);
}

// The digits of the text as a number; 0 when it has none.
DEFINE_DIGITS_ONLY(DataGridUI)

// The text, or empty for 0 unless zeros are shown.
DEFINE_HIDE_ZERO_TEXT(DataGridUI)

// Gives the main grid's rows their grid's colours and selects the first row when none is selected.
void DataGridUI::Unacquire()
{
    if (!main_grid)
        return;
    for (int i = 0; i < main_grid->Items->Count; i++)
        main_grid->Items->Item[i]->ParentColors = true;
    if (main_grid->Selected == NULL)
    {
        if (main_grid->Items->Count > 0)
        {
            main_grid->Items->Item[0]->Selected = true;
            main_grid->Items->Item[0]->Focused = true;
            main_grid->Selected = main_grid->Items->Item[0];
        }
    }
}

void Datagridui_UnusedLiterals1()
{
    "";
}

// Colours the selected row of the main grid and selects the first sub-grid line when none is selected.
void DataGridUI::Acquire()
{
    if (!main_grid)
        return;
    if (main_grid->Selected == NULL)
        return;
    if (FormMain->config->GetTheme() == "dark")
    {
        main_grid->Selected->RowBkColor = (TColor)0xB5B077;
        main_grid->Selected->BkColor = (TColor)0xB5B077;
    }
    else
    {
        main_grid->Selected->RowBkColor = (TColor)0xD6A656;
        main_grid->Selected->BkColor = (TColor)0xD6A656;
    }
    main_grid->Selected->ParentColors = false;
    if (sub_grid->Selected == NULL && sub_grid->Items->Count > 0)
    {
        sub_grid->Items->Item[0]->Selected = true;
        sub_grid->Items->Item[0]->Focused = true;
    }
}

void DataGridUI::ClearRowColors()
{
    if (main_grid)
    {
        for (int i = 0; i < main_grid->Items->Count; i++)
            main_grid->Items->Item[i]->ParentColors = true;
    }
}
