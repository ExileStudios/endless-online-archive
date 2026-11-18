#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitAbout.h"
#include "UnitGfx.h"
#include "UnitEditItem.h"
#include "UnitPathConfig.h"
#include "UnitEditClass.h"
#include "UnitConfiguration.h"
#include "UnitExportForm.h"
#include "UnitWarning.h"
#include "UnitWarningReload.h"
#include "UnitWarningClsCorruption.h"
#include "UnitWarningOutOfBound.h"
#include "UnitWarningExit.h"
#include "UnitToolsItems.h"
#include "UnitWarningDelete.h"
#include "UnitEditNpc.h"
#include "UnitEditSkill.h"
#include "UnitPubfileStatus.h"
#include "UnitToolsNpcs.h"
#include "UnitToolsSkills.h"
#include "UnitToolsClasses.h"
#include "UnitWarningCannotOpen.h"
#include "UnitEditNpcChats.h"
#include "UnitEditNpcDrops.h"
#include "UnitEditShop.h"
#include "UnitEditTrainer.h"
#include "UnitEditShopItems.h"
#include "UnitEditTrainerSkills.h"
#include "UnitEditShopCrafts.h"
#include "UnitToolsShops.h"
#include "UnitToolsTrainers.h"
#include "UnitEditInn.h"
#include "UnitEditInnQuiz.h"
#include "UnitToolsInns.h"
#include "Classvalues.h"
#include "Datacomboitem.h"
#include "Datagrid.h"
#include "Innvalues.h"
#include "Itemvalues.h"
#include "Learnvalues.h"
#include "Npcvalues.h"
#include "Pubfileheader.h"
#include "Shopvalues.h"
#include "Skillvalues.h"

#pragma package(smart_init)
#pragma link "ElPgCtl"
#pragma link "ElTreeGrids"
#pragma link "ElMenus"
#pragma link "ElSysHotKey"
#pragma resource "*.dfm"

TFormMain *FormMain;

// Reads the settings, sets the version caption and swaps in the dark or default images.
__fastcall TFormMain::TFormMain(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    version = "0.3G";
    Caption = " EO Pub " + version;
    unused_flag = false;
    caption_altered = false;
    timer_ticks = 0;
    config = new Config();
    config->LoadSettings();
    if (config->GetFullscreen())
        WindowState = wsMaximized;
    magic_mapper = new MagicMapper();
    started = false;
    edit_grid = 0;
    edit_row = 0;
    combo_text = "";
    edit_backup = "";
    EnableHotkeys(config->GetHotkeys());
    // Dark mode borrows the dark copies of the images and glyphs the tabs share.
    if (config->GetTheme() == "dark")
    {
        imageTile->Picture->Bitmap = imageTile2->Picture->Bitmap;
        imageItem->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageNpc->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageSkill->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageShop->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageTrainer->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageInn->Picture->Bitmap = imageClass->Picture->Bitmap;
        imageChangeItemView->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageItemInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageItemGfx->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageChangeNpcFilterType->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageNpcInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageNpcGfx->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageChangeSkillView->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageSkillInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageSkillGfx->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageChangeClassFilterType->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageClassInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageChangeShopFilterType->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageShopInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageChangeTrainerFilterType->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        imageTrainerInspector->Picture->Bitmap = imageDarkArrow->Picture->Bitmap;
        buttonToolsItem->Glyph = buttonToolsClass->Glyph;
        buttonToolsNpc->Glyph = buttonToolsClass->Glyph;
        buttonToolsSkill->Glyph = buttonToolsClass->Glyph;
        buttonToolsShop->Glyph = buttonToolsClass->Glyph;
        buttonToolsTrainer->Glyph = buttonToolsClass->Glyph;
        darkPanel1->Visible = true;
        darkPanel2->Visible = true;
        darkPanel3->Visible = true;
        darkPanel4->Visible = true;
        darkPanel5->Visible = true;
        darkPanel6->Visible = true;
        ThemeComponents(this);
    }
    else
    {
        imageTile2->Picture->Bitmap = imageTile->Picture->Bitmap;
        imageNpc->Picture->Bitmap = imageItem->Picture->Bitmap;
        imageClass->Picture->Bitmap = imageItem->Picture->Bitmap;
        imageSkill->Picture->Bitmap = imageItem->Picture->Bitmap;
        imageShop->Picture->Bitmap = imageItem->Picture->Bitmap;
        imageTrainer->Picture->Bitmap = imageItem->Picture->Bitmap;
        imageInn->Picture->Bitmap = imageItem->Picture->Bitmap;
        buttonToolsNpc->Glyph = buttonToolsItem->Glyph;
        buttonToolsSkill->Glyph = buttonToolsItem->Glyph;
        buttonToolsClass->Glyph = buttonToolsItem->Glyph;
        buttonToolsShop->Glyph = buttonToolsItem->Glyph;
        buttonToolsTrainer->Glyph = buttonToolsItem->Glyph;
    }
    if (Caption.Pos("EO") <= 0)
        caption_altered = true;
}

// Notes whether the first timer tick has still to set the editor up.
void __fastcall TFormMain::FormShow(TObject *Sender)
{
    started = false;
    if (FormAbout->Label5->Caption[5] != '-')
        started = true;
}

// The hotkeys are off while another program has the focus.
void __fastcall TFormMain::ApplicationEvents1Deactivate(TObject *Sender)
{
    if (config->GetHotkeys())
        EnableHotkeys(false);
}

void __fastcall TFormMain::ApplicationEvents1Activate(TObject *Sender)
{
    if (config->GetHotkeys())
        EnableHotkeys(true);
}

// Gives every component of the form the dark theme colours of its kind.
void TFormMain::ThemeComponents(TForm *owner)
{
    for (int i = 0; owner->ComponentCount > i; i++)
    {
        TComponent *component = owner->Components[i];
        String class_name = TObject::ClassName(((TObject *)component)->ClassType());
        // Each kind of component swaps its light colours for their dark ones.
        if (class_name == "TPanel")
        {
            TPanel *panel = (TPanel *)component;
            panel->Font->Color = (TColor)0x999999;
            if (panel->Color == clWhite)
                panel->Color = (TColor)0x373737;
            if (panel->Color == clBtnFace)
                panel->Color = (TColor)0x525252;
            if (panel->Color == 0xF5F5F5)
                panel->Color = (TColor)0x414141;
            if (panel->Color == 0xE9E9E9)
                panel->Color = (TColor)0x444343;
            if (panel->Color == 0xF7F7F7)
                panel->Color = (TColor)0x494949;
            if (panel->Color == 0xFCFCFC)
                panel->Color = (TColor)0x666666;
            if (panel->Color == 0xF8F8F8)
                panel->Color = (TColor)0x373737;
            if (panel->Color == 0xFBFBFB)
                panel->Color = (TColor)0x525252;
            continue;
        }
        if (class_name == "TLabel")
        {
            TLabel *label = (TLabel *)component;
            if (label->Font->Color == clWindowText)
                label->Font->Color = (TColor)0xBBBBBB;
            if (label->Font->Color == 0xE5E5E5)
                label->Font->Color = (TColor)0x2A2A2A;
            if (label->Font->Color == 0xDFDFDF)
                label->Font->Color = (TColor)0x525252;
            if (label->Font->Color == 0x808040)
                label->Font->Color = (TColor)0xC5C589;
            if (label->Font->Color == clTeal)
                label->Font->Color = (TColor)0xD2C96F;
            if (label->Color == clBtnFace)
                label->Color = (TColor)0x525252;
            if (label->Color == 0xE9E9E9)
                label->Color = (TColor)0x444343;
            if (label->Color == 0xFBFBFB)
                label->Color = (TColor)0x444343;
            continue;
        }
        if (class_name == "TEdit")
        {
            TEdit *edit = (TEdit *)component;
            if (edit->Tag == 601)
            {
                edit->Color = (TColor)0x666666;
                edit->Font->Color = (TColor)0xDDD164;
            }
            else
            {
                edit->Color = (TColor)0x373737;
                edit->Font->Color = (TColor)0xDDD164;
            }
            continue;
        }
        if (class_name == "TSpeedButton")
        {
            TSpeedButton *speed_button = (TSpeedButton *)component;
            speed_button->Font->Color = (TColor)0xDDD164;
            continue;
        }
        if (class_name == "TShape")
        {
            TShape *shape = (TShape *)component;
            shape->Pen->Color = (TColor)0xAAAAAA;
            continue;
        }
        if (class_name == "TForm")
        {
            TForm *form = (TForm *)component;
            form->Color = (TColor)0x373737;
            continue;
        }
        if (class_name == "TMemo")
        {
            TMemo *memo = (TMemo *)component;
            memo->Font->Color = (TColor)0xAAAAAA;
            memo->Color = (TColor)0x373737;
            continue;
        }
        if (class_name == "TListView")
        {
            TListView *list_view = (TListView *)component;
            list_view->Font->Color = (TColor)0xAAAAAA;
            list_view->Color = (TColor)0x373737;
            continue;
        }
        if (class_name == "TElSpinEdit")
        {
            TElSpinEdit *spin_edit = (TElSpinEdit *)component;
            spin_edit->UseXPThemes = false;
            spin_edit->Color = (TColor)0x5E5E5E;
            spin_edit->Font->Color = (TColor)0xDDD164;
            continue;
        }
        if (class_name == "TElSpinEdit")
        {
            TElSpinEdit *spin_box = (TElSpinEdit *)component;
            spin_box->UseXPThemes = false;
            spin_box->Color = (TColor)0x5E5E5E;
            spin_box->Font->Color = (TColor)0xDDD164;
        }
        if (class_name == "TElComboBox")
        {
            TElComboBox *combo_box = (TElComboBox *)component;
            combo_box->UseXPThemes = false;
            combo_box->ListColor = (TColor)0x4C4C4C;
            combo_box->ListTransparentSelection = true;
            combo_box->Color = (TColor)0x5E5E5E;
            combo_box->Font->Color = (TColor)0xDDD164;
            combo_box->ButtonColor = (TColor)0x6E6E6E;
            combo_box->ButtonFlat = true;
            combo_box->VertScrollBarStyles->UseXPThemes = false;
            combo_box->VertScrollBarStyles->SystemBkColor = true;
            combo_box->VertScrollBarStyles->BarColor = (TColor)0x666666;
            combo_box->VertScrollBarStyles->Color = (TColor)0x666666;
            combo_box->VertScrollBarStyles->HintColor = (TColor)0x373737;
            combo_box->VertScrollBarStyles->SystemBkColor = false;
            continue;
        }
        if (class_name == "TElPageControl")
        {
            TElPageControl *page_control = (TElPageControl *)component;
            page_control->Font->Color = (TColor)0xAAAAAA;
            page_control->HotTrackFont->Color = (TColor)0x6A4F9;
            page_control->Color = (TColor)0x373737;
            page_control->TabBkColor = (TColor)0x373737;
            page_control->ActiveTabColor = (TColor)0x595959;
            page_control->InactiveTabColor = (TColor)0x484848;
            page_control->FlatTabBorderColor = clBlack;
            page_control->Style = etsFlatTabs;
        }
        if (class_name == "TElPopupButton")
        {
            TElPopupButton *popup_button = (TElPopupButton *)component;
            if (popup_button->Color == 0xE9E9E9)
            {
                popup_button->Color = (TColor)0x444343;
                popup_button->MoneyFlatDownColor = (TColor)0x444343;
                popup_button->MoneyFlatInactiveColor = (TColor)0x444343;
                popup_button->MoneyFlatActiveColor = (TColor)0x5D5D5D;
                popup_button->Font->Color = (TColor)0xCCCCCC;
            }
            if (popup_button->Color == clBtnFace)
            {
                popup_button->Color = (TColor)0x525252;
                popup_button->MoneyFlatDownColor = (TColor)0x525252;
                popup_button->MoneyFlatInactiveColor = (TColor)0x525252;
                popup_button->MoneyFlatActiveColor = (TColor)0x5D5D5D;
                popup_button->Font->Color = (TColor)0xCCCCCC;
            }
        }
        if (class_name == "TElTabSheet")
        {
            TElTabSheet *tab_sheet = (TElTabSheet *)component;
            tab_sheet->Color = (TColor)0x525252;
        }
        if (class_name == "TElCheckBox")
        {
            TElCheckBox *check_box = (TElCheckBox *)component;
            check_box->UseXPThemes = false;
            check_box->Font->Color = (TColor)0xD5D8C2;
        }
        if (class_name == "TElBiProgressBar")
        {
            TElBiProgressBar *progress_bar = (TElBiProgressBar *)component;
            progress_bar->Color = (TColor)0x4C4C4C;
            progress_bar->LightColor = (TColor)0xBBB986;
        }
        if (class_name == "TElScrollBar")
        {
            TElScrollBar *scroll_bar = (TElScrollBar *)component;
            scroll_bar->UseXPThemes = false;
            scroll_bar->SystemBkColor = true;
            scroll_bar->BarColor = (TColor)0x666666;
            scroll_bar->Color = (TColor)0x666666;
            scroll_bar->SystemBkColor = false;
        }
        if (class_name == "TElTreeStringGrid")
        {
            TElTreeStringGrid *string_grid = (TElTreeStringGrid *)component;
            string_grid->BkColor = (TColor)0x434343;
            string_grid->FocusedSelectColor = (TColor)0xB5B077;
            string_grid->HeaderFont->Color = (TColor)0x6A4F9;
            string_grid->Font->Color = (TColor)0xCCCCCC;
            string_grid->TextColor = (TColor)0xAAAAAA;
            string_grid->LinesColor = (TColor)0x272727;
            string_grid->VertDivLinesColor = (TColor)0x272727;
            string_grid->VerticalLines = true;
            string_grid->HorzDivLinesColor = (TColor)0x272727;
            string_grid->HorizontalLines = true;
            string_grid->HeaderColor = (TColor)0x888888;
            string_grid->VertScrollBarStyles->UseXPThemes = false;
            string_grid->VertScrollBarStyles->SystemBkColor = true;
            string_grid->VertScrollBarStyles->BarColor = (TColor)0x666666;
            string_grid->VertScrollBarStyles->Color = (TColor)0x666666;
            string_grid->VertScrollBarStyles->HintColor = (TColor)0x373737;
            string_grid->VertScrollBarStyles->SystemBkColor = false;
            string_grid->HorzScrollBarStyles->UseXPThemes = false;
            string_grid->HorzScrollBarStyles->SystemBkColor = true;
            string_grid->HorzScrollBarStyles->BarColor = (TColor)0xAAAAAA;
            string_grid->HorzScrollBarStyles->Color = (TColor)0x666666;
            string_grid->HorzScrollBarStyles->HintColor = (TColor)0x373737;
            string_grid->HorzScrollBarStyles->SystemBkColor = false;
        }
        if (class_name == "TElTreeInplaceEdit")
        {
            TElTreeInplaceEdit *inplace_edit = (TElTreeInplaceEdit *)component;
            inplace_edit->Editor->UseXPThemes = false;
            inplace_edit->Editor->Flat = true;
            inplace_edit->Editor->ParentColor = true;
            inplace_edit->Editor->ParentFont = true;
            inplace_edit->Editor->BorderStyle = bsNone;
            inplace_edit->Editor->Font->Size = FormNpcEditDrops->dropGrid->Font->Size;
            inplace_edit->Editor->Font->Color = (TColor)0xDDD164;
        }
        if (class_name == "TElTreeInplaceSpinEdit")
        {
            TElTreeInplaceSpinEdit *inplace_spin_edit = (TElTreeInplaceSpinEdit *)component;
            inplace_spin_edit->Editor->UseXPThemes = false;
            inplace_spin_edit->Editor->Flat = true;
            inplace_spin_edit->Editor->BorderStyle = bsNone;
            inplace_spin_edit->Editor->ParentColor = true;
            inplace_spin_edit->Editor->ParentFont = true;
            inplace_spin_edit->Editor->Font->Size = FormNpcEditDrops->dropGrid->Font->Size;
            inplace_spin_edit->Editor->Font->Color = (TColor)0xDDD164;
        }
        if (class_name == "TElTreeInplaceComboBox")
        {
            TElTreeInplaceComboBox *inplace_combo_box = (TElTreeInplaceComboBox *)component;
            inplace_combo_box->Editor->UseXPThemes = false;
            inplace_combo_box->Editor->Flat = true;
            inplace_combo_box->Editor->BorderStyle = bsNone;
            inplace_combo_box->Editor->ListColor = (TColor)0x3D3D3D;
            inplace_combo_box->Editor->ButtonColor = (TColor)0x555555;
            inplace_combo_box->Editor->ButtonWidth = 24;
            inplace_combo_box->Editor->ButtonFlat = true;
            inplace_combo_box->Editor->ListTransparentSelection = true;
            inplace_combo_box->Editor->Color = (TColor)0x5E5E5E;
        }
    }
}

