#include <vcl.h>
#pragma hdrstop

#include "UnitSelectSkill.h"
#include "UnitMain.h"
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include "Learndataadapter.h"

#pragma package(smart_init)
#pragma link "ElTreeGrids"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormSelectSkill *FormSelectSkill;

// Makes the form's type menu.
__fastcall TFormSelectSkill::TFormSelectSkill(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    view_menu = new TElPopupMenu((TComponent *)FormMain);
    view_menu->Font->Size = 10;
    view_menu->Font->Name = "MS Sans Serif";
    view_menu->AutoPopup = false;
    view_menu->OwnerDraw = true;
}

// Themes the form when dark and selects the chosen skill's row, scrolled into view.
void __fastcall TFormSelectSkill::FormShow(TObject *Sender)
{
    int top_index;
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        imageChangeSkillView->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Color = (TColor)0x525252;
    }
    if ((char)((int __fastcall (*)(int))FindDynaInst((void *)itemGrid, -68))((int)itemGrid) &&
        itemGrid->Items->Count > 0)
    {
        int row = 0;
        if (selected_id > 0)
        {
            TElTreeItem *item = FindSkill(selected_id);
            row = item->Index;
        }
        itemGrid->Items->Item[row]->Selected = true;
        itemGrid->Items->Item[row]->Focused = true;
        itemGrid->Selected = itemGrid->Items->Item[row];
        Panel1->SetFocus();
        itemGrid->SetFocus();
        if (row > 0)
        {
            top_index = row - 5;
            if (top_index < 0)
                top_index = 0;
            itemGrid->TopIndex = top_index;
        }
    }
}

// Centres the form on the owner, fills the list from the skills tab and shows it modally.
DEFINE_OPEN(TFormSelectSkill)

// Fills the list with each skill's ID, name, type and target, and the type menu with every skill type.
void TFormSelectSkill::FillGrid()
{
    TElMenuItem *all_item;
    TElMenuItem *menu_item;
    itemGrid->IsUpdating = true;
    itemGrid->Items->Clear();
    for (int i = 0; i < FormMain->skills_tab->main_grid->Items->Count; i++)
        itemGrid->Items->AddItem(0);
    DataGridCol *name_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *target_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target");
    view_menu->Items->Clear();
    all_item = new TElMenuItem((TComponent *)view_menu);
    all_item->Caption = WideString("all items");
    all_item->OnClick = ViewMenuClick;
    all_item->Checked = false;
    view_menu->Items->Add(all_item);
    for (int j = 0; j < type_column->ComboItemCount(); j++)
    {
        DataComboItem *combo_item = type_column->GetComboItem(j);
        menu_item = new TElMenuItem((TComponent *)view_menu);
        menu_item->Caption = WideString(combo_item->text);
        menu_item->OnClick = ViewMenuClick;
        menu_item->Checked = false;
        view_menu->Items->Add(menu_item);
    }
    for (int k = 0; FormMain->skills_tab->main_grid->Items->Count > k; k++)
    {
        TElTreeItem *source_item = FormMain->skills_tab->main_grid->Items->Item[k];
        int row = StrToInt(source_item->Text) - 1;
        TElTreeItem *item = itemGrid->Items->Item[row];
        item->Text = source_item->Text;
        item->ColumnText->Add(source_item->ColumnText->Strings[name_column->text_index]);
        item->ColumnText->Add(source_item->ColumnText->Strings[type_column->text_index]);
        item->ColumnText->Add(source_item->ColumnText->Strings[target_column->text_index]);
    }
    itemGrid->IsUpdating = false;
    if (itemGrid->Items->Count < selected_id)
        selected_id = itemGrid->Items->Count;
}

// Aligns the type columns and shows the selected skill on the OK button, or hides the button.
void TFormSelectSkill::AlignColumns()
{
    if (FormMain->config->GetNumberedLists())
    {
        itemGrid->HeaderSections->Item[2]->Alignment = hsaLeft;
        itemGrid->HeaderSections->Item[3]->Alignment = hsaLeft;
    }
    else
    {
        itemGrid->HeaderSections->Item[2]->Alignment = hsaCenter;
        itemGrid->HeaderSections->Item[3]->Alignment = hsaCenter;
    }
    if (itemGrid->Selected != NULL)
    {
        buttonCancel->Left = 56;
        buttonOk->Left = 352;
        labelSelectedSkillName->Caption = itemGrid->Selected->ColumnText->Strings[0];
        buttonOk->Caption = WideString("OK | ID: " + itemGrid->Selected->Text);
        buttonOk->Enabled = true;
        buttonOk->Visible = true;
        return;
    }
    labelSelectedSkillName->Caption = "No Select";
    buttonOk->Caption = WideString("OK");
    buttonCancel->Left = (Panel3->Width - buttonCancel->Width) / 2;
    buttonOk->Enabled = false;
    buttonOk->Visible = false;
}

