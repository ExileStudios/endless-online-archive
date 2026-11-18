#include <vcl.h>
#pragma hdrstop

#include "Dataform.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include <ctype.h>
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Datalinkitem.h"
#include <Graphics.hpp>

#pragma package(smart_init)

DataForm::DataForm(String title, DataGridUI *ui, int tree, TElTreeStringGrid *grid)
{
    name = title;
    main_grid = (TCustomElTree *)tree;
    form_grid = grid;
    grid_ui = ui;
    CreateEditors();
}

DataForm::~DataForm()
{
}

void DataForm::SetGraphics(HINSTANCE__ *library, int first, int step)
{
    gfx_first = first;
    gfx_step = step;
    gfx_library = library;
    transparent = false;
}

void DataForm::SetSecondGraphics(HINSTANCE__ *library, int first, int step)
{
    animation_first = first;
    animation_step = step;
    animation_library = library;
}

void DataForm::SetTransparent(char on)
{
    transparent = on;
}

// Loads the graphic for the number into the image at twice its size, centred, and shows its resource id.
bool DataForm::ShowGraphic(TPanel *panel, TImage *image, TLabel *label, int number)
{
    Graphics::TBitmap *bitmap;
    if (number < 1)
        number = 1;
    int resource_id = (number - 1) * gfx_step + gfx_first;
    image->Picture->Bitmap->TransparentColor = clBlack;
    image->Picture->Bitmap->Transparent = true;
    label->Caption = "res.id:" + String(resource_id);
    try
    {
        image->Picture->Bitmap->LoadFromResourceID((unsigned int)gfx_library, resource_id);
    }
    catch (...)
    {
        if (panel)
            panel->Caption = "(not found)";
        image->Visible = false;
        label->Caption = "error: " + String(resource_id);
        label->Font->Color = clRed;
        return false;
    }
    if (transparent)
    {
        bitmap = image->Picture->Bitmap;
        bitmap->Width = image->Picture->Bitmap->Width / 2;
    }
    image->Visible = true;
    label->Font->Color = (TColor)0x808040;
    image->Width = 2 * image->Picture->Bitmap->Width;
    image->Height = 2 * image->Picture->Bitmap->Height;
    if (panel)
    {
        panel->Caption = "";
        image->Left = (panel->Width - image->Width) / 2;
        image->Top = (panel->Height - image->Height) / 2;
    }
    return true;
}

// Draws the animation for the number into the image and shows its id, in red when not found.
void DataForm::ShowAnimation(TPanel *panel, TImage *image, TLabel *label, int number)
{
    if (DrawFrames(image, (int)main_grid, number))
    {
        label->Visible = true;
        panel->Caption = "";
        label->Font->Color = clBlack;
        label->Caption = "ani.id: " + IntToStr(3 * number);
    }
    else
    {
        label->Font->Color = clRed;
        label->Caption = "ani.id: " + IntToStr(3 * number);
        panel->Caption = "(resource not found)";
    }
}

// Draws the three frames of the animation into the image; false and hidden when none is found.
bool DataForm::DrawFrames(TImage *image, int tree, int number)
{
    char drawn = false;
    if (DrawFrame(image, number, 0, drawn))
        drawn = true;
    if (DrawFrame(image, number, 1, drawn))
        drawn = true;
    if (DrawFrame(image, number, 2, drawn))
        drawn = true;
    if (!drawn)
    {
        image->Visible = false;
        return false;
    }
    image->Picture->Bitmap->TransparentColor = clBlack;
    image->Picture->Bitmap->Transparent = true;
    image->Visible = true;
    return true;
}