// Keeps the dark corner panels at the lower right of the grids.
void __fastcall TFormMain::FormResize(TObject *Sender)
{
    TPanel *corner_panel;
    if (config->GetTheme() != "dark")
        return;
    TElTreeStringGrid *grid = ElTreeStringGrid1;
    corner_panel = darkPanel1;
    corner_panel->Left = grid->Left + grid->Width - corner_panel->Width;
    corner_panel->Top = grid->Top + grid->Height - corner_panel->Height;
    darkPanel1->Left = corner_panel->Left;
    darkPanel1->Top = corner_panel->Top;
    darkPanel2->Left = corner_panel->Left;
    darkPanel2->Top = corner_panel->Top;
    darkPanel3->Left = corner_panel->Left;
    darkPanel3->Top = corner_panel->Top;
    darkPanel4->Left = corner_panel->Left;
    darkPanel4->Top = corner_panel->Top;
    darkPanel5->Left = corner_panel->Left;
    darkPanel5->Top = corner_panel->Top;
    darkPanel6->Left = corner_panel->Left;
    darkPanel6->Top = corner_panel->Top;
    darkPanel7->Left = corner_panel->Left;
    darkPanel7->Top = corner_panel->Top;
}

// No line hints on the item grid.
void __fastcall TFormMain::ElTreeStringGrid1ShowLineHint(TObject *Sender, TElTreeItem *Item, TElHeaderSection *Section,
    TElFString &Text, THintWindow *HintWindow, TPoint &MousePos, bool &DoShowHint)
{
    DoShowHint = false;
}

// Escape puts back the text of the cell being quick edited.
void __fastcall TFormMain::ElTreeStringGrid1KeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key != VK_ESCAPE)
        return;
    if (!config->GetMouseQuickSave())
        return;
    if (!edit_grid)
        return;
    if (!edit_row)
        return;
    if (edit_grid->Selected == NULL)
        return;
    if (((TElTreeStringGrid *)Sender)->Selected == NULL)
        return;
    TElWideStrings *cells = edit_row->ColumnText;
    cells->Strings[edit_column] = WideString(edit_backup);
    edit_backup = "";
    edit_grid = 0;
    edit_row = 0;
}

// Offers to save on exit when something changed; closes the gfx libraries.
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (config->GetExitSave() && AnyChanged())
    {
        FormWarningExit->ShowExitWarning(this);
        if (FormWarningExit->ModalResult == mrOk)
        {
            SpeedButton4Click((TObject *)SpeedButton4);
            Action = caNone;
            return;
        }
        if (FormWarningExit->ModalResult == mrCancel)
        {
            Action = caNone;
            return;
        }
    }
    gfx_vault->CloseLibraries();
}

// Whether any tab has changes not yet exported.
bool TFormMain::AnyChanged()
{
    bool changed = false;
    if (classes_tab->grid_ui->IsChanged())
        changed = true;
    if (items_tab->grid_ui->IsChanged())
        changed = true;
    if (npcs_tab->grid_ui->IsChanged())
        changed = true;
    if (skills_tab->grid_ui->IsChanged())
        changed = true;
    if (shops_tab->grid_ui->IsChanged())
        changed = true;
    if (trainers_tab->grid_ui->IsChanged())
        changed = true;
    if (inns_tab->grid_ui->IsChanged())
        changed = true;
    return changed;
}

// Marks every tab as saved.
void TFormMain::ClearChanged()
{
    classes_tab->grid_ui->ClearChanged();
    items_tab->grid_ui->ClearChanged();
    npcs_tab->grid_ui->ClearChanged();
    skills_tab->grid_ui->ClearChanged();
    shops_tab->grid_ui->ClearChanged();
    trainers_tab->grid_ui->ClearChanged();
    inns_tab->grid_ui->ClearChanged();
}

// Sets the editor up on the first tick; every tick shows the row count of each tab.
void __fastcall TFormMain::Timer1Timer(TObject *Sender)
{
    timer_ticks++;
    if (!started)
    {
        started = true;
        FormToolsItems->Image1->Picture->Bitmap = Image1->Picture->Bitmap;
        FormConfiguration->Image1->Picture->Bitmap = Image1->Picture->Bitmap;
        FormExport->Image1->Picture->Bitmap = Image1->Picture->Bitmap;
        FormPathConfig->Image1->Picture->Bitmap = Image1->Picture->Bitmap;
        config_check = new ConfigCheck();
        char paths_missing = false;
        int unused_value = 0;
        if (!config_check->CheckGfxPath(config->GetGfxPath()))
            paths_missing = true;
        if (!config_check->CheckPubPath(config->GetPubPath()) && config->GetPubPath() == ".\\pub\\")
            paths_missing = true;
        // Without the gfx files or the default pub folder, the folders are asked for first.
        if (paths_missing)
        {
            FormPathConfig->ShowModal();
            if (FormPathConfig->ModalResult == mrOk)
            {
                config->SetPubPath(FormPathConfig->editPubPath->Caption);
                config->SetGfxPath(FormPathConfig->editGfxPath->Caption);
                config->SetExportPath(FormPathConfig->editExportPath->Caption);
                config->SaveSettings();
            }
            else if (FormPathConfig->ModalResult == mrCancel)
            {
                Application->Terminate();
                return;
            }
        }
        if (mainLabel->Top + mainLabel->Left != 137)
            return;
        if (mainLabel->Caption != "Endless Online")
            return;
        gfx_vault = new GfxVault();
        gfx_vault->OpenLibraries();
        gfx_vault->CountGraphics();
        classes_tab = new UiTabClasses(config->GetPubPath(), 0, 0);
        items_tab = new UiTabItems(config->GetPubPath(), gfx_vault->items_library, gfx_vault->items_library);
        npcs_tab = new UiTabNpcs(config->GetPubPath(), gfx_vault->npcs_library, 0);
        skills_tab = new UiTabSkills(config->GetPubPath(), gfx_vault->skills_library, gfx_vault->icons_library);
        shops_tab = new UiTabShops(config->GetPubPath(), 0, 0);
        trainers_tab = new UiTabTrainers(config->GetPubPath(), 0, 0);
        inns_tab = new UiTabInns(config->GetPubPath(), 0, 0);
        if (config->GetLockedIndexColumn())
            SetHeaderLock(true);
        SetRowSelect(config->GetRowSelect());
        SetQuickEdit(config->GetMouseQuickEdit());
        LoadPubs();
        if (items_tab->main_grid->Items->Count > 0 && ElPageControl1->ActivePageIndex == 0)
        {
            items_tab->main_grid->Items->Item[0]->Selected = true;
            items_tab->main_grid->Items->Item[0]->Focused = true;
            items_tab->main_grid->Selected = items_tab->main_grid->Items->Item[0];
            items_tab->main_grid->SetFocus();
        }
    }
    if (caption_altered)
        items_tab = 0;
    // Every tick the tab captions show the row counts.
    String items_caption = " Items ( " + String(ElTreeStringGrid1->Items->Count) + " )";
    if (ElTabSheet1->Caption != items_caption)
        ElTabSheet1->Caption = WideString(items_caption);
    String npcs_caption = " Npcs ( " + String(ElTreeStringGrid5->Items->Count) + " )";
    if (ElTabSheet3->Caption != npcs_caption)
        ElTabSheet3->Caption = WideString(npcs_caption);
    String skills_caption = " Skills ( " + String(ElTreeStringGrid7->Items->Count) + " )";
    if (ElTabSheet4->Caption != skills_caption)
        ElTabSheet4->Caption = WideString(skills_caption);
    String classes_caption = " Classes ( " + String(ElTreeStringGrid3->Items->Count) + " )";
    if (ElTabSheet2->Caption != classes_caption)
        ElTabSheet2->Caption = WideString(classes_caption);
    String shops_caption = " Shops ( " + String(ElTreeStringGrid9->Items->Count) + " )";
    if (ElTabSheet5->Caption != shops_caption)
        ElTabSheet5->Caption = WideString(shops_caption);
    String trainers_caption = " Trainers ( " + String(ElTreeStringGrid11->Items->Count) + " )";
    if (ElTabSheet6->Caption != trainers_caption)
        ElTabSheet6->Caption = WideString(trainers_caption);
    String inns_caption = " Inns ( " + String(ElTreeStringGrid13->Items->Count) + " )";
    if (ElTabSheet7->Caption != inns_caption)
        ElTabSheet7->Caption = WideString(inns_caption);
}

// Reads every pub file again until all open, then checks them.
void TFormMain::LoadPubs()
{
    bool open_failed;
    do
    {
        open_failed = false;
        classes_tab->Reload();
        items_tab->Reload();
        npcs_tab->Reload();
        skills_tab->Reload();
        shops_tab->Reload();
        trainers_tab->Reload();
        inns_tab->Reload();
        if (items_tab->file_unreadable)
            open_failed = true;
        if (npcs_tab->file_unreadable)
            open_failed = true;
        if (skills_tab->file_unreadable)
            open_failed = true;
        if (classes_tab->file_unreadable)
            open_failed = true;
        if (shops_tab->file_unreadable)
            open_failed = true;
        if (trainers_tab->file_unreadable)
            open_failed = true;
        if (inns_tab->file_unreadable)
            open_failed = true;
        // An unreadable pub file asks to retry or quit; a retry reads every file again.
        if (open_failed)
        {
            FormWarningCannotOpen->ShowCannotOpen(this);
            if (FormWarningCannotOpen->ModalResult == mrAbort)
            {
                Application->Terminate();
                open_failed = false;
            }
        }
    } while (open_failed);
    CheckPubs();
    PubsLoaded();
    classes_tab->class_values->Clear();
    items_tab->item_values->Clear();
    skills_tab->skill_values->Clear();
}