void __fastcall TFormSelectSkill::itemGridItemFocused(TObject *Sender)
{
    AlignColumns();
}

// Loads the skill's icon into the hint image, sized to fit its panel.
void TFormSelectSkill::ShowSkillImage(int icon)
{
    HINSTANCE__ *library;
    Graphics::TBitmap *bitmap;
    int resource_id;
    int top;
    TImage *image = hintImage;
    int step = 1;
    int first_id = 101;
    TPanel *panel = hintImagePanel;
    library = FormMain->gfx_vault->icons_library;
    if (icon < 1)
        icon = 1;
    resource_id = first_id + (icon - 1) * step;
    image->Picture->Bitmap->TransparentColor = clBlack;
    image->Picture->Bitmap->Transparent = true;
    try
    {
        image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id);
        bitmap = image->Picture->Bitmap;
        bitmap->Width = image->Picture->Bitmap->Width / 2;
    }
    catch (...)
    {
        if (panel)
            panel->Caption = "(n/a)";
        image->Visible = false;
        return;
    }
    image->Visible = true;
    if (3 * image->Picture->Bitmap->Width < panel->Width && 3 * image->Picture->Bitmap->Height < panel->Height)
    {
        image->Width = 3 * image->Picture->Bitmap->Width;
        image->Height = 3 * image->Picture->Bitmap->Height;
    }
    else
    {
        image->Width = 2 * image->Picture->Bitmap->Width;
        image->Height = 2 * image->Picture->Bitmap->Height;
    }
    if (panel)
    {
        panel->Caption = "";
        image->Left = (panel->Width - image->Width) / 2;
        image->Top = (panel->Height - image->Height) / 2;
        top = image->Top;
        if (top < 0)
            image->Top = top / 3;
    }
}

// Checks the chosen type and shows the skills of that type.
void __fastcall TFormSelectSkill::ViewMenuClick(TObject *sender)
{
    TElMenuItem *item;
    if (!view_menu)
        return;
    for (int i = 0; i < view_menu->Items->Count; i++)
        view_menu->Items->Items[i]->Checked = false;
    item = (TElMenuItem *)sender;
    item->Checked = true;
    labelChangeSkillView->Caption = item->Caption;
    if (item->MenuIndex > 0)
        view_caption = item->Caption;
    else
        view_caption = "";
    ShowView(view_caption, searchEdit->Text);
}

// Shows only the skills of the type whose ID or name holds the search text.
void TFormSelectSkill::ShowView(String caption, String filter)
{
    filter = LowerCase(filter);
    String label_caption = caption;
    if (!label_caption.Length())
        label_caption = "all items";
    labelChangeSkillView->Caption = label_caption;
    imageChangeSkillView->Left = labelChangeSkillView->Left + labelChangeSkillView->Width + 8;
    labelSelectedSkillName->Left = imageChangeSkillView->Left + imageChangeSkillView->Width + 8;
    itemGrid->IsUpdating = true;
    for (int i = 0; itemGrid->Items->Count > i; i++)
    {
        TElTreeItem *item = itemGrid->Items->Item[i];
        if (caption != "")
        {
            if (item->ColumnText->Strings[1] != caption)
            {
                item->Hidden = true;
                continue;
            }
        }
        if (filter == "")
            item->Hidden = false;
        else if (item->Text.Pos(filter) > 0)
            item->Hidden = false;
        else if (LowerCase(item->ColumnText->Strings[0]).Pos(filter) > 0)
            item->Hidden = false;
        else
            item->Hidden = true;
    }
    itemGrid->FilteredVisibility = true;
    itemGrid->IsUpdating = false;
    AlignColumns();
}

// The row of the skills tab with the ID text; none when no row has it.
TElTreeItem *TFormSelectSkill::FindSkill(String id_text)
{
    TElTreeStringGrid *grid = FormMain->skills_tab->main_grid;
    for (int i = 0; i < grid->Items->Count; i++)
    {
        TElTreeItem *item = grid->Items->Item[i];
        if (item->Text == id_text)
            return item;
    }
    return 0;
}