// Draws one frame of the animation into the image, over what is drawn when asked; false when not found.
bool DataForm::DrawFrame(TImage *image, int number, int frame, char drawn)
{
    Graphics::TBitmap *bitmap;
    Graphics::TBitmap *overlay;
    char has_first_frame = FormMain->magic_mapper->base_layer[number - 1];
    char has_second_frame = FormMain->magic_mapper->blend_layer[number - 1];
    char has_third_frame = FormMain->magic_mapper->solid_layer[number - 1];
    int frame_count = FormMain->magic_mapper->GetFrameCount(number - 1);
    if ((!frame && has_first_frame) || (frame == 1 && has_second_frame) || (frame == 2 && has_third_frame))
    {
        try
        {
            int offset = 3 * number - 3;
            int resource_id = offset + 101;
            HINSTANCE__ *library = animation_library;
            if (!drawn)
            {
                image->Picture->Bitmap->Width = 0;
                image->Picture->Bitmap->Height = 0;
                image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id + frame);
                bitmap = image->Picture->Bitmap;
                bitmap->Width = image->Picture->Bitmap->Width / frame_count;
                return true;
            }
            overlay = new Graphics::TBitmap();
            overlay->LoadFromResourceID((unsigned int)library, resource_id + frame);
            overlay->Width = overlay->Width / frame_count;
            int back_color = overlay->Canvas->Pixels[0][0];
            for (int x = 0; x < overlay->Width; x++)
            {
                for (int y = 0; y < overlay->Height; y++)
                {
                    if (overlay->Canvas->Pixels[x][y] != back_color)
                        image->Picture->Bitmap->Canvas->Pixels[x][y] = overlay->Canvas->Pixels[x][y];
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

// Fills the list with the choices of the named column.
void DataForm::FillCombo(TElComboBox *combo, String column_name)
{
    DataGridCol *column = grid_ui->data_grid->FindColumn(column_name);
    if (!column)
        return;
    combo->Items->Clear();
    for (int i = 0; i < column->ComboItemCount(); i++)
    {
        DataComboItem *item = column->GetComboItem(i);
        combo->Items->Add(WideString(item->text));
    }
}

// Selects the form grid's first line when none is selected.
void DataForm::SelectFirstRow()
{
    if (form_grid && form_grid->Selected == NULL && form_grid->Items->Count > 0)
    {
        form_grid->Items->Item[0]->Selected = true;
        form_grid->Items->Item[0]->Focused = true;
    }
}

// Picks the editor for a form line by its column's kind; the kind's variant of the column with smart editors.
char DataForm::EditCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor, String column_name)
{
    TElWideStrings *error_items;
    if (!form_grid)
        return 0;
    if (form_grid->Selected == NULL)
        return 0;
    edit_section = section;
    edit_row = form_grid->Selected->Index;
    DataGridCol *column = grid_ui->data_grid->GetColumn(form_grid->Selected->Tag);
    edit_column = column;
    column = column->FindSubColumn(column_name);
    if (column && FormMain->config->GetSmartEditors())
        edit_column = column;
    if (edit_column->disabled)
    {
        editor = NULL;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Number)
    {
        spin_editor->Editor->MinValue = edit_column->min_value;
        if (edit_column->max_value == 252)
        {
            spin_editor->Editor->MaxValue = 9999;
            spin_editor->Editor->Tag = 252;
        }
        if (edit_column->max_value == 64008)
        {
            spin_editor->Editor->MaxValue = 999999;
            spin_editor->Editor->Tag = 64008;
        }
        if (edit_column->max_value == 0xF71AE3)
        {
            spin_editor->Editor->MaxValue = 999999999;
            spin_editor->Editor->Tag = 0xF71AE3;
        }
        editor = spin_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_Combo || edit_column->kind == ColumnKind_SmartCombo)
    {
        combo_editor->Editor->Items->Clear();
        for (int i = 0; i < edit_column->ComboItemCount(); i++)
        {
            DataComboItem *combo_item = edit_column->GetComboItem(i);
            combo_editor->Editor->Items->Add(WideString(combo_item->text));
        }
        editor = combo_editor;
        for (int j = 0; combo_editor->Editor->Items->Count > j; j++)
        {
            if (combo_editor->Editor->Items->Strings[j] == edit_column->header_text)
                combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    if (edit_column->kind == ColumnKind_Link)
    {
        combo_editor->Editor->Items->Clear();
        if (edit_column->LinkCount() > 0)
        {
            DataLinkItem *link = edit_column->GetLink(0);
            TElTreeStringGrid *link_grid = link->grid;
            short link_section = link->FindColumnIndex();
            if (link_section < 0)
            {
                error_items = combo_editor->Editor->Items;
                error_items->Add(WideString("[LCERR]"));
            }
            else
            {
                error_items = combo_editor->Editor->Items;
                error_items->Add(WideString("-"));
                for (int row = 0; link_grid->Items->Count > row; row++)
                {
                    DataComboItem *link_combo_item = edit_column->GetComboItem(row);
                    combo_editor->Editor->Items->Add(WideString(String(link_grid->Items->Item[row]->ColumnText->Strings[link_section])));
                }
            }
        }
        editor = combo_editor;
        for (int k = 0; combo_editor->Editor->Items->Count > k; k++)
        {
            if (combo_editor->Editor->Items->Strings[k] == edit_column->header_text)
                combo_editor->Editor->Text = WideString(edit_column->header_text);
        }
        return 0;
    }
    if (edit_column->kind == ColumnKind_Text)
    {
        text_editor->Editor->MaxLength = edit_column->max_value;
        editor = text_editor;
        return 0;
    }
    if (edit_column->kind == ColumnKind_PaperdollGraphic)
    {
        editor = NULL;
        return ColumnKind_PaperdollGraphic;
    }
    editor = NULL;
    return 0;
}

// Makes the text, spin and list editors of the form grid, dark-themed when set, and hooks their events.
void DataForm::CreateEditors()
{
    text_editor = new TElTreeInplaceEdit((TComponent *)form_grid);
    spin_editor = new TElTreeInplaceSpinEdit((TComponent *)form_grid);
    combo_editor = new TElTreeInplaceComboBox((TComponent *)form_grid);
    if (FormMain->config->GetTheme() == "dark")
    {
        text_editor->Editor->UseXPThemes = false;
        text_editor->Editor->Flat = true;
        text_editor->Editor->ParentColor = true;
        text_editor->Editor->ParentFont = true;
        text_editor->Editor->BorderStyle = bsNone;
        text_editor->Editor->Font->Size = form_grid->Font->Size;
        text_editor->Editor->Font->Color = (TColor)0xDDD164;
        combo_editor->Editor->UseXPThemes = false;
        combo_editor->Editor->Flat = true;
        combo_editor->Editor->BorderStyle = bsNone;
        combo_editor->Editor->ListColor = (TColor)0x3D3D3D;
        combo_editor->Editor->ButtonColor = (TColor)0x555555;
        combo_editor->Editor->ButtonWidth = 24;
        combo_editor->Editor->ButtonFlat = true;
        combo_editor->Editor->ListTransparentSelection = true;
        combo_editor->Editor->Color = (TColor)0x5E5E5E;
        spin_editor->Editor->UseXPThemes = false;
        spin_editor->Editor->Flat = true;
        spin_editor->Editor->BorderStyle = bsNone;
        spin_editor->Editor->ParentColor = true;
        spin_editor->Editor->ParentFont = true;
        spin_editor->Editor->Font->Size = form_grid->Font->Size;
        spin_editor->Editor->Font->Color = (TColor)0xDDD164;
    }
    text_editor->Tree = (TCustomElTree *)form_grid;
    spin_editor->Tree = (TCustomElTree *)form_grid;
    combo_editor->Tree = (TCustomElTree *)form_grid;
    combo_editor->InitiallyDropped = true;
    combo_editor->Editor->DropDownCount = 28;
    combo_editor->Editor->AutoCompletion = true;
    text_editor->OnBeforeOperation = EditorKeyDown;
    spin_editor->OnBeforeOperation = EditorKeyDown;
    combo_editor->OnBeforeOperation = EditorKeyDown;
    text_editor->OnValidateResult = TextEditorAfter;
    spin_editor->OnValidateResult = SpinEditorAfter;
    combo_editor->OnValidateResult = ComboEditorAfter;
    text_editor->OnAfterOperation = TextEditorValidate;
    spin_editor->OnAfterOperation = SpinEditorValidate;
    combo_editor->OnAfterOperation = ComboEditorValidate;
    spin_editor->Editor->OnKeyPress = SpinEditorKeyPress;
}

void __fastcall DataForm::SpinEditorKeyPress(TObject *sender, char &key)
{
}

// With mouse quick save on, keeps the grid, row and text being edited for a later save.
void __fastcall DataForm::EditorKeyDown(TObject *sender, bool &handled)
{
    if (!FormMain->config->GetMouseQuickSave())
        return;
    if (!form_grid)
        return;
    if (form_grid->Selected == NULL)
        return;
    FormMain->edit_grid = form_grid;
    FormMain->edit_row = form_grid->Selected;
    FormMain->edit_column = 0;
    FormMain->edit_backup = form_grid->Selected->ColumnText->Strings[0];
}

// Trims the text; with mouse quick save, refuses a text too short or EOF and capitalizes when set.
void __fastcall DataForm::TextEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
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

// Keeps the number within the editor's limit.
void __fastcall DataForm::SpinEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
    if (FormMain->config->GetMouseQuickSave())
        accept = true;
    TElTreeInplaceSpinEdit *spin_editor = (TElTreeInplaceSpinEdit *)sender;
    if (spin_editor->Editor->Tag < spin_editor->Editor->Value)
        spin_editor->Editor->Value = spin_editor->Editor->Tag;
}

// Takes the typed text as a choice: by name, by part of a name, by number or as [OOB-n]; else refuses it.
void __fastcall DataForm::ComboEditorValidate(TObject *sender, bool &accept, bool &default_op)
{
    int i;
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
    if (text.Length() > 2)
    {
        for (i = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > i; i++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i]) == lower_text)
            {
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

// Refuses a text too short or EOF and applies the column's letter case.
void __fastcall DataForm::TextEditorAfter(TObject *sender, bool &changed)
{
    TElTreeInplaceEdit *text_editor = (TElTreeInplaceEdit *)sender;
    String text(text_editor->Editor->Text);
    if (text.Length() <= 1 && !FormMain->config->GetExpertMode())
        changed = false;
    if (LowerCase(text) == "eof")
    {
        String message = "EOF tags are not needed. The Endless parser supports EOF automatically once needed.";
        FormWarning->ShowWarning(FormMain, "About EOF..", message);
        changed = false;
    }
    if (edit_column->text_format == ColumnFormat_Capitalized && text.Length() > 0 &&
        FormMain->config->GetAutoCapitalization())
    {
        text[1] = toupper(text[1]);
        text_editor->Editor->Text = WideString(text);
    }
    if (edit_column->text_format == ColumnFormat_Lower && text.Length() > 0)
    {
        text = LowerCase(text);
        text_editor->Editor->Text = WideString(text);
    }
    if (edit_column->text_format == ColumnFormat_Upper && text.Length() > 0)
    {
        text = UpperCase(text);
        text_editor->Editor->Text = WideString(text);
    }
}

// An empty number becomes 0 and a number past the limit becomes the limit.
DEFINE_SPIN_EDITOR_AFTER(DataForm)

// Settles the chosen text: by name, by part of a name, by number or as [OOB-n]; else no change.
void __fastcall DataForm::ComboEditorAfter(TObject *sender, bool &changed)
{
    int i;
    ((TElTreeInplaceComboBox *)sender)->Editor->ItemIndex =
        ((TElTreeInplaceComboBox *)sender)->Editor->SelectedItemIndex;
    String text(((TElTreeInplaceComboBox *)sender)->Editor->Text);
    if (text.Length() < 0)
    {
        changed = false;
        return;
    }
    String lower_text = LowerCase(text);
    if (text.Length() > 2)
    {
        for (i = 0; ((TElTreeInplaceComboBox *)sender)->Editor->Items->Count > i; i++)
        {
            if (LowerCase(((TElTreeInplaceComboBox *)sender)->Editor->Items->Strings[i]) == lower_text)
            {
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

// Fills the grid with a line per column, shown by the kind's groups, holding defaults or the selected row.
int DataForm::FillGrid(TElTreeStringGrid *grid, DataComboItem *kind, unsigned char operation, String kind_text)
{
    DataGridCol *column;
    TElWideStrings *id_cells;
    TElWideStrings *value_cells;
    DataGridCol *sub_column;
    TElWideStrings *zero_cells;
    TElWideStrings *link_cells;
    TElWideStrings *gfx_cells;
    TElWideStrings *other_cells;
    DataComboItem *combo_item;
    int shown;
    int section;
    TElTreeItem *item;
    shown = 0;
    if (!main_grid)
        return 0;
    if (operation == 1 && main_grid->Selected == NULL)
        return 0;
    if (!grid->IsUpdating)
        grid->IsUpdating = true;
    grid->Items->Clear();
    for (section = 0; ((TElTreeStringGrid *)main_grid)->HeaderSections->Count > section; section++)
    {
        String header_text = ((TElTreeStringGrid *)main_grid)->HeaderSections->Item[section]->Text;
        column = grid_ui->data_grid->GetColumn(section);
        sub_column = column->FindSubColumn(kind_text);
        if (sub_column && FormMain->config->GetSmartEditors())
            column = sub_column;
        item = grid->Items->AddItem(0);
        item->Text = WideString(column->caption_text);
        item->Hint = WideString(grid_ui->data_grid->ColumnWidth(column));
        item->Tag = column->index;
        if (!column->group || column->disabled)
            grid->Items->Item[section]->Hidden = true;
        else if (!kind)
        {
            grid->Items->Item[section]->Hidden = false;
            shown++;
        }
        else if (kind->HasGroup((unsigned char)column->group))
        {
            grid->Items->Item[section]->Hidden = false;
            shown++;
        }
        else
            grid->Items->Item[section]->Hidden = true;
        // A new record shows each column's default; an existing one its row's values.
        if (!operation)
        {
            if (column->kind == ColumnKind_Number)
            {
                String min_text = IntToStr((unsigned int)column->min_value);
                TElWideStrings *number_cells = item->ColumnText;
                number_cells->Add(WideString(min_text));
                if (!FormMain->config->GetShowZeros() && min_text == "0")
                {
                    zero_cells = item->ColumnText;
                    zero_cells->Strings[0] = WideString("");
                }
            }
            else if (column->kind == ColumnKind_Combo)
            {
                DataComboItem *first_item = column->GetComboItem(0);
                item->ColumnText->Add(WideString(first_item->text));
            }
            else if (column->kind == ColumnKind_SmartCombo)
            {
                DataComboItem *first_smart_item = column->GetComboItem(0);
                item->ColumnText->Add(WideString(first_smart_item->text));
            }
            else if (column->kind == ColumnKind_Link)
            {
                DataLinkItem *link = column->GetLink(0);
                short link_column_index = link->FindColumnIndex();
                link_cells = item->ColumnText;
                link_cells->Add(WideString("-"));
            }
            else if (column->kind == ColumnKind_PaperdollGraphic)
            {
                gfx_cells = item->ColumnText;
                gfx_cells->Add(WideString(NumberText(1)));
            }
            else
            {
                other_cells = item->ColumnText;
                other_cells->Add(WideString(NumberText(0)));
            }
        }
        if (operation == 1)
        {
            if (!section)
            {
                id_cells = item->ColumnText;
                id_cells->Add(main_grid->Selected->Text);
            }
            else
            {
                String cell_text(main_grid->Selected->ColumnText->Strings[section - 1]);
                if (column->kind == ColumnKind_SmartCombo)
                {
                    combo_item = column->GetComboItem(DigitsOnly(cell_text));
                    if (combo_item)
                        cell_text = combo_item->text;
                }
                value_cells = item->ColumnText;
                value_cells->Add(WideString(cell_text));
            }
        }
    }
    grid->FilteredVisibility = true;
    grid->IsUpdating = false;
    grid->Update();
    return shown;
}

// The digits of the text as a number; 0 when it has none.
DEFINE_DIGITS_ONLY(DataForm)

// The text, or empty for 0 unless zeros are shown.
DEFINE_HIDE_ZERO_TEXT(DataForm)

// The number as text; empty for 0 or less unless zeros are shown.
DEFINE_NUMBER_TEXT(DataForm)