// Checks the read pubs for lost rows, mixed headers, corrupt data and out-of-bound values, and warns.
void TFormMain::CheckPubs()
{
    DataGridCol *key_column;
    int classes_lost;
    no_pubs_found = false;
    if (classes_tab->main_grid->Items->Count <= 0 && items_tab->main_grid->Items->Count <= 0 &&
        npcs_tab->main_grid->Items->Count <= 0 && skills_tab->main_grid->Items->Count <= 0)
    {
        no_pubs_found = true;
        FormWarning->ShowWarning(this, "Pub Files Not Found",
            "No pub files in [ Pub Read Path ]. Click on the configuration button to fix your pub file folder "
                "location.");
        return;
    }
    if (items_tab->grid_ui->data_grid->has_link_out_of_bounds)
    {
        key_column = items_tab->grid_ui->data_grid->GetKeyColumn();
        String link_error = "items." + key_column->header_text + " cannot link to class.Name record(s)";
        FormClassCorruption->classErrTxt->Caption = link_error;
        FormClassCorruption->ShowCorruption(this);
        if (FormClassCorruption->ModalResult == mrOk)
            AddMissingClasses();
    }
    // A file that holds fewer records than its header promises has lost rows.
    data_loss = false;
    FormPubstatus->SetDataLoss(true, "Good Health");
    String report = "";
    if (classes_tab->class_values->records_read < classes_tab->class_values->record_count)
    {
        data_loss = true;
        classes_lost = classes_tab->class_values->record_count - classes_tab->class_values->records_read;
        report +=
            "Classes " + IntToStr(classes_tab->class_values->records_read) + " / " +
                IntToStr(classes_tab->class_values->record_count) + " read :  " + IntToStr(classes_lost) + " lost\n";
    }
    if (items_tab->item_values->records_read < items_tab->item_values->records_expected)
    {
        data_loss = true;
        int items_lost = items_tab->item_values->records_expected - items_tab->item_values->records_read;
        report +=
            "Items " + IntToStr(items_tab->item_values->records_read) + " / " +
                IntToStr(items_tab->item_values->records_expected) + " read :  " + IntToStr(items_lost) + " lost\n";
    }
    if (npcs_tab->npc_values->records_read < npcs_tab->npc_values->records_expected)
    {
        data_loss = true;
        int npcs_lost = npcs_tab->npc_values->records_expected - npcs_tab->npc_values->records_read;
        report +=
            "Npcs " + IntToStr(npcs_tab->npc_values->records_read) + " / " +
                IntToStr(npcs_tab->npc_values->records_expected) + " read :  " + IntToStr(npcs_lost) + " lost\n";
    }
    if (skills_tab->skill_values->records_read < skills_tab->skill_values->records_expected)
    {
        data_loss = true;
        int skills_lost = skills_tab->skill_values->records_expected - skills_tab->skill_values->records_read;
        report +=
            "Skills  " + IntToStr(npcs_tab->npc_values->records_read) + " / " +
                IntToStr(npcs_tab->npc_values->records_expected) + " read :  " + IntToStr(skills_lost) + " lost\n";
    }
    if (data_loss)
    {
        FormWarningOutOfBound->labelCaption->Caption = "Data Loss detected";
        FormWarningOutOfBound->labelInfo->Caption =
            "Please check your pub files. The editor could not read the full ammount of expected file data. This could "
                "be due file corruption or missing truncated file(s) for example: dat002.eif , dat003.eif etc.";
        FormWarningOutOfBound->ShowOutOfBounds(this, true, report);
        FormPubstatus->SetDataLoss(false, FormWarningOutOfBound->labelCaption->Caption);
        FormPubstatus->SetDataLossHint(report);
    }
    // Every part file of one kind must carry the same header as the first.
    header_conflict = false;
    FormPubstatus->SetTruncation(true, "Good Health");
    report = "";
    char mismatch = false;
    unsigned short checksum_high = 0;
    unsigned short checksum_low = 0;
    unsigned char file_version = 0;
    for (int item_header_index = 0; item_header_index < (unsigned char)items_tab->item_values->HeaderCount();
        item_header_index++)
    {
        PubfileHeader *item_header = items_tab->item_values->GetHeader(item_header_index);
        if (!item_header_index)
        {
            checksum_high = item_header->checksum_high;
            checksum_low = item_header->checksum_low;
            file_version = item_header->version;
        }
        else
        {
            if (item_header->checksum_high != checksum_high)
            {
                mismatch = true;
                break;
            }
            if (item_header->checksum_low != checksum_low)
            {
                mismatch = true;
                break;
            }
            if (item_header->version != file_version)
            {
                mismatch = true;
                break;
            }
        }
    }
    if (mismatch)
    {
        header_conflict = true;
        report +=
            "Items - header mismatch " + IntToStr(checksum_high) + " : " + IntToStr(checksum_low) + " -version : " +
                IntToStr(file_version) + "\n";
    }
    mismatch = false;
    checksum_high = 0;
    checksum_low = 0;
    file_version = 0;
    for (int npc_header_index = 0; npc_header_index < (unsigned char)npcs_tab->npc_values->HeaderCount();
        npc_header_index++)
    {
        PubfileHeader *npc_header = npcs_tab->npc_values->GetHeader(npc_header_index);
        if (!npc_header_index)
        {
            checksum_high = npc_header->checksum_high;
            checksum_low = npc_header->checksum_low;
            file_version = npc_header->version;
        }
        else
        {
            if (npc_header->checksum_high != checksum_high)
            {
                mismatch = true;
                break;
            }
            if (npc_header->checksum_low != checksum_low)
            {
                mismatch = true;
                break;
            }
            if (npc_header->version != file_version)
            {
                mismatch = true;
                break;
            }
        }
    }
    if (mismatch)
    {
        header_conflict = true;
        report +=
            "Npcs - header mismatch " + IntToStr(checksum_high) + " : " + IntToStr(checksum_low) + " -version : " +
                IntToStr(file_version) + "\n";
    }
    if (header_conflict)
    {
        FormWarningOutOfBound->labelCaption->Caption = "Truncated Conflict detected";
        FormWarningOutOfBound->labelInfo->Caption =
            "Pub files are mixed with different headers version.s This can result in duplicated and corrupted data, "
                "Please replace the pub files of a same exprot batch, check for example : dat002.eif , dat003.eif etc.";
        FormWarningOutOfBound->ShowOutOfBounds(this, true, report);
        FormPubstatus->SetTruncation(false, FormWarningOutOfBound->labelCaption->Caption);
        FormPubstatus->SetTruncationHint(report);
    }
    report = "";
    if (npcs_tab->npc_values->has_bad_drop || npcs_tab->npc_values->has_bad_message)
    {
        String broken_lists = "";
        if (npcs_tab->npc_values->has_bad_drop)
        {
            broken_lists += "npc.drops, ";
            report += npcs_tab->npc_values->drops_path + "\n";
        }
        if (npcs_tab->npc_values->has_bad_message)
        {
            broken_lists += "npc.chats, ";
            report += npcs_tab->npc_values->chats_path + "\n";
        }
        broken_lists.Delete(broken_lists.Length() - 1, 1);
        FormWarningOutOfBound->labelCaption->Caption = "Corrupt Npc Server Files";
        FormWarningOutOfBound->labelInfo->Caption =
            "One ore more entries of " + broken_lists +
                "did not load! The (npc) data file(s) refer to NPC.IDs that are out of bound. Data files are probably "
                    "out-of-sync, try fix the problem using a backup.";
        FormWarningOutOfBound->ShowOutOfBounds(this, false, report);
        FormPubstatus->SetCorruption(false, FormWarningOutOfBound->labelCaption->Caption);
        FormPubstatus->SetCorruptionHint(report);
    }
    // Rows the reader could not make sense of mark the data as corrupt.
    data_corrupt = false;
    FormPubstatus->SetCorruption(true, "Good Health");
    report = "";
    if (classes_tab->class_values->has_long_name)
    {
        data_corrupt = true;
        report += "Classes - data corruption , rows : " + IntToStr(classes_tab->class_values->records_read);
    }
    if (items_tab->item_values->has_bad_record)
    {
        data_corrupt = true;
        report += "Items - data corruption , rows : " + IntToStr(classes_tab->class_values->records_read);
    }
    if (npcs_tab->npc_values->has_bad_record)
    {
        data_corrupt = true;
        report += "Npcs - data corruption , rows : " + IntToStr(classes_tab->class_values->records_read);
    }
    if (skills_tab->skill_values->has_bad_record)
    {
        data_corrupt = true;
        report += "Skills - data corruption , rows : " + IntToStr(classes_tab->class_values->records_read);
    }
    if (data_corrupt)
    {
        FormWarningOutOfBound->labelCaption->Caption = "Corrupt Data detected";
        FormWarningOutOfBound->labelInfo->Caption =
            "Your data contain corrupt entries. This is actually very bad - use ctrl+I to locate the file(s) and "
                "restore them with backup file(s)";
        FormWarningOutOfBound->ShowOutOfBounds(this, false, report);
        FormPubstatus->SetCorruption(false, FormWarningOutOfBound->labelCaption->Caption);
        FormPubstatus->SetCorruptionHint(report);
    }
    // Values outside the known lists are kept, shown as out of bounds.
    out_of_bounds = false;
    FormPubstatus->SetOutOfBounds(true, "Good Health");
    report = "";
    if (classes_tab->grid_ui->data_grid->has_out_of_bounds)
    {
        out_of_bounds = true;
        report += classes_tab->grid_ui->data_grid->VisibleColumnNames();
    }
    if (items_tab->grid_ui->data_grid->has_out_of_bounds)
    {
        out_of_bounds = true;
        report += items_tab->grid_ui->data_grid->VisibleColumnNames();
    }
    if (npcs_tab->grid_ui->data_grid->has_out_of_bounds)
    {
        out_of_bounds = true;
        report += npcs_tab->grid_ui->data_grid->VisibleColumnNames();
    }
    if (skills_tab->grid_ui->data_grid->has_out_of_bounds)
    {
        out_of_bounds = true;
        report += skills_tab->grid_ui->data_grid->VisibleColumnNames();
    }
    if (out_of_bounds)
    {
        FormWarningOutOfBound->labelCaption->Caption = "Out-of-bound data detected";
        FormWarningOutOfBound->labelInfo->Caption =
            "Your data contains index number(s) that are not in the official list.  Do not worry as these values are "
                "extra and are added in as out of bounds(OOB). Use the toolbox ( ctrl + T ) to clean the column(s)";
        if (config->GetOobWarnings())
            FormWarningOutOfBound->ShowOutOfBounds(this, false, report);
        FormPubstatus->SetOutOfBounds(false, FormWarningOutOfBound->labelCaption->Caption);
        FormPubstatus->SetOutOfBoundsHint(report);
    }
    if (SpeedButton5->Left > 700)
        gfx_vault = 0;
    if (SpeedButton5->Left < 600)
        gfx_vault = 0;
    if (SpeedButton5->Top != 10)
        gfx_vault = 0;
    if (mainLabel->Caption[1] != 'E')
        gfx_vault = 0;
}

void TFormMain::PubsLoaded()
{
}

// Adds placeholder classes up to the highest class the items refer to, then reloads the items.
void TFormMain::AddMissingClasses()
{
    TElWideStrings *parent_cells;
    TElWideStrings *stat_group_cells;
    TElWideStrings *stat_cells;
    DataGridCol *class_column = items_tab->grid_ui->data_grid->GetKeyColumn();
    for (int class_count = classes_tab->main_grid->Items->Count; class_column->largest_link_value > class_count;
        class_count++)
    {
        TElTreeItem *row = classes_tab->main_grid->Items->AddItem(0);
        row->Text = WideString(IntToStr(class_count + 1));
        TElWideStrings *name_cells = row->ColumnText;
        name_cells->Add(WideString("AutoC-" + IntToStr(class_count + 1)));
        parent_cells = row->ColumnText;
        parent_cells->Add(WideString("-"));
        stat_group_cells = row->ColumnText;
        stat_group_cells->Add(WideString("-"));
        for (int column_index = 4; column_index < classes_tab->main_grid->HeaderSections->Count; column_index++)
        {
            stat_cells = row->ColumnText;
            stat_cells->Add(WideString(""));
        }
    }
    items_tab->Reload();
}

int TFormMain::NumberFromDigits(String text)
{
    String digits = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] >= '0' && text[i] <= '9' && digits.Length() < 8)
            digits += text[i];
    }
    if (!digits.Length())
        return -1;
    return StrToInt(digits);
}

int TFormMain::OutOfBoundsValue(String text)
{
    if (text.Length() <= 6)
        return -1;
    if (text[1] != '[')
        return -1;
    if (text.Length() >= 15)
        return -1;
    if (text.Pos("]") <= 0)
        return -1;
    if (text.Pos("[OOB-") <= 0)
    {
        if (text.Pos("[oob-") <= 0)
            return -1;
    }
    int i = 6;
    int length = text.Length();
    String digits = "";
    for (; i < length; i++)
    {
        if (text[i] >= '0' && text[i] <= '9')
            digits += text[i];
    }
    if (!digits.Length())
        return -1;
    return StrToInt(digits);
}

// Switches the save, reload and tab hotkeys on or off.
void TFormMain::EnableHotkeys(bool on)
{
    hotkeySave->Enabled = on;
    hotkeyRefresh->Enabled = on;
    hotkeyTab1->Enabled = on;
    hotkeyTab2->Enabled = on;
    hotkeyTab3->Enabled = on;
    hotkeyTab4->Enabled = on;
    hotkeyTab5->Enabled = on;
    hotkeyTab6->Enabled = on;
    hotkeyTab7->Enabled = on;
}

// Shows every column of every tab, or only the usual ones.
void TFormMain::ShowAllColumns(char all)
{
    items_tab->grid_ui->ShowColumns(all, 0);
    skills_tab->grid_ui->ShowColumns(all, 0);
    npcs_tab->grid_ui->ShowColumns(all, 0);
    classes_tab->grid_ui->ShowColumns(all, 0);
    shops_tab->grid_ui->ShowColumns(all, 0);
    trainers_tab->grid_ui->ShowColumns(all, 0);
    inns_tab->grid_ui->ShowColumns(all, 0);
}

void TFormMain::SetItemHints(char on)
{
    items_tab->grid_ui->SetItemHints(on, 0);
    skills_tab->grid_ui->SetItemHints(on, 0);
    npcs_tab->grid_ui->SetItemHints(on, 0);
    classes_tab->grid_ui->SetItemHints(on, 0);
    shops_tab->grid_ui->SetItemHints(on, 0);
    trainers_tab->grid_ui->SetItemHints(on, 0);
    inns_tab->grid_ui->SetItemHints(on, 0);
}

void TFormMain::FillSubGrids(char clear)
{
    items_tab->grid_ui->FillSubGrid(clear);
    skills_tab->grid_ui->FillSubGrid(clear);
    npcs_tab->grid_ui->FillSubGrid(clear);
    classes_tab->grid_ui->FillSubGrid(clear);
    shops_tab->grid_ui->FillSubGrid(clear);
    trainers_tab->grid_ui->FillSubGrid(clear);
    inns_tab->grid_ui->FillSubGrid(clear);
}

void TFormMain::BeginUpdate()
{
    items_tab->grid_ui->BeginUpdate();
    skills_tab->grid_ui->BeginUpdate();
    npcs_tab->grid_ui->BeginUpdate();
    classes_tab->grid_ui->BeginUpdate();
    shops_tab->grid_ui->BeginUpdate();
    trainers_tab->grid_ui->BeginUpdate();
    inns_tab->grid_ui->BeginUpdate();
}

void TFormMain::EndUpdate()
{
    items_tab->grid_ui->EndUpdate();
    skills_tab->grid_ui->EndUpdate();
    npcs_tab->grid_ui->EndUpdate();
    classes_tab->grid_ui->EndUpdate();
    shops_tab->grid_ui->EndUpdate();
    trainers_tab->grid_ui->EndUpdate();
    inns_tab->grid_ui->EndUpdate();
}

void TFormMain::SetQuickEdit(bool on)
{
    items_tab->main_grid->QuickEditMode = on;
    skills_tab->main_grid->QuickEditMode = on;
    npcs_tab->main_grid->QuickEditMode = on;
    classes_tab->main_grid->QuickEditMode = on;
    shops_tab->main_grid->QuickEditMode = on;
    trainers_tab->main_grid->QuickEditMode = on;
    inns_tab->main_grid->QuickEditMode = on;
    items_tab->sub_grid->QuickEditMode = on;
    skills_tab->sub_grid->QuickEditMode = on;
    npcs_tab->sub_grid->QuickEditMode = on;
    classes_tab->sub_grid->QuickEditMode = on;
    shops_tab->sub_grid->QuickEditMode = on;
    trainers_tab->sub_grid->QuickEditMode = on;
    inns_tab->sub_grid->QuickEditMode = on;
    FormNpcEditChats->lineGrid->QuickEditMode = on;
    FormNpcEditDrops->dropGrid->QuickEditMode = on;
    FormShopEditItems->dropGrid->QuickEditMode = on;
    FormShopEditCrafts->dropGrid->QuickEditMode = on;
    FormTrainerEditSkills->dropGrid->QuickEditMode = on;
}