// Shows a hint by the row under the mouse: the skill's name, type, icon and the trainers teaching it.
void __fastcall TFormSelectSkill::itemGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    int row_count;
    TElTreeStringGrid *grid = itemGrid;
    int left_edge = 0;
    int right_edge = grid->HeaderSections->Item[0]->Width;
    right_edge += grid->HeaderSections->Item[1]->Width;
    row_count = grid->Items->Count;
    Y -= grid->HeaderHeight;
    if (X < left_edge)
    {
        hintPanel->Visible = false;
        return;
    }
    if (X > right_edge)
    {
        hintPanel->Visible = false;
        return;
    }
    if (Y < 0)
    {
        hintPanel->Visible = false;
        return;
    }
    if (24 * row_count < Y)
    {
        hintPanel->Visible = false;
        return;
    }
    int column = 0;
    int row = Y / grid->DefaultRowHeight;
    int visible_row = row;
    row += grid->TopIndex;
    TElTreeItem *item;
    int shown = 0;
    char found = false;
    for (int i = 0; i < itemGrid->Items->Count; i++)
    {
        item = grid->Items->Item[i];
        if (!item->Hidden)
        {
            if (shown == row)
            {
                found = true;
                break;
            }
            shown++;
        }
    }
    if (!found)
    {
        hintPanel->Visible = false;
        return;
    }
    // The hint stays as it is while the mouse is on the same row.
    if (hint_row == row)
    {
        hintPanel->Visible = true;
        return;
    }
    hint_row = row;
    TElTreeItem *source_item = FindSkill(item->Text);
    if (grid->HeaderSections->Item[0]->Width < X)
        column = 1;
    if (grid->HeaderSections->Item[0]->Width + grid->HeaderSections->Item[1]->Width < X)
        column = 2;
    int hint_left = Panel1->Left;
    int hint_top = Panel1->Top + Panel4->Height;
    hint_left += 20;
    if (column == 1)
        hint_left += grid->HeaderSections->Item[0]->Width;
    if (column == 2)
    {
        hint_left += grid->HeaderSections->Item[0]->Width;
        hint_left += grid->HeaderSections->Item[1]->Width;
    }
    hint_top += 62;
    hint_top += grid->DefaultRowHeight * visible_row;
    DataGridCol *name_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *target_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target");
    String type_text(source_item->ColumnText->Strings[type_column->text_index]);
    hintCaption->Caption = source_item->ColumnText->Strings[name_column->text_index];
    hintIDCaption->Caption = "| ID: " + source_item->Text;
    hintSubString->Caption = type_text;
    hintIDCaption->Left = hintCaption->Left + hintCaption->Width + 8;
    hintPanel->Width = 337;
    int hint_width = hintIDCaption->Left + hintIDCaption->Width + 24;
    if (hint_width > hintPanel->Width)
        hintPanel->Width = hint_width;
    if (LowerCase(type_text).Pos("armor") > 0)
    {
        String target_text(source_item->ColumnText->Strings[target_column->text_index]);
    }
    hintLearnCountString->Caption =
        IntToStr(FormMain->trainers_tab->adapter->SkillLearnCount(StrToInt(source_item->Text)));
    ShowSkillImage(FormMain->NumberOr(source_item->ColumnText->Strings[gfx_column->text_index], 0));
    hintPanel->Left = hint_left;
    hintPanel->Top = hint_top;
    hintPanel->Visible = true;
}

void __fastcall TFormSelectSkill::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    hintPanel->Visible = false;
}

void __fastcall TFormSelectSkill::itemGridSortBegin(TObject *Sender)
{
    hintPanel->Visible = false;
}

void __fastcall TFormSelectSkill::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Keeps the selected skill's ID and name and closes the form.
DEFINE_FORM_BUTTON_OK_CLICK(TFormSelectSkill)

// Pops up the type menu below the type label.
void __fastcall TFormSelectSkill::labelChangeSkillViewClick(TObject *Sender)
{
    int menu_left = Panel1->Left + Left + labelChangeSkillView->Left;
    int menu_top = Panel1->Top + Top + labelChangeSkillView->Top + 52;
    view_menu->Popup(menu_left, menu_top);
}

void __fastcall TFormSelectSkill::searchEditChange(TObject *Sender)
{
    ShowView(view_caption, searchEdit->Text);
}

void __fastcall TFormSelectSkill::itemGridDblClick(TObject *Sender)
{
    buttonOkClick((TObject *)this);
}

// Escape closes the form and Enter takes the selected skill, once the opening pause is over.
DEFINE_BUTTON_OK_KEY_UP(TFormSelectSkill)

// Ends the short pause after opening in which a released key is ignored.
void __fastcall TFormSelectSkill::Timer1Timer(TObject *Sender)
{
    Timer1->Enabled = false;
}