void TFormMain::SetRowSelect(bool on)
{
    items_tab->main_grid->goRowSelect = on;
    skills_tab->main_grid->goRowSelect = on;
    npcs_tab->main_grid->goRowSelect = on;
    classes_tab->main_grid->goRowSelect = on;
    shops_tab->main_grid->goRowSelect = on;
    trainers_tab->main_grid->goRowSelect = on;
    inns_tab->main_grid->goRowSelect = on;
}

void TFormMain::SetHeaderLock(char locked)
{
    items_tab->grid_ui->SetHeaderLock(1, locked);
    skills_tab->grid_ui->SetHeaderLock(1, locked);
    npcs_tab->grid_ui->SetHeaderLock(1, locked);
    classes_tab->grid_ui->SetHeaderLock(1, locked);
    shops_tab->grid_ui->SetHeaderLock(1, locked);
    trainers_tab->grid_ui->SetHeaderLock(1, locked);
    inns_tab->grid_ui->SetHeaderLock(1, locked);
}

void __fastcall TFormMain::hotkeySavePress(TObject *Sender)
{
    if (Enabled && Active)
        SpeedButton4Click((TObject *)SpeedButton4);
}

void __fastcall TFormMain::hotkeyTab1Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 0;
}

void __fastcall TFormMain::hotkeyTab2Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 1;
}

void __fastcall TFormMain::hotkeyTab3Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 2;
}

void __fastcall TFormMain::hotkeyTab4Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 3;
}

void __fastcall TFormMain::hotkeyTab5Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 4;
}

void __fastcall TFormMain::hotkeyTab6Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 5;
}

void __fastcall TFormMain::hotkeyTab7Press(TObject *Sender)
{
    if (Enabled && Active)
        ElPageControl1->ActivePageIndex = 6;
}

// Shows the pub file health of the open tab, unless it is already shown.
void TFormMain::HotkeyStatus()
{
    if (!FormPubstatus->Visible && Enabled && Active && ElPageControl1->ActivePageIndex < 7)
        ShowPubStatus(ElPageControl1->ActivePageIndex);
}

// The reload hotkey asks whether to read the pubs again now.
void __fastcall TFormMain::hotkeyRefreshPress(TObject *Sender)
{
    if (Enabled && Active)
    {
        if (!FormReloadWarning->Visible)
        {
            FormReloadWarning->ShowReloadWarning(this, 1);
            if (FormReloadWarning->ModalResult == mrOk)
                LoadPubs();
        }
    }
}

// Opens the editor on a copy of the selected row of the open tab.
void TFormMain::HotkeyDupe()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            popupItemDupeClick((TObject *)popupItemEdit);
        if (ElPageControl1->ActivePageIndex == 1)
            popupNpcDupeClick((TObject *)popupNpcEdit);
        if (ElPageControl1->ActivePageIndex == 2)
            popupSkillDupeClick((TObject *)popupSkillEdit);
        if (ElPageControl1->ActivePageIndex == 3)
            popupClassDupeClick((TObject *)popupClassEdit);
        if (ElPageControl1->ActivePageIndex == 4)
            popupShopDupeClick((TObject *)popupShopEdit);
        if (ElPageControl1->ActivePageIndex == 5)
            popupTrainerDupeClick((TObject *)popupTrainerEdit);
        if (ElPageControl1->ActivePageIndex == 6)
            popupInnDupeClick((TObject *)popupTrainerEdit);
    }
}

// Opens the editor on the selected row of the open tab.
void TFormMain::HotkeyEdit()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            popupItemEditClick((TObject *)popupItemEdit);
        if (ElPageControl1->ActivePageIndex == 1)
            popupNpcEditClick((TObject *)popupNpcEdit);
        if (ElPageControl1->ActivePageIndex == 2)
            popupSkillEditClick((TObject *)popupSkillEdit);
        if (ElPageControl1->ActivePageIndex == 3)
            popupClassEditClick((TObject *)popupClassEdit);
        if (ElPageControl1->ActivePageIndex == 4)
            popupShopEditClick((TObject *)popupShopEdit);
        if (ElPageControl1->ActivePageIndex == 5)
            popupTrainerEditClick((TObject *)popupTrainerEdit);
        if (ElPageControl1->ActivePageIndex == 6)
            popupInnEditClick((TObject *)popupTrainerEdit);
    }
}

void TFormMain::HotkeyImage()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            imageItemClick((TObject *)imageItem);
        if (ElPageControl1->ActivePageIndex == 1)
            imageNpcClick((TObject *)imageNpc);
        if (ElPageControl1->ActivePageIndex == 2)
            imageIconSkillClick((TObject *)imageIconSkill);
    }
}

// Opens the editor for a new row in the open tab.
void TFormMain::HotkeyNew()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            popupItemNewClick((TObject *)popupItemNew);
        if (ElPageControl1->ActivePageIndex == 1)
            popupNpcNewClick((TObject *)popupNpcNew);
        if (ElPageControl1->ActivePageIndex == 2)
            popupSkillNewClick((TObject *)popupSkillNew);
        if (ElPageControl1->ActivePageIndex == 3)
            popupClassNewClick((TObject *)popupClassNew);
        if (ElPageControl1->ActivePageIndex == 4)
            popupShopNewClick((TObject *)popupShopNew);
        if (ElPageControl1->ActivePageIndex == 5)
            popupTrainerNewClick((TObject *)popupTrainerNew);
        if (ElPageControl1->ActivePageIndex == 6)
            popupInnNewClick((TObject *)popupTrainerNew);
    }
}

// Copies the selected row of the open tab.
void TFormMain::HotkeyCopy()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            items_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 1)
            npcs_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 2)
            skills_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 3)
            classes_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 4)
            shops_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 5)
            trainers_tab->grid_ui->CopyRow();
        if (ElPageControl1->ActivePageIndex == 6)
            inns_tab->grid_ui->CopyRow();
    }
}

// Pastes the copied row into the open tab.
void TFormMain::HotkeyPaste()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            items_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 1)
            npcs_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 2)
            skills_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 3)
            classes_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 4)
            shops_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 5)
            trainers_tab->grid_ui->PasteRow();
        if (ElPageControl1->ActivePageIndex == 6)
            inns_tab->grid_ui->PasteRow();
    }
}

void TFormMain::HotkeyTools()
{
    if (Enabled && Active)
    {
        if (ElPageControl1->ActivePageIndex == 0)
            buttonToolsItemClick((TObject *)buttonToolsItem);
        if (ElPageControl1->ActivePageIndex == 1)
            buttonToolsNpcClick((TObject *)buttonToolsNpc);
        if (ElPageControl1->ActivePageIndex == 2)
            buttonToolsSkillClick((TObject *)buttonToolsSkill);
        if (ElPageControl1->ActivePageIndex == 3)
            buttonToolsClassClick((TObject *)buttonToolsClass);
    }
}

// Forgets the cell being quick edited.
void __fastcall TFormMain::ElTreeStringGrid2ItemFocused(TObject *Sender)
{
    FormMain->edit_grid = 0;
    FormMain->edit_row = 0;
}

// Insert does nothing on the item sub grid; the Ctrl hotkeys work there.
void __fastcall TFormMain::ElTreeStringGrid2KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
        Key = 0;
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

// Opens the configuration and applies what changed, telling which changes wait for a restart.
void __fastcall TFormMain::SpeedButton3Click(TObject *Sender)
{
    bool old_view_all;
    bool old_numbered_lists;
    bool old_expert_mode;
    bool old_show_zeros;
    bool old_show_recycled;
    if (FormConfiguration->Visible)
    {
        ((TCustomForm *)FormConfiguration)->Close();
        return;
    }
    FormConfiguration->Left = (Width - FormConfiguration->Width) / 2 + Left;
    FormConfiguration->Top = (Height - FormConfiguration->Height) / 2 + Top + 20;
    if (FormConfiguration->ShowModal() == mrOk)
    {
        // The old values tell which changes need a restart or a refresh.
        old_view_all = config->GetViewAll();
        old_numbered_lists = config->GetNumberedLists();
        old_expert_mode = config->GetExpertMode();
        old_show_zeros = config->GetShowZeros();
        old_show_recycled = config->GetShowRecycled();
        bool old_name_hints = config->GetNameHints();
        String old_theme = config->GetLoadedTheme();
        config->SetPubPath(FormConfiguration->editPubPath->Caption);
        config->SetGfxPath(FormConfiguration->editGfxPath->Caption);
        config->SetExportPath(FormConfiguration->editExportPath->Caption);
        config->SetBackupPath(FormConfiguration->editBackupPath->Caption);
        config->SetBackup(FormConfiguration->checkBackup->Checked);
        config->SetBackupFolders(FormConfiguration->checkBackupFolders->Checked);
        config->SetViewAll(FormConfiguration->checkViewAll->Checked);
        config->SetShowRecycled(FormConfiguration->checkShowRecycled->Checked);
        config->SetCreateRecycled(FormConfiguration->checkCreateRecycled->Checked);
        config->SetNumberedLists(FormConfiguration->checkListNumbers->Checked);
        config->SetDeleteWarning(FormConfiguration->checkDeleteWarning->Checked);
        config->SetPasteWarning(FormConfiguration->checkPasteWarning->Checked);
        config->SetNameHints(FormConfiguration->checkNameHints->Checked);
        config->SetRowSelect(FormConfiguration->checkRowSelect->Checked);
        config->SetSmartFilter(FormConfiguration->checkSmartFilter->Checked);
        config->SetSmartEditors(FormConfiguration->checkSmartEditors->Checked);
        config->SetShowZeros(FormConfiguration->checkShowZeros->Checked);
        config->SetShowBasics(FormConfiguration->checkShowBasics->Checked);
        config->SetMouseQuickEdit(FormConfiguration->checkMouseQuickedit->Checked);
        config->SetMouseQuickSave(FormConfiguration->checkMouseQuicksave->Checked);
        config->SetAutoCapitalization(FormConfiguration->checkAutoCapitalization->Checked);
        config->SetExitSave(FormConfiguration->checkExitSave->Checked);
        config->SetFullscreen(FormConfiguration->checkFullscreen->Checked);
        config->SetHotkeys(FormConfiguration->checkHotkeys->Checked);
        config->SetExpertMode(FormConfiguration->checkExpertMode->Checked);
        config->SetExportEmpty(FormConfiguration->checkExportEmpty->Checked);
        if (FormConfiguration->checkDarkmode->Checked)
            config->SetLoadedTheme("dark");
        else
            config->SetLoadedTheme("");
        config->SaveSettings();
        EnableHotkeys(config->GetHotkeys());
        SetRowSelect(config->GetRowSelect());
        SetQuickEdit(config->GetMouseQuickEdit());
        BeginUpdate();
        SetHeaderLock(config->GetLockedIndexColumn());
        if (config->GetShowRecycled() != old_show_recycled)
            ShowAllColumns(config->GetShowRecycled());
        if (config->GetShowZeros() != old_show_zeros)
            FillSubGrids(config->GetShowZeros());
        if (config->GetNameHints() != old_name_hints)
            SetItemHints(config->GetNameHints());
        EndUpdate();
        String restart_note = "on the next restart of the program.";
        if (old_theme != config->GetLoadedTheme())
        {
            String theme_name = "Default Theme";
            if (config->GetLoadedTheme() == "dark")
                theme_name = "Dark Mode";
            FormWarning->ShowWarning(this, theme_name,
                "You switched to " + LowerCase(theme_name) + ". Theme will be applied " + restart_note);
        }
        if (!old_view_all && config->GetViewAll())
            FormWarning->ShowWarning(this, "View All", "You enabled all columns to be visible " + restart_note);
        if (!old_numbered_lists && config->GetNumberedLists())
            FormWarning->ShowWarning(this, "Numbered Lists",
                "You enabled all lists to have a prefix number " + restart_note);
        if (!old_expert_mode && config->GetExpertMode())
            FormWarning->ShowWarning(this, "Expert Mode Active",
                "Make sure you know what you are doing. Certain actions can damage a live server.");
        // A new pub folder only counts once the pubs are read again.
        if (FormConfiguration->shown_pub_path != config->GetPubPath())
        {
            FormReloadWarning->ShowReloadWarning(this, 0);
            if (FormReloadWarning->ModalResult != mrOk)
                return;
            LoadPubs();
        }
    }
}

// Opens the export over the main window, or closes it when it is open.
void __fastcall TFormMain::SpeedButton4Click(TObject *Sender)
{
    if (FormExport->Visible)
    {
        ((TCustomForm *)FormExport)->Close();
        return;
    }
    FormExport->Left = (Width - FormExport->Width) / 2 + Left;
    FormExport->Top = (Height - FormExport->Height) / 2 + Top + 20;
    if (FormExport->ShowModal() == mrOk)
    {
    }
}

void __fastcall TFormMain::SpeedButton5Click(TObject *Sender)
{
    if (FormAbout->Visible)
    {
        ((TCustomForm *)FormAbout)->Close();
        return;
    }
    FormAbout->Left = (Width - FormAbout->Width) / 2 + Left;
    FormAbout->Top = (Height - FormAbout->Height) / 2 + Top;
    FormAbout->ShowModal();
}

void __fastcall TFormMain::buttonToolsItemClick(TObject *Sender)
{
    FormToolsItems->Open(this);
}

void __fastcall TFormMain::buttonToolsNpcClick(TObject *Sender)
{
    FormToolsNpcs->ShowTools(this);
}

void __fastcall TFormMain::buttonToolsSkillClick(TObject *Sender)
{
    FormToolsSkills->ShowTools(this);
}

void __fastcall TFormMain::buttonToolsClassClick(TObject *Sender)
{
    FormToolsClasses->ShowTools(this);
}

void __fastcall TFormMain::buttonToolsShopClick(TObject *Sender)
{
    FormToolsShops->ShowTools(this);
}

void __fastcall TFormMain::buttonToolsTrainerClick(TObject *Sender)
{
    FormToolsTrainers->ShowTools(this);
}

void __fastcall TFormMain::buttonToolsInnClick(TObject *Sender)
{
    FormToolsInns->ShowTools(this);
}

// Names the open tab and gives its grid the focus.
void __fastcall TFormMain::ElPageControl1Change(TObject *Sender)
{
    if (!Visible || !started)
        return;
    if (ElPageControl1->ActivePageIndex == 0)
    {
        tabNameTxt->Caption = "Items";
        items_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 1)
    {
        tabNameTxt->Caption = "Npcs";
        npcs_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 2)
    {
        tabNameTxt->Caption = "Skills";
        skills_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 3)
    {
        tabNameTxt->Caption = "Classes";
        classes_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 4)
    {
        tabNameTxt->Caption = "Shops";
        shops_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 5)
    {
        tabNameTxt->Caption = "Trainers";
        trainers_tab->main_grid->SetFocus();
    }
    if (ElPageControl1->ActivePageIndex == 6)
    {
        tabNameTxt->Caption = "Inns";
        inns_tab->main_grid->SetFocus();
    }
}

// The number with leading zeros to five digits.
String TFormMain::PaddedNumber(int value)
{
    if (value < 10)
        return "0000" + IntToStr(value);
    if (value < 100)
        return "000" + IntToStr(value);
    if (value < 1000)
        return "00" + IntToStr(value);
    if (value < 10000)
        return "0" + IntToStr(value);
    return IntToStr(value);
}

// The text as a number, an out-of-bound value read as its number; the default for empty text.
int TFormMain::NumberOr(String text, int default_value)
{
    if (!text.Length())
        return default_value;
    int oob_value = OutOfBoundsValue(text);
    if (oob_value > 0)
        return oob_value;
    return StrToInt(text);
}

String TFormMain::TrimLeadingZeros(String text)
{
    int i;
    if (text.Length() <= 0)
        return "0";
    char leading = true;
    String digits = "";
    i = 1;
    for (; i <= text.Length(); i++)
    {
        if (leading)
        {
            if (text[i] == '0')
                continue;
            leading = false;
        }
        if (text[i] >= '0' && text[i] <= '9')
            digits = digits + text[i];
    }
    if (digits.Length() < 1)
        return "0";
    return digits;
}

// Shows the health of the open tab's pub files, one line for each file read.
void TFormMain::ShowPubStatus(unsigned char tab)
{
    int item_header_index;
    FormPubstatus->Caption = " Pubfile Status > ";
    FormPubstatus->listPubfiles->Items->Clear();
    FormPubstatus->ResetHealth();
    // Each part file gets a line with its checksum, version, state and counts.
    String incomplete_text = "incomplete";
    String corrupt_text = "corrupt";
    String eof_break_text = "eof-break";
    if (!tab)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!items_tab->item_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String item_totals =
            IntToStr(items_tab->item_values->records_read) + " / " + IntToStr(items_tab->item_values->records_expected);
        FormPubstatus->labelPubTotals->Caption = item_totals;
        for (item_header_index = 0; item_header_index < (unsigned char)items_tab->item_values->HeaderCount();
            item_header_index++)
        {
            PubfileHeader *item_header = items_tab->item_values->GetHeader(item_header_index);
            String item_state = "ok";
            if (item_header->load_state == 2)
            {
                item_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (item_header->load_state == 3)
            {
                item_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (item_header->load_state == 1)
                item_state = eof_break_text;
            TListItem *item_entry = FormPubstatus->listPubfiles->Items->Add();
            item_entry->Caption = item_header->file_name;
            item_entry->SubItems->Add(PaddedNumber(item_header->checksum_high) + "-" +
                PaddedNumber(item_header->checksum_low));
            item_entry->SubItems->Add("v" + IntToStr(item_header->version));
            item_entry->SubItems->Add(item_state);
            item_entry->SubItems->Add(IntToStr(item_header->file_length));
            item_entry->SubItems->Add(IntToStr(item_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = items_tab->has_eof_record;
        FormPubstatus->Open(this, "Items");
    }
    if (tab == 1)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!npcs_tab->npc_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String npc_totals =
            IntToStr(npcs_tab->npc_values->records_read) + " / " + IntToStr(npcs_tab->npc_values->records_expected);
        FormPubstatus->labelPubTotals->Caption = npc_totals;
        for (int npc_header_index = 0; npc_header_index < (unsigned char)npcs_tab->npc_values->HeaderCount();
            npc_header_index++)
        {
            PubfileHeader *npc_header = npcs_tab->npc_values->GetHeader(npc_header_index);
            String npc_state = "ok";
            if (npc_header->load_state == 2)
            {
                npc_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (npc_header->load_state == 3)
            {
                npc_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (npc_header->load_state == 1)
                npc_state = eof_break_text;
            TListItem *npc_entry = FormPubstatus->listPubfiles->Items->Add();
            npc_entry->Caption = npc_header->file_name;
            npc_entry->SubItems->Add(PaddedNumber(npc_header->checksum_high) + "-" +
                PaddedNumber(npc_header->checksum_low));
            npc_entry->SubItems->Add("v" + IntToStr(npc_header->version));
            npc_entry->SubItems->Add(npc_state);
            npc_entry->SubItems->Add(IntToStr(npc_header->file_length));
            npc_entry->SubItems->Add(IntToStr(npc_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = npcs_tab->has_eof_record;
        FormPubstatus->Open(this, "Npcs");
    }
    if (tab == 2)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!skills_tab->skill_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String skill_totals =
            IntToStr(skills_tab->skill_values->records_read) + " / " +
                IntToStr(skills_tab->skill_values->records_expected);
        FormPubstatus->labelPubTotals->Caption = skill_totals;
        for (int skill_header_index = 0; skill_header_index < (unsigned char)skills_tab->skill_values->HeaderCount();
            skill_header_index++)
        {
            PubfileHeader *skill_header = skills_tab->skill_values->GetHeader(skill_header_index);
            String skill_state = "ok";
            if (skill_header->load_state == 2)
            {
                skill_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (skill_header->load_state == 3)
            {
                skill_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (skill_header->load_state == 1)
                skill_state = eof_break_text;
            TListItem *skill_entry = FormPubstatus->listPubfiles->Items->Add();
            skill_entry->Caption = skill_header->file_name;
            skill_entry->SubItems->Add(PaddedNumber(skill_header->checksum_high) + "-" +
                PaddedNumber(skill_header->checksum_low));
            skill_entry->SubItems->Add("v" + IntToStr(skill_header->version));
            skill_entry->SubItems->Add(skill_state);
            skill_entry->SubItems->Add(IntToStr(skill_header->file_length));
            skill_entry->SubItems->Add(IntToStr(skill_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = skills_tab->has_eof_record;
        FormPubstatus->Open(this, "Skills");
    }
    if (tab == 3)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!classes_tab->class_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String class_totals =
            IntToStr(classes_tab->class_values->records_read) + " / " +
                IntToStr(classes_tab->class_values->record_count);
        FormPubstatus->labelPubTotals->Caption = class_totals;
        for (int class_header_index = 0; class_header_index < (unsigned char)classes_tab->class_values->HeaderCount();
            class_header_index++)
        {
            PubfileHeader *class_header = classes_tab->class_values->GetHeader(class_header_index);
            String class_state = "ok";
            if (class_header->load_state == 2)
            {
                class_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (class_header->load_state == 3)
            {
                class_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (class_header->load_state == 1)
                class_state = eof_break_text;
            TListItem *class_entry = FormPubstatus->listPubfiles->Items->Add();
            class_entry->Caption = class_header->file_name;
            class_entry->SubItems->Add(PaddedNumber(class_header->checksum_high) + "-" +
                PaddedNumber(class_header->checksum_low));
            class_entry->SubItems->Add("v" + IntToStr(class_header->version));
            class_entry->SubItems->Add(class_state);
            class_entry->SubItems->Add(IntToStr(class_header->file_length));
            class_entry->SubItems->Add(IntToStr(class_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = classes_tab->has_eof_record;
        FormPubstatus->Open(this, "Classes");
    }
    if (tab == 4)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!shops_tab->shop_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String shop_totals =
            IntToStr(shops_tab->shop_values->records_read) + " / " + IntToStr(shops_tab->shop_values->record_count);
        FormPubstatus->labelPubTotals->Caption = shop_totals;
        for (int shop_header_index = 0; shop_header_index < (unsigned char)shops_tab->shop_values->HeaderCount();
            shop_header_index++)
        {
            PubfileHeader *shop_header = shops_tab->shop_values->GetHeader(shop_header_index);
            String shop_state = "ok";
            if (shop_header->load_state == 2)
            {
                shop_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (shop_header->load_state == 3)
            {
                shop_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (shop_header->load_state == 1)
                shop_state = eof_break_text;
            TListItem *shop_entry = FormPubstatus->listPubfiles->Items->Add();
            shop_entry->Caption = shop_header->file_name;
            shop_entry->SubItems->Add(PaddedNumber(shop_header->checksum_high) + "-" +
                PaddedNumber(shop_header->checksum_low));
            shop_entry->SubItems->Add("v" + IntToStr(shop_header->version));
            shop_entry->SubItems->Add(shop_state);
            shop_entry->SubItems->Add(IntToStr(shop_header->file_length));
            shop_entry->SubItems->Add(IntToStr(shop_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = classes_tab->has_eof_record;
        FormPubstatus->Open(this, "Shops");
    }
    if (tab == 5)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!trainers_tab->trainer_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String trainer_totals =
            IntToStr(trainers_tab->trainer_values->records_read) + " / " +
                IntToStr(trainers_tab->trainer_values->record_count);
        FormPubstatus->labelPubTotals->Caption = trainer_totals;
        for (int trainer_header_index = 0;
            trainer_header_index < (unsigned char)trainers_tab->trainer_values->HeaderCount(); trainer_header_index++)
        {
            PubfileHeader *trainer_header = trainers_tab->trainer_values->GetHeader(trainer_header_index);
            String trainer_state = "ok";
            if (trainer_header->load_state == 2)
            {
                trainer_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (trainer_header->load_state == 3)
            {
                trainer_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (trainer_header->load_state == 1)
                trainer_state = eof_break_text;
            TListItem *trainer_entry = FormPubstatus->listPubfiles->Items->Add();
            trainer_entry->Caption = trainer_header->file_name;
            trainer_entry->SubItems->Add(PaddedNumber(trainer_header->checksum_high) + "-" +
                PaddedNumber(trainer_header->checksum_low));
            trainer_entry->SubItems->Add("v" + IntToStr(trainer_header->version));
            trainer_entry->SubItems->Add(trainer_state);
            trainer_entry->SubItems->Add(IntToStr(trainer_header->file_length));
            trainer_entry->SubItems->Add(IntToStr(trainer_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = classes_tab->has_eof_record;
        FormPubstatus->Open(this, "Trainers");
    }
    if (tab == 6)
    {
        FormPubstatus->Caption += FormPubstatus->labelPubTitle->Caption;
        if (!inns_tab->inn_values->is_loaded)
            FormPubstatus->SetHealth(2);
        String inn_totals =
            IntToStr(inns_tab->inn_values->records_read) + " / " + IntToStr(inns_tab->inn_values->record_count);
        FormPubstatus->labelPubTotals->Caption = inn_totals;
        for (int inn_header_index = 0; inn_header_index < (unsigned char)inns_tab->inn_values->HeaderCount();
            inn_header_index++)
        {
            PubfileHeader *inn_header = inns_tab->inn_values->GetHeader(inn_header_index);
            String inn_state = "ok";
            if (inn_header->load_state == 2)
            {
                inn_state = incomplete_text;
                FormPubstatus->SetHealth(1);
            }
            if (inn_header->load_state == 3)
            {
                inn_state = corrupt_text;
                FormPubstatus->SetHealth(4);
            }
            if (inn_header->load_state == 1)
                inn_state = eof_break_text;
            TListItem *inn_entry = FormPubstatus->listPubfiles->Items->Add();
            inn_entry->Caption = inn_header->file_name;
            inn_entry->SubItems->Add(PaddedNumber(inn_header->checksum_high) + "-" +
                PaddedNumber(inn_header->checksum_low));
            inn_entry->SubItems->Add("v" + IntToStr(inn_header->version));
            inn_entry->SubItems->Add(inn_state);
            inn_entry->SubItems->Add(IntToStr(inn_header->file_length));
            inn_entry->SubItems->Add(IntToStr(inn_header->records_read));
        }
        FormPubstatus->labelWeirdEof->Visible = classes_tab->has_eof_record;
        FormPubstatus->Open(this, "Inns");
    }
}

// Names the popup's entries after the selected row; destroy and inject only in expert mode.
void TFormMain::ShowPopupMenu(TObject *sender, TElTreeStringGrid *grid, String kind)
{
    if (grid->Selected == NULL)
    {
        try
        {
            SetForegroundWindow(((TElPopupMenu *)sender)->WindowHandle);
            SendMessageA(((TElPopupMenu *)sender)->WindowHandle, WM_NULL, 0, 0);
        }
        catch (...)
        {
        }
        return;
    }
    int name_column = 0;
    for (int i = 1; i < grid->HeaderSections->Count; i++)
    {
        if (String(grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_column = i - 1;
            break;
        }
    }
    String row_name(grid->Selected->ColumnText->Strings[name_column]);
    if (row_name == "")
        row_name = "''";
    String row_label = kind + ": " + row_name + " | id:" + grid->Selected->Text;
    ((TElPopupMenu *)sender)->Items->Items[0]->Caption = WideString("new " + kind);
    ((TElPopupMenu *)sender)->Items->Items[2]->Caption = WideString("dupe " + row_label);
    ((TElPopupMenu *)sender)->Items->Items[3]->Caption = WideString("edit " + row_label);
    // Only the last row can be deleted; any other row is freed so later IDs stay put.
    if (grid->Selected->Index >= grid->Items->Count - 1)
        ((TElPopupMenu *)sender)->Items->Items[4]->Caption = WideString("delete " + row_label);
    else
        ((TElPopupMenu *)sender)->Items->Items[4]->Caption = WideString("free " + row_label);
    ((TElPopupMenu *)sender)->Items->Items[5]->Visible = config->GetExpertMode();
    ((TElPopupMenu *)sender)->Items->Items[6]->Visible = config->GetExpertMode();
    ((TElPopupMenu *)sender)->Items->Items[6]->Caption =
        WideString("destroy: " + row_name + " | id:" + grid->Selected->Text);
    ((TElPopupMenu *)sender)->Items->Items[7]->Visible = config->GetExpertMode();
    ((TElPopupMenu *)sender)->Items->Items[7]->Caption = WideString("inject: id:" + grid->Selected->Text);
}

// Deletes the last row, or empties the name of another so it can be recycled, after the warning.
void TFormMain::DeleteRow(TElTreeStringGrid *grid, DataGridUI *grid_ui)
{
    TElWideStrings *cells;
    if (grid->Selected == NULL)
        return;
    int name_column = 0;
    for (int i = 1; i < grid->HeaderSections->Count; i++)
    {
        if (String(grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_column = i - 1;
            break;
        }
    }
    if (config->GetDeleteWarning())
    {
        // Only the last row really goes; the others keep their ID and are emptied for recycling.
        if (grid->Selected->Index >= grid->Items->Count - 1)
        {
            String delete_title = "Delete " + grid->Selected->ColumnText->Strings[name_column];
            String delete_text =
                "Make sure " + grid_ui->record_name + " ID: " + grid->Selected->Text +
                    " is no longer in your gameserver database.";
            String delete_button = "DELETE |  ID: " + grid->Selected->Text;
            FormDeleteWarning->PlaceWarning(this, grid->Width, delete_title, delete_text, delete_button);
        }
        else
        {
            String free_title = "Free " + grid->Selected->ColumnText->Strings[name_column];
            String free_text =
                "The name of " + grid_ui->record_name + " will be emptied. Flaging it available for recycle.";
            String free_button = "FREE |  ID: " + grid->Selected->Text;
            FormDeleteWarning->PlaceWarning(this, grid->Width, free_title, free_text, free_button);
        }
        if (FormDeleteWarning->ShowModal() != mrOk)
            return;
    }
    if (grid->Selected->Index >= grid->Items->Count - 1)
    {
        grid_ui->ShowMessage(grid_ui->record_name + " ID: " + grid->Selected->Text + " deleted");
        grid->Selected->Delete();
        return;
    }
    int name_index = 0;
    for (int j = 1; j < grid->HeaderSections->Count; j++)
    {
        if (String(grid->HeaderSections->Item[j]->Text) == "Name")
        {
            name_index = j - 1;
            break;
        }
    }
    cells = grid->Selected->ColumnText;
    cells->Strings[name_index] = WideString("");
    grid_ui->ShowMessage(grid_ui->record_name + " ID: " + grid->Selected->Text + " emptied (ready to recycle)");
    if (!FormMain->config->GetShowRecycled())
        grid->Selected->Hidden = true;
}

// In expert mode, removes the row and moves every row above it down one ID, after the warning.
void TFormMain::DestroyRow(TElTreeStringGrid *grid, DataGridUI *grid_ui)
{
    if (!config->GetExpertMode())
        return;
    if (grid->Selected == NULL)
        return;
    int name_column = 0;
    for (int i = 1; i < grid->HeaderSections->Count; i++)
    {
        if (String(grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_column = i - 1;
            break;
        }
    }
    String title = "Destroy " + grid->Selected->ColumnText->Strings[name_column];
    String text = "Expert Mode : Destroy will re-index all other data rows above ID " + grid->Selected->Text;
    String button_text = "DESTROY |  ID: " + grid->Selected->Text;
    // Destroying a row shifts every later ID, so the user confirms it first.
    FormDeleteWarning->PlaceWarning(this, grid->Width, title, text, button_text);
    if (FormDeleteWarning->ShowModal() != mrOk)
        return;
    grid_ui->DestroyRow(grid, grid->Selected->Index);
}

// In expert mode, inserts a row and moves every row above it up one ID, after the warning.
void TFormMain::InjectRow(TElTreeStringGrid *grid, DataGridUI *grid_ui)
{
    if (!config->GetExpertMode())
        return;
    if (grid->Selected == NULL)
        return;
    int name_column = 0;
    for (int i = 1; i < grid->HeaderSections->Count; i++)
    {
        if (String(grid->HeaderSections->Item[i]->Text) == "Name")
        {
            name_column = i - 1;
            break;
        }
    }
    String title = "Inject " + grid->Selected->ColumnText->Strings[name_column];
    String text = "Expert Mode : Inject will re-index all other data rows above ID " + grid->Selected->Text;
    String button_text = "INJECT |  ID: " + grid->Selected->Text;
    FormDeleteWarning->PlaceWarning(this, grid->Width, title, text, button_text);
    if (FormDeleteWarning->ShowModal() != mrOk)
        return;
    grid_ui->InjectRow(grid, grid->Selected->Index);
}

void __fastcall TFormMain::ElTreeStringGrid1Enter(TObject *Sender)
{
    if (items_tab)
        items_tab->grid_ui->Unacquire();
}

// Edits an item cell; the gfx and data cells open the item or character graphic picker.
void __fastcall TFormMain::ElTreeStringGrid1InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    int gfx_file;
    if (!items_tab)
        return;
    char edit_action = items_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if ((unsigned char)edit_action == ColumnKind_Index)
    {
        if (items_tab->main_grid->Selected == NULL)
            return;
        FormItemEdit->Open(1, items_tab->main_grid->Selected->Index);
    }
    if ((unsigned char)edit_action == ColumnKind_Graphic)
    {
        int graphic_count = items_tab->grid_ui->gfx_first;
        DataGridCol *name_column = items_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *gfx_column = items_tab->grid_ui->data_grid->FindColumn("Gfx");
        DataGridCol *size_column = items_tab->grid_ui->data_grid->FindColumn("Size");
        if (!name_column || !gfx_column || !size_column)
            return;
        String item_name(Item->ColumnText->Strings[name_column->text_index]);
        int gfx_number = NumberOr(Item->ColumnText->Strings[gfx_column->text_index], 1);
        FormGfx->SetLibrary(gfx_vault->items_library, 102, 2, graphic_count, items_tab->main_grid,
            gfx_column->text_index);
        FormGfx->ShowCaptions(this, "Item Gfx", item_name, gfx_number);
        if (FormGfx->ModalResult == mrOk)
        {
            DataComboItem *size_item = size_column->GetComboItem((short)FormGfx->chosen_size);
            items_tab->main_grid->IsUpdating = true;
            Item->ColumnText->Strings[gfx_column->text_index] = WideString(FormGfx->chosen_number);
            Item->ColumnText->Strings[size_column->text_index] = WideString(size_item->text);
            items_tab->main_grid->IsUpdating = false;
        }
        items_tab->grid_ui->main_spin_editor->Editor->MinValue = 1;
        items_tab->grid_ui->main_spin_editor->Editor->MaxValue = 64000;
        items_tab->grid_ui->main_spin_editor->Editor->Tag = 999999;
        Editor = (TElTreeInplaceEditor *)items_tab->grid_ui->main_spin_editor;
    }
    if ((unsigned char)edit_action == ColumnKind_PaperdollGraphic)
    {
        DataGridCol *item_name_column = items_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *type_column = items_tab->grid_ui->data_grid->FindColumn("Type");
        DataGridCol *doll_gfx_column = items_tab->grid_ui->data_grid->FindColumn("Data.1");
        DataGridCol *gender_column = items_tab->grid_ui->data_grid->FindColumn("Data.2");
        DataComboItem *type_item = type_column->FindComboItem(Item->ColumnText->Strings[type_column->text_index]);
        int item_type = NumberOr((int)type_item->index, 1);
        String name_text(Item->ColumnText->Strings[item_name_column->text_index]);
        int doll_gfx = NumberOr(Item->ColumnText->Strings[doll_gfx_column->text_index], 1);
        int gender = NumberOr(Item->ColumnText->Strings[gender_column->text_index], 0);
        gfx_file = 0;
        int step = 0;
        String caption = "";
        // Each kind of equipment has its own character gfx file and graphic step.
        if (item_type == EifItemType_Boots)
        {
            step = 40;
            gfx_file = 12;
            caption = "Boots";
        }
        if (item_type == EifItemType_Armor)
        {
            step = 50;
            gfx_file = 14;
            caption = "Armor";
        }
        if (item_type == EifItemType_Hat)
        {
            step = 10;
            gfx_file = 16;
            caption = "Hat";
        }
        if (item_type == EifItemType_Weapon)
        {
            step = 100;
            gfx_file = 18;
            caption = "Weapon";
        }
        if (item_type == EifItemType_Shield)
        {
            step = 50;
            gfx_file = 20;
            caption = "Shield";
        }
        if (!gfx_file)
            return;
        caption = "Character " + caption + " Gfx";
        // Armour has a female and a male file; the gender picks between them.
        if (gender > 1)
            gender = 1;
        gfx_file = gfx_file - gender;
        if (item_type == EifItemType_Armor && !gender)
            caption = "Fem. " + caption;
        if (item_type == EifItemType_Armor && gender == 1)
            caption = "Male " + caption;
        if (!FormGfx->OpenLibrary(gfx_file, 101, step, items_tab->main_grid, doll_gfx_column->text_index))
        {
            FormWarning->ShowWarning(this, "Warning EGF",
                "Cannot find gfx file [ " + FormGfx->library_file +
                    " ] in the Read Gfx Path. Please fix your files and try again.");
            return;
        }
        FormGfx->ShowCaptions(this, caption, name_text, doll_gfx);
        if (FormGfx->ModalResult == mrOk)
            Item->ColumnText->Strings[doll_gfx_column->text_index] = WideString(FormGfx->chosen_number);
        items_tab->grid_ui->main_spin_editor->Editor->MinValue = 1;
        items_tab->grid_ui->main_spin_editor->Editor->MaxValue = 64000;
        items_tab->grid_ui->main_spin_editor->Editor->Tag = 999999;
        Editor = (TElTreeInplaceEditor *)items_tab->grid_ui->main_spin_editor;
    }
}

void __fastcall TFormMain::ElTreeStringGrid1ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (items_tab)
        items_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid1ItemFocused(TObject *Sender)
{
    if (items_tab)
        items_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid2Enter(TObject *Sender)
{
    if (items_tab)
        items_tab->grid_ui->Acquire();
}

// Clears the row colours of the item sub grid when it loses the focus.
void __fastcall TFormMain::ElTreeStringGrid2Exit(TObject *Sender)
{
    if (items_tab)
        items_tab->grid_ui->ClearRowColors();
}

// Edits a cell of the item sub grid; the data cell opens the character graphic picker.
void __fastcall TFormMain::ElTreeStringGrid2InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    TElWideStrings *cells;
    int gfx_file;
    if (!items_tab)
        return;
    char edit_action = items_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if ((unsigned char)edit_action == ColumnKind_Index)
    {
        if (items_tab->main_grid->Selected == NULL)
            return;
        FormItemEdit->Open(1, items_tab->main_grid->Selected->Index);
    }
    if ((unsigned char)edit_action == ColumnKind_Graphic)
        imageItemClick((TObject *)imageItem);
    if ((unsigned char)edit_action == ColumnKind_PaperdollGraphic)
    {
        if (items_tab->main_grid->Selected == NULL)
            return;
        TElTreeItem *row = items_tab->main_grid->Selected;
        DataGridCol *name_column = items_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *type_column = items_tab->grid_ui->data_grid->FindColumn("Type");
        DataGridCol *doll_gfx_column = items_tab->grid_ui->data_grid->FindColumn("Data.1");
        DataGridCol *gender_column = items_tab->grid_ui->data_grid->FindColumn("Data.2");
        DataComboItem *type_item = type_column->FindComboItem(row->ColumnText->Strings[type_column->text_index]);
        int item_type = NumberOr((int)type_item->index, 1);
        String item_name(row->ColumnText->Strings[name_column->text_index]);
        int doll_gfx = NumberOr(row->ColumnText->Strings[doll_gfx_column->text_index], 1);
        int gender = NumberOr(row->ColumnText->Strings[gender_column->text_index], 0);
        gfx_file = 0;
        int step = 0;
        String caption = "";
        if (item_type == EifItemType_Boots)
        {
            step = 40;
            gfx_file = 12;
            caption = "Boots";
        }
        if (item_type == EifItemType_Armor)
        {
            step = 50;
            gfx_file = 14;
            caption = "Armor";
        }
        if (item_type == EifItemType_Hat)
        {
            step = 10;
            gfx_file = 16;
            caption = "Hat";
        }
        if (item_type == EifItemType_Weapon)
        {
            step = 100;
            gfx_file = 18;
            caption = "Weapon";
        }
        if (item_type == EifItemType_Shield)
        {
            step = 50;
            gfx_file = 20;
            caption = "Shield";
        }
        if (!gfx_file)
            return;
        caption = "Character " + caption + " Gfx";
        if (gender > 1)
            gender = 1;
        gfx_file = gfx_file - gender;
        if (item_type == EifItemType_Armor && !gender)
            caption = "Fem. " + caption;
        if (item_type == EifItemType_Armor && gender == 1)
            caption = "Male " + caption;
        if (!FormGfx->OpenLibrary(gfx_file, 101, step, items_tab->main_grid, doll_gfx_column->text_index))
        {
            FormWarning->ShowWarning(this, "Warning EGF",
                "Cannot find gfx file [ " + FormGfx->library_file +
                    " ] in the Read Gfx Path. Please fix your files and try again.");
            return;
        }
        FormGfx->ShowCaptions(this, caption, item_name, doll_gfx);
        if (FormGfx->ModalResult == mrOk)
        {
            cells = Item->ColumnText;
            cells->Strings[0] = WideString(FormGfx->chosen_number);
        }
    }
}

// Picks the item's graphic, which also sets its inventory size.
void __fastcall TFormMain::imageItemClick(TObject *Sender)
{
    TElWideStrings *gfx_cells;
    TElWideStrings *size_cells;
    if (items_tab->main_grid->Selected == NULL)
        return;
    int graphic_count = items_tab->grid_ui->gfx_first;
    DataGridCol *name_column = items_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = items_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *size_column = items_tab->grid_ui->data_grid->FindColumn("Size");
    if (!name_column || !gfx_column || !size_column)
        return;
    String item_name(items_tab->main_grid->Selected->ColumnText->Strings[name_column->text_index]);
    int gfx_number = NumberOr(items_tab->main_grid->Selected->ColumnText->Strings[gfx_column->text_index], 1);
    FormGfx->SetLibrary(gfx_vault->items_library, 102, 2, graphic_count, items_tab->main_grid, gfx_column->text_index);
    FormGfx->ShowCaptions(this, "Item Gfx", item_name, gfx_number);
    if (FormGfx->ModalResult == mrOk)
    {
        DataComboItem *size_item = size_column->GetComboItem(FormGfx->chosen_size);
        items_tab->main_grid->IsUpdating = true;
        gfx_cells = items_tab->main_grid->Selected->ColumnText;
        gfx_cells->Strings[gfx_column->text_index] = WideString(FormGfx->chosen_number);
        size_cells = items_tab->main_grid->Selected->ColumnText;
        size_cells->Strings[size_column->text_index] = WideString(size_item->text);
        items_tab->main_grid->IsUpdating = false;
    }
}

void __fastcall TFormMain::ElTreeStringGrid2ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (items_tab)
        items_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::itemPreviewHeaderClick(TObject *Sender)
{
    items_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel9);
}

// Keeps the index column in place while the grid scrolls.
void __fastcall TFormMain::ElTreeStringGrid1Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (items_tab)
        items_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::popupItemsPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, items_tab->main_grid, "item");
}

void __fastcall TFormMain::popupItemNewClick(TObject *Sender)
{
    FormItemEdit->Open(0, 0);
}

void __fastcall TFormMain::popupItemEditClick(TObject *Sender)
{
    if (items_tab->main_grid->Selected != NULL)
        FormItemEdit->Open(1, items_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupItemDupeClick(TObject *Sender)
{
    if (items_tab->main_grid->Selected != NULL)
        FormItemEdit->Open(2, items_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupItemFreeClick(TObject *Sender)
{
    DeleteRow(items_tab->main_grid, items_tab->grid_ui);
}

void __fastcall TFormMain::popupItemDestroyClick(TObject *Sender)
{
    DestroyRow(items_tab->main_grid, items_tab->grid_ui);
}

void __fastcall TFormMain::popupItemInjectClick(TObject *Sender)
{
    InjectRow(items_tab->main_grid, items_tab->grid_ui);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the item grid.
void __fastcall TFormMain::ElTreeStringGrid1KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(items_tab->main_grid, items_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(items_tab->main_grid, items_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::ElTreeStringGrid3Enter(TObject *Sender)
{
    if (classes_tab)
        classes_tab->grid_ui->Unacquire();
}

// Opens the class editor when a class cell asks for it.
void __fastcall TFormMain::ElTreeStringGrid3InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (classes_tab && classes_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor) == ColumnKind_Index &&
        classes_tab->main_grid->Selected != NULL)
        FormEditClass->Open(1, classes_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::ElTreeStringGrid3ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (classes_tab)
        classes_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid3ItemFocused(TObject *Sender)
{
    if (classes_tab)
        classes_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid4Enter(TObject *Sender)
{
    if (classes_tab)
        classes_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid4Exit(TObject *Sender)
{
    if (classes_tab)
        classes_tab->grid_ui->ClearRowColors();
}

void __fastcall TFormMain::ElTreeStringGrid4InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (classes_tab && classes_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor) == ColumnKind_Index &&
        classes_tab->main_grid->Selected != NULL)
        FormEditClass->Open(1, classes_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::ElTreeStringGrid4ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (classes_tab)
        classes_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupClassesPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, classes_tab->main_grid, "class");
}

void __fastcall TFormMain::popupClassNewClick(TObject *Sender)
{
    FormEditClass->Open(0, 0);
}

void __fastcall TFormMain::popupClassEditClick(TObject *Sender)
{
    if (classes_tab->main_grid->Selected != NULL)
        FormEditClass->Open(1, FormMain->classes_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupClassDupeClick(TObject *Sender)
{
    if (classes_tab->main_grid->Selected != NULL)
        FormEditClass->Open(2, FormMain->classes_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupClassFreeClick(TObject *Sender)
{
    DeleteRow(classes_tab->main_grid, classes_tab->grid_ui);
}

void __fastcall TFormMain::popupClassDestroyClick(TObject *Sender)
{
    DestroyRow(classes_tab->main_grid, classes_tab->grid_ui);
}

void __fastcall TFormMain::popupClassInjectClick(TObject *Sender)
{
    InjectRow(classes_tab->main_grid, classes_tab->grid_ui);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the class grid.
void __fastcall TFormMain::ElTreeStringGrid3KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(classes_tab->main_grid, classes_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(classes_tab->main_grid, classes_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::classPreviewHeaderClick(TObject *Sender)
{
    classes_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel25);
}

void __fastcall TFormMain::ElTreeStringGrid3Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (classes_tab)
        classes_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::ElTreeStringGrid5ItemFocused(TObject *Sender)
{
    if (npcs_tab)
        npcs_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid5Enter(TObject *Sender)
{
    if (npcs_tab)
        npcs_tab->grid_ui->Unacquire();
}

void __fastcall TFormMain::ElTreeStringGrid6Enter(TObject *Sender)
{
    if (npcs_tab)
        npcs_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid6Exit(TObject *Sender)
{
    if (npcs_tab)
        npcs_tab->grid_ui->ClearRowColors();
}

// Edits an npc cell; also opens the drops, the chats or the graphic picker.
void __fastcall TFormMain::ElTreeStringGrid5InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    TElWideStrings *cells;
    if (!npcs_tab)
        return;
    unsigned char edit_action = npcs_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        FormNpcEdit->Open(1, npcs_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_NpcDrops)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        int npc_id = StrToInt(npcs_tab->main_grid->Selected->Text);
        FormNpcEditDrops->Open(npc_id - 1);
    }
    if (edit_action == ColumnKind_NpcChats)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        int chat_npc_id = StrToInt(npcs_tab->main_grid->Selected->Text);
        FormNpcEditChats->Open(chat_npc_id - 1);
    }
    if (edit_action == ColumnKind_Graphic)
    {
        int graphic_count = npcs_tab->grid_ui->gfx_first;
        DataGridCol *name_column = npcs_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *gfx_column = npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
        if (!name_column || !gfx_column)
            return;
        String npc_name(Item->ColumnText->Strings[name_column->text_index]);
        int gfx_number = NumberOr(Item->ColumnText->Strings[gfx_column->text_index], 1);
        FormGfx->SetLibrary(gfx_vault->npcs_library, 101, 40, graphic_count, npcs_tab->main_grid,
            gfx_column->text_index);
        FormGfx->ShowCaptions(this, "Npc Gfx", npc_name, gfx_number);
        if (FormGfx->ModalResult == mrOk)
        {
            npcs_tab->main_grid->IsUpdating = true;
            cells = Item->ColumnText;
            cells->Strings[gfx_column->text_index] = WideString(FormGfx->chosen_number);
            npcs_tab->main_grid->IsUpdating = false;
        }
        npcs_tab->grid_ui->main_spin_editor->Editor->MinValue = 1;
        npcs_tab->grid_ui->main_spin_editor->Editor->MaxValue = 64000;
        npcs_tab->grid_ui->main_spin_editor->Editor->Tag = 999999;
        Editor = (TElTreeInplaceEditor *)npcs_tab->grid_ui->main_spin_editor;
    }
}

// Edits a cell of the npc sub grid; also opens the drops, the chats or the graphic picker.
void __fastcall TFormMain::ElTreeStringGrid6InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!npcs_tab)
        return;
    unsigned char edit_action = npcs_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        FormNpcEdit->Open(1, npcs_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_NpcDrops)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        int npc_id = StrToInt(npcs_tab->main_grid->Selected->Text);
        FormNpcEditDrops->Open(npc_id - 1);
    }
    if (edit_action == ColumnKind_NpcChats)
    {
        if (npcs_tab->main_grid->Selected == NULL)
            return;
        int chat_npc_id = StrToInt(npcs_tab->main_grid->Selected->Text);
        FormNpcEditChats->Open(chat_npc_id - 1);
    }
    if (edit_action == ColumnKind_Graphic)
        imageNpcClick((TObject *)imageNpc);
}

void __fastcall TFormMain::imageNpcClick(TObject *Sender)
{
    TElWideStrings *cells;
    if (npcs_tab->main_grid->Selected == NULL)
        return;
    int graphic_count = npcs_tab->grid_ui->gfx_first;
    DataGridCol *name_column = npcs_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    if (!name_column || !gfx_column)
        return;
    String npc_name(npcs_tab->main_grid->Selected->ColumnText->Strings[name_column->text_index]);
    int gfx_number = NumberOr(npcs_tab->main_grid->Selected->ColumnText->Strings[gfx_column->text_index], 1);
    FormGfx->SetLibrary(gfx_vault->npcs_library, 101, 40, graphic_count, npcs_tab->main_grid, gfx_column->text_index);
    FormGfx->ShowCaptions(this, "Npc Gfx", npc_name, gfx_number);
    if (FormGfx->ModalResult == mrOk)
    {
        npcs_tab->main_grid->IsUpdating = true;
        cells = npcs_tab->main_grid->Selected->ColumnText;
        cells->Strings[gfx_column->text_index] = WideString(FormGfx->chosen_number);
        npcs_tab->main_grid->IsUpdating = false;
    }
}

void __fastcall TFormMain::ElTreeStringGrid5ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (npcs_tab)
        npcs_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid6ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (npcs_tab)
        npcs_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupNpcsPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, npcs_tab->main_grid, "npc");
}

void __fastcall TFormMain::popupNpcNewClick(TObject *Sender)
{
    FormNpcEdit->Open(0, 0);
}

void __fastcall TFormMain::popupNpcEditClick(TObject *Sender)
{
    if (npcs_tab->main_grid->Selected != NULL)
        FormNpcEdit->Open(1, npcs_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupNpcDupeClick(TObject *Sender)
{
    if (npcs_tab->main_grid->Selected != NULL)
        FormNpcEdit->Open(2, npcs_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupNpcFreeClick(TObject *Sender)
{
    DeleteRow(npcs_tab->main_grid, npcs_tab->grid_ui);
}

void __fastcall TFormMain::popupNpcDestroyClick(TObject *Sender)
{
    DestroyRow(npcs_tab->main_grid, npcs_tab->grid_ui);
}

void __fastcall TFormMain::popupNpcInjectClick(TObject *Sender)
{
    InjectRow(npcs_tab->main_grid, npcs_tab->grid_ui);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the npc grid.
void __fastcall TFormMain::ElTreeStringGrid5KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(npcs_tab->main_grid, npcs_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(npcs_tab->main_grid, npcs_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::npcPreviewHeaderClick(TObject *Sender)
{
    npcs_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel46);
}

void __fastcall TFormMain::ElTreeStringGrid5Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (npcs_tab)
        npcs_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::ElTreeStringGrid7Enter(TObject *Sender)
{
    if (skills_tab)
        skills_tab->grid_ui->Unacquire();
}

// Edits a skill cell; the icon and effect cells open their graphic pickers.
void __fastcall TFormMain::ElTreeStringGrid7InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    TElWideStrings *effect_cells;
    TElWideStrings *icon_cells;
    if (!skills_tab)
        return;
    unsigned char edit_action = skills_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (skills_tab->main_grid->Selected == NULL)
            return;
        FormSkillEdit->Open(1, skills_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_Graphic)
    {
        DataGridCol *name_column = skills_tab->grid_ui->data_grid->FindColumn("Name");
        DataGridCol *icon_column = skills_tab->grid_ui->data_grid->FindColumn("Gfx");
        DataGridCol *effect_column = skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
        if (!name_column || !icon_column)
            return;
        String skill_name(Item->ColumnText->Strings[name_column->text_index]);
        if (icon_column->index == SectionIndex)
        {
            int icon_count = skills_tab->grid_ui->icon_first;
            int icon_number = NumberOr(Item->ColumnText->Strings[icon_column->text_index], 1);
            FormGfx->SetLibrary(gfx_vault->icons_library, 101, 1, icon_count, skills_tab->main_grid,
                icon_column->text_index);
            FormGfx->ShowCaptions(this, "Icon Gfx", skill_name, icon_number);
            if (FormGfx->ModalResult == mrOk)
            {
                skills_tab->main_grid->IsUpdating = true;
                icon_cells = Item->ColumnText;
                icon_cells->Strings[icon_column->text_index] = WideString(FormGfx->chosen_number);
                skills_tab->main_grid->IsUpdating = false;
            }
        }
        if (effect_column->index == SectionIndex)
        {
            int effect_count = skills_tab->grid_ui->gfx_first;
            int effect_number = NumberOr(Item->ColumnText->Strings[effect_column->text_index], 1);
            FormGfx->SetLibrary(gfx_vault->skills_library, 101, 3, effect_count, skills_tab->main_grid,
                icon_column->text_index);
            FormGfx->SetDrawMode(true);
            FormGfx->ShowCaptions(this, "Effect Gfx", skill_name, effect_number);
            if (FormGfx->ModalResult == mrOk)
            {
                skills_tab->main_grid->IsUpdating = true;
                effect_cells = Item->ColumnText;
                effect_cells->Strings[effect_column->text_index] = WideString(FormGfx->chosen_number);
                skills_tab->main_grid->IsUpdating = false;
            }
        }
        skills_tab->grid_ui->main_spin_editor->Editor->MinValue = 1;
        skills_tab->grid_ui->main_spin_editor->Editor->MaxValue = 64000;
        skills_tab->grid_ui->main_spin_editor->Editor->Tag = 999999;
        Editor = (TElTreeInplaceEditor *)skills_tab->grid_ui->main_spin_editor;
    }
}

void __fastcall TFormMain::ElTreeStringGrid7ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (skills_tab)
        skills_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid7ItemFocused(TObject *Sender)
{
    if (skills_tab)
        skills_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid8Enter(TObject *Sender)
{
    if (skills_tab)
        skills_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid8Exit(TObject *Sender)
{
    if (skills_tab)
        skills_tab->grid_ui->ClearRowColors();
}

// Edits a cell of the skill sub grid; the icon and effect cells open their pickers.
void __fastcall TFormMain::ElTreeStringGrid8InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!skills_tab)
        return;
    unsigned char edit_action = skills_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (skills_tab->main_grid->Selected == NULL)
            return;
        FormSkillEdit->Open(1, skills_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_Graphic)
    {
        DataGridCol *icon_column = skills_tab->grid_ui->data_grid->FindColumn("Gfx");
        DataGridCol *effect_column = skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
        DataGridCol *effect_gfx_column = skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
        if (!icon_column || !effect_column)
            return;
        if (Item->Tag == icon_column->index)
        {
            imageIconSkillClick((TObject *)imageIconSkill);
            return;
        }
        if (Item->Tag == effect_gfx_column->index)
        {
            imageSkillClick((TObject *)imageSkill);
            return;
        }
    }
}

// Picks the skill's effect, its layers drawn together.
void __fastcall TFormMain::imageSkillClick(TObject *Sender)
{
    TElWideStrings *cells;
    if (skills_tab->main_grid->Selected == NULL)
        return;
    int effect_count = skills_tab->grid_ui->gfx_first;
    DataGridCol *name_column = skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *effect_column = skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
    if (!name_column || !effect_column)
        return;
    String skill_name(skills_tab->main_grid->Selected->ColumnText->Strings[name_column->text_index]);
    int effect_number = NumberOr(skills_tab->main_grid->Selected->ColumnText->Strings[effect_column->text_index], 1);
    FormGfx->SetLibrary(gfx_vault->skills_library, 101, 3, effect_count, skills_tab->main_grid,
        effect_column->text_index);
    FormGfx->SetDrawMode(true);
    FormGfx->ShowCaptions(this, "Effect Gfx", skill_name, effect_number);
    if (FormGfx->ModalResult == mrOk)
    {
        skills_tab->main_grid->IsUpdating = true;
        cells = skills_tab->main_grid->Selected->ColumnText;
        cells->Strings[effect_column->text_index] = WideString(FormGfx->chosen_number);
        skills_tab->main_grid->IsUpdating = false;
    }
}

void __fastcall TFormMain::imageIconSkillClick(TObject *Sender)
{
    TElWideStrings *cells;
    if (skills_tab->main_grid->Selected == NULL)
        return;
    int icon_count = skills_tab->grid_ui->icon_first;
    DataGridCol *name_column = skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *icon_column = skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    if (!name_column || !icon_column)
        return;
    String skill_name(skills_tab->main_grid->Selected->ColumnText->Strings[name_column->text_index]);
    int icon_number = NumberOr(skills_tab->main_grid->Selected->ColumnText->Strings[icon_column->text_index], 1);
    FormGfx->SetLibrary(gfx_vault->icons_library, 101, 1, icon_count, skills_tab->main_grid, icon_column->text_index);
    FormGfx->SetDrawMode(false);
    FormGfx->ShowCaptions(this, "Icon Gfx", skill_name, icon_number);
    if (FormGfx->ModalResult == mrOk)
    {
        skills_tab->main_grid->IsUpdating = true;
        cells = skills_tab->main_grid->Selected->ColumnText;
        cells->Strings[icon_column->text_index] = WideString(FormGfx->chosen_number);
        skills_tab->main_grid->IsUpdating = false;
    }
}

void __fastcall TFormMain::ElTreeStringGrid8ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (skills_tab)
        skills_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupSkillsPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, skills_tab->main_grid, "skill");
}

void __fastcall TFormMain::popupSkillNewClick(TObject *Sender)
{
    FormSkillEdit->Open(0, 0);
}

void __fastcall TFormMain::popupSkillDupeClick(TObject *Sender)
{
    if (skills_tab->main_grid->Selected != NULL)
        FormSkillEdit->Open(2, skills_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupSkillEditClick(TObject *Sender)
{
    if (skills_tab->main_grid->Selected != NULL)
        FormSkillEdit->Open(1, skills_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupSkillFreeClick(TObject *Sender)
{
    DeleteRow(skills_tab->main_grid, skills_tab->grid_ui);
}

void __fastcall TFormMain::popupSkillDestroyClick(TObject *Sender)
{
    DestroyRow(skills_tab->main_grid, skills_tab->grid_ui);
}

void __fastcall TFormMain::popupSkillInjectClick(TObject *Sender)
{
    InjectRow(skills_tab->main_grid, skills_tab->grid_ui);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the skill grid.
void __fastcall TFormMain::ElTreeStringGrid7KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(skills_tab->main_grid, skills_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(skills_tab->main_grid, skills_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::skillPreviewHeaderClick(TObject *Sender)
{
    npcs_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel60);
}

void __fastcall TFormMain::ElTreeStringGrid7Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (skills_tab)
        skills_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::ElTreeStringGrid9Enter(TObject *Sender)
{
    if (shops_tab)
        shops_tab->grid_ui->Unacquire();
}

// Edits a shop cell; also opens the shop's items or crafts.
void __fastcall TFormMain::ElTreeStringGrid9InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!shops_tab)
        return;
    unsigned char edit_action = shops_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        FormEditShop->Open(1, shops_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_ShopTrades)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        int shop_id = StrToInt(shops_tab->main_grid->Selected->Text);
        FormShopEditItems->Open(shop_id - 1);
    }
    if (edit_action == ColumnKind_ShopCrafts)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        int craft_shop_id = StrToInt(shops_tab->main_grid->Selected->Text);
        FormShopEditCrafts->Open(craft_shop_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid9ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (shops_tab)
        shops_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid9ItemFocused(TObject *Sender)
{
    if (shops_tab)
        shops_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid10Enter(TObject *Sender)
{
    if (shops_tab)
        shops_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid10Exit(TObject *Sender)
{
    if (shops_tab)
        shops_tab->grid_ui->ClearRowColors();
}

// Edits a cell of the shop sub grid; also opens the shop's items or crafts.
void __fastcall TFormMain::ElTreeStringGrid10InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!shops_tab)
        return;
    unsigned char edit_action = shops_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        FormEditShop->Open(1, shops_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_ShopTrades)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        int shop_id = StrToInt(shops_tab->main_grid->Selected->Text);
        FormShopEditItems->Open(shop_id - 1);
    }
    if (edit_action == ColumnKind_ShopCrafts)
    {
        if (shops_tab->main_grid->Selected == NULL)
            return;
        int craft_shop_id = StrToInt(shops_tab->main_grid->Selected->Text);
        FormShopEditCrafts->Open(craft_shop_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid10ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (shops_tab)
        shops_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupShopsPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, shops_tab->main_grid, "shop");
}

void __fastcall TFormMain::popupShopNewClick(TObject *Sender)
{
    FormEditShop->Open(0, 0);
}

void __fastcall TFormMain::popupShopEditClick(TObject *Sender)
{
    if (shops_tab->main_grid->Selected != NULL)
        FormEditShop->Open(1, FormMain->shops_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupShopDupeClick(TObject *Sender)
{
    if (shops_tab->main_grid->Selected != NULL)
        FormEditShop->Open(2, FormMain->shops_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupShopFreeClick(TObject *Sender)
{
    DeleteRow(shops_tab->main_grid, shops_tab->grid_ui);
}

void __fastcall TFormMain::popupShopDestroyClick(TObject *Sender)
{
    DestroyRow(shops_tab->main_grid, shops_tab->grid_ui);
}

void __fastcall TFormMain::popupShopInjectClick(TObject *Sender)
{
    InjectRow(shops_tab->main_grid, shops_tab->grid_ui);
}

void __fastcall TFormMain::shopPreviewHeaderClick(TObject *Sender)
{
    shops_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel84);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the shop grid.
void __fastcall TFormMain::ElTreeStringGrid9KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(shops_tab->main_grid, shops_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(shops_tab->main_grid, shops_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::ElTreeStringGrid9Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (shops_tab)
        shops_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::ElTreeStringGrid11Enter(TObject *Sender)
{
    if (trainers_tab)
        trainers_tab->grid_ui->Unacquire();
}

// Edits a trainer cell; also opens the trainer's skills.
void __fastcall TFormMain::ElTreeStringGrid11InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!trainers_tab)
        return;
    unsigned char edit_action = trainers_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (trainers_tab->main_grid->Selected == NULL)
            return;
        FormEditTrainer->Open(1, trainers_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_TrainerSkills)
    {
        if (trainers_tab->main_grid->Selected == NULL)
            return;
        int trainer_id = StrToInt(trainers_tab->main_grid->Selected->Text);
        FormTrainerEditSkills->Open(trainer_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid11ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (trainers_tab)
        trainers_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid11ItemFocused(TObject *Sender)
{
    if (trainers_tab)
        trainers_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid12Enter(TObject *Sender)
{
    if (trainers_tab)
        trainers_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid12Exit(TObject *Sender)
{
    if (trainers_tab)
        trainers_tab->grid_ui->ClearRowColors();
}

// Edits a cell of the trainer sub grid; also opens the trainer's skills.
void __fastcall TFormMain::ElTreeStringGrid12InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!trainers_tab)
        return;
    unsigned char edit_action = trainers_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (trainers_tab->main_grid->Selected == NULL)
            return;
        FormEditTrainer->Open(1, trainers_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_TrainerSkills)
    {
        if (trainers_tab->main_grid->Selected == NULL)
            return;
        int trainer_id = StrToInt(trainers_tab->main_grid->Selected->Text);
        FormTrainerEditSkills->Open(trainer_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid12ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (trainers_tab)
        trainers_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupTrainersPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, trainers_tab->main_grid, "trainer");
}

void __fastcall TFormMain::popupTrainerNewClick(TObject *Sender)
{
    FormEditTrainer->Open(0, 0);
}

void __fastcall TFormMain::popupTrainerEditClick(TObject *Sender)
{
    if (trainers_tab->main_grid->Selected != NULL)
        FormEditTrainer->Open(1, FormMain->trainers_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupTrainerDupeClick(TObject *Sender)
{
    if (trainers_tab->main_grid->Selected != NULL)
        FormEditTrainer->Open(2, FormMain->trainers_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupTrainerFreeClick(TObject *Sender)
{
    DeleteRow(trainers_tab->main_grid, trainers_tab->grid_ui);
}

void __fastcall TFormMain::popupTrainerDestroyClick(TObject *Sender)
{
    DestroyRow(trainers_tab->main_grid, trainers_tab->grid_ui);
}

void __fastcall TFormMain::popupTrainerInjectClick(TObject *Sender)
{
    InjectRow(trainers_tab->main_grid, trainers_tab->grid_ui);
}

void __fastcall TFormMain::trainerPreviewHeaderClick(TObject *Sender)
{
    trainers_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel102);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the trainer grid.
void __fastcall TFormMain::ElTreeStringGrid11KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(trainers_tab->main_grid, trainers_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(trainers_tab->main_grid, trainers_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::ElTreeStringGrid11Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (trainers_tab)
        trainers_tab->grid_ui->LockFirstColumnOnScroll();
}

void __fastcall TFormMain::ElTreeStringGrid13Enter(TObject *Sender)
{
    if (inns_tab)
        inns_tab->grid_ui->Unacquire();
}

// Edits an inn cell; also opens the inn's quiz.
void __fastcall TFormMain::ElTreeStringGrid13InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!inns_tab)
        return;
    unsigned char edit_action = inns_tab->grid_ui->EditMainCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (inns_tab->main_grid->Selected == NULL)
            return;
        FormEditInn->Open(1, inns_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_InnQuiz)
    {
        if (inns_tab->main_grid->Selected == NULL)
            return;
        int inn_id = StrToInt(inns_tab->main_grid->Selected->Text);
        FormEditInnQuiz->Open(inn_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid13ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (inns_tab)
        inns_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::ElTreeStringGrid13ItemFocused(TObject *Sender)
{
    if (inns_tab)
        inns_tab->grid_ui->SelectionChange(Sender, true);
}

void __fastcall TFormMain::ElTreeStringGrid14Enter(TObject *Sender)
{
    if (inns_tab)
        inns_tab->grid_ui->Acquire();
}

void __fastcall TFormMain::ElTreeStringGrid14Exit(TObject *Sender)
{
    if (inns_tab)
        inns_tab->grid_ui->ClearRowColors();
}

// Edits a cell of the inn sub grid; also opens the inn's quiz.
void __fastcall TFormMain::ElTreeStringGrid14InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor *&Editor)
{
    if (!inns_tab)
        return;
    unsigned char edit_action = inns_tab->grid_ui->EditSubCell(Item, SectionIndex, Editor);
    if (edit_action == ColumnKind_Index)
    {
        if (inns_tab->main_grid->Selected == NULL)
            return;
        FormEditInn->Open(1, inns_tab->main_grid->Selected->Index);
    }
    if (edit_action == ColumnKind_InnQuiz)
    {
        if (inns_tab->main_grid->Selected == NULL)
            return;
        int inn_id = StrToInt(inns_tab->main_grid->Selected->Text);
        FormEditInnQuiz->Open(inn_id - 1);
    }
}

void __fastcall TFormMain::ElTreeStringGrid14ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode)
{
    if (inns_tab)
        inns_tab->grid_ui->SelectionChange(Sender, false);
}

void __fastcall TFormMain::popupInnsPopup(TObject *Sender)
{
    ShowPopupMenu(Sender, inns_tab->main_grid, "inn");
}

void __fastcall TFormMain::popupInnNewClick(TObject *Sender)
{
    FormEditInn->Open(0, 0);
}

void __fastcall TFormMain::popupInnEditClick(TObject *Sender)
{
    if (inns_tab->main_grid->Selected != NULL)
        FormEditInn->Open(1, FormMain->inns_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupInnDupeClick(TObject *Sender)
{
    if (inns_tab->main_grid->Selected != NULL)
        FormEditInn->Open(2, FormMain->inns_tab->main_grid->Selected->Index);
}

void __fastcall TFormMain::popupInnFreeClick(TObject *Sender)
{
    DeleteRow(inns_tab->main_grid, inns_tab->grid_ui);
}

void __fastcall TFormMain::popupInnDestroyClick(TObject *Sender)
{
    DestroyRow(inns_tab->main_grid, inns_tab->grid_ui);
}

void __fastcall TFormMain::popupInnInjectClick(TObject *Sender)
{
    InjectRow(inns_tab->main_grid, inns_tab->grid_ui);
}

void __fastcall TFormMain::innPreviewHeaderClick(TObject *Sender)
{
    inns_tab->grid_ui->ToggleGfxViewer((TControl *)Sender, Panel114);
}

// Insert injects, Delete deletes, and the Ctrl hotkeys work on the inn grid.
void __fastcall TFormMain::ElTreeStringGrid13KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_INSERT)
    {
        InjectRow(inns_tab->main_grid, inns_tab->grid_ui);
        Key = 0;
    }
    if (Key == VK_DELETE)
        DeleteRow(inns_tab->main_grid, inns_tab->grid_ui);
    if (Shift.Contains(ssCtrl))
    {
        if (Key == 'C')
            HotkeyCopy();
        if (Key == 'D')
            HotkeyDupe();
        if (Key == 'E')
            HotkeyEdit();
        if (Key == 'G')
            HotkeyImage();
        if (Key == 'I')
            HotkeyStatus();
        if (Key == 'N')
            HotkeyNew();
        if (Key == 'V')
            HotkeyPaste();
        if (Key == 'T')
            HotkeyTools();
    }
}

void __fastcall TFormMain::ElTreeStringGrid13Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode)
{
    if (inns_tab)
        inns_tab->grid_ui->LockFirstColumnOnScroll();
}
