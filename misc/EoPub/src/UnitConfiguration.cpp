#include <vcl.h>
#pragma hdrstop

#include "UnitConfiguration.h"
#include "UnitMain.h"
#include "UnitAbout.h"
#include "UnitWarning.h"
#include "UnitWarningExpertMode.h"
#include "UnitFolderSelect.h"

#pragma package(smart_init)
#pragma link "ElPgCtl"
#pragma link "ElCheckCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormConfiguration *FormConfiguration;

__fastcall TFormConfiguration::TFormConfiguration(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    status_ticks = 0;
    enabled_text = " Enabled ";
    disabled_text = " Disabled ";
}

// Themes the form and shows every setting as it is now.
void __fastcall TFormConfiguration::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    if (FormMain->config->GetLoadedTheme() == "dark")
        checkDarkmode->Checked = true;
    else
        checkDarkmode->Checked = false;
    editGfxPath->Caption = FormMain->config->GetGfxPath();
    editPubPath->Caption = FormMain->config->GetPubPath();
    editExportPath->Caption = FormMain->config->GetExportPath();
    editBackupPath->Caption = FormMain->config->GetBackupPath();
    checkBackup->Checked = FormMain->config->GetBackup();
    checkBackupFolders->Checked = FormMain->config->GetBackupFolders();
    checkViewAll->Checked = FormMain->config->GetViewAll();
    checkShowRecycled->Checked = FormMain->config->GetShowRecycled();
    checkCreateRecycled->Checked = FormMain->config->GetCreateRecycled();
    checkListNumbers->Checked = FormMain->config->GetNumberedLists();
    checkDeleteWarning->Checked = FormMain->config->GetDeleteWarning();
    checkPasteWarning->Checked = FormMain->config->GetPasteWarning();
    checkNameHints->Checked = FormMain->config->GetNameHints();
    checkRowSelect->Checked = FormMain->config->GetRowSelect();
    checkSmartFilter->Checked = FormMain->config->GetSmartFilter();
    checkSmartEditors->Checked = FormMain->config->GetSmartEditors();
    checkShowZeros->Checked = FormMain->config->GetShowZeros();
    checkShowBasics->Checked = FormMain->config->GetShowBasics();
    checkMouseQuickedit->Checked = FormMain->config->GetMouseQuickEdit();
    checkMouseQuicksave->Checked = FormMain->config->GetMouseQuickSave();
    checkAutoCapitalization->Checked = FormMain->config->GetAutoCapitalization();
    checkExitSave->Checked = FormMain->config->GetExitSave();
    checkExportEmpty->Checked = FormMain->config->GetExportEmpty();
    checkFullscreen->Checked = FormMain->config->GetFullscreen();
    checkHotkeys->Checked = FormMain->config->GetHotkeys();
    checkExpertMode->Checked = FormMain->config->GetExpertMode();
    shown_pub_path = editPubPath->Caption;
    CheckPaths();
    editGfxPath->Hint = editGfxPath->Caption + "\nclick to open path";
    editPubPath->Hint = editPubPath->Caption + "\nclick to open path";
    editExportPath->Hint = editExportPath->Caption + "\nclick to open path";
    editBackupPath->Hint = editBackupPath->Caption + "\nclick to open path";
    Label4->Hint =
        String("export path to save pub files\nrecommended: same path as [ pub read path ] \n ( to overwrite the pub "
            "files on export )");
    old_export_path = "";
    ElPageControl1->SetFocus();
}

// Checks the four folders and shows which files are missing; the gfx folder must be found to save.
void TFormConfiguration::CheckPaths()
{
    problemText->Caption = "Settings will be saved to " + FormMain->config->GetExeName() + ".ini";
    buttonOk->Caption = WideString("SAVE");
    if (FormMain->config_check->CheckBackupPath(editBackupPath->Caption))
    {
        ImageBackupFileOk->Visible = true;
        ImageBackupFileError->Visible = false;
    }
    else
    {
        ImageBackupFileOk->Visible = false;
        ImageBackupFileError->Visible = true;
    }
    if (FormMain->config_check->CheckExportPath(editExportPath->Caption))
    {
        ImagePubExportFileOk->Visible = true;
        imagePubExportFileError->Visible = false;
    }
    else
    {
        ImagePubExportFileOk->Visible = false;
        imagePubExportFileError->Visible = true;
    }
    if (FormMain->config_check->CheckPubPath(editPubPath->Caption))
    {
        ImagePubFileOk->Visible = true;
        imagePubFileError->Visible = false;
    }
    else
    {
        problemText->Caption = "Editor will create new pub list(s) because no pub file(s) found.";
        buttonOk->Caption = WideString("SAVE BUT CANNOT READ PUB(S)");
        ImagePubFileOk->Visible = false;
        imagePubFileError->Visible = true;
        String pub_problems = " missing pub files:";
        int missing_pubs = 0;
        if (!FormMain->config_check->GetPubFolderFound())
            pub_problems += "Cannot find pub folder";
        if (!FormMain->config_check->GetClassPubFound())
        {
            missing_pubs++;
            pub_problems += "\nCannot find : " + FormMain->config_check->GetClassPubPath();
        }
        if (!FormMain->config_check->GetItemPubFound())
        {
            missing_pubs++;
            pub_problems += "\nCannot find : " + FormMain->config_check->GetItemPubPath();
        }
        if (!FormMain->config_check->GetNpcPubFound())
        {
            missing_pubs++;
            pub_problems += "\nCannot find : " + FormMain->config_check->GetNpcPubPath();
        }
        if (!FormMain->config_check->GetSkillPubFound())
        {
            missing_pubs++;
            pub_problems += "\nCannot find : " + FormMain->config_check->GetSkillPubPath();
        }
        imagePubFileError->Hint = IntToStr(missing_pubs) + pub_problems;
    }
    // Without the gfx files the settings cannot be saved.
    if (FormMain->config_check->CheckGfxPath(editGfxPath->Caption))
    {
        ImageGfxFileOk->Visible = true;
        imageGfxFileError->Visible = false;
    }
    else
    {
        problemText->Caption = "Must enter the gfx folder to continue.";
        buttonOk->Caption = WideString("CANNOT SAVE");
        ImageGfxFileOk->Visible = false;
        imageGfxFileError->Visible = true;
        String gfx_problems = " missing gfx files:";
        int missing_gfx = 0;
        if (!FormMain->config_check->GetGfxFolderFound())
            gfx_problems += "Cannot find gfx folder";
        if (!FormMain->config_check->GetGfx021Found())
        {
            missing_gfx++;
            gfx_problems += "\nCannot find : " + FormMain->config_check->GetGfx021Path();
        }
        if (!FormMain->config_check->GetGfx023Found())
        {
            missing_gfx++;
            gfx_problems += "\nCannot find : " + FormMain->config_check->GetGfx023Path();
        }
        if (!FormMain->config_check->GetGfx024Found())
        {
            missing_gfx++;
            gfx_problems += "\nCannot find : " + FormMain->config_check->GetGfx024Path();
        }
        if (!FormMain->config_check->GetGfx025Found())
        {
            missing_gfx++;
            gfx_problems += "\nCannot find : " + FormMain->config_check->GetGfx025Path();
        }
        imageGfxFileError->Hint = IntToStr(missing_gfx) + gfx_problems;
    }
}

// Picks the gfx folder.
void __fastcall TFormConfiguration::SpeedButton3Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Read gfx folder", editGfxPath->Caption, 1, false);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editGfxPath->Caption = FormFolderSelect->SelectedFolder();
        editGfxPath->Hint = FormFolderSelect->SelectedFolder() + "\nclick to open path";
        CheckPaths();
    }
}

// Picks the pub folder.
void __fastcall TFormConfiguration::SpeedButton4Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Read pub folder", editPubPath->Caption, 1, true);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editPubPath->Caption = FormFolderSelect->SelectedFolder();
        editPubPath->Hint = FormFolderSelect->SelectedFolder() + "\nclick to open path";
        CheckPaths();
    }
}

// Picks the folder pubs are exported to.
void __fastcall TFormConfiguration::SpeedButton1Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Export pub folder", editExportPath->Caption, 1, 2);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editExportPath->Caption = FormFolderSelect->SelectedFolder();
        editExportPath->Hint = FormFolderSelect->SelectedFolder() + "\nclick to open path";
        CheckPaths();
    }
}

// Picks the backup folder.
void __fastcall TFormConfiguration::SpeedButton5Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Backup pub folder", editBackupPath->Caption, 1, 3);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editBackupPath->Caption = FormFolderSelect->SelectedFolder();
        editBackupPath->Hint = FormFolderSelect->SelectedFolder() + "\nclick to open path";
        CheckPaths();
    }
}

// Opens the folder in Explorer when it exists; otherwise picks another.
void __fastcall TFormConfiguration::editGfxPathClick(TObject *Sender)
{
    if (FormMain->config_check->PathExists(((TLabel *)Sender)->Caption))
    {
        String path = ((TLabel *)Sender)->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton3Click((TObject *)SpeedButton3);
}

// Opens the pub folder in Explorer when it exists; otherwise picks another.
void __fastcall TFormConfiguration::editPubPathClick(TObject *Sender)
{
    if (FormMain->config_check->PathExists(((TLabel *)Sender)->Caption))
    {
        String path = ((TLabel *)Sender)->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton4Click((TObject *)SpeedButton4);
}

void __fastcall TFormConfiguration::editExportPathClick(TObject *Sender)
{
    if (FormMain->config_check->PathExists(((TLabel *)Sender)->Caption))
    {
        String path = ((TLabel *)Sender)->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton1Click((TObject *)SpeedButton1);
}

void __fastcall TFormConfiguration::editBackupPathClick(TObject *Sender)
{
    if (FormMain->config_check->PathExists(((TLabel *)Sender)->Caption))
    {
        String path = ((TLabel *)Sender)->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton5Click((TObject *)SpeedButton5);
}

// Writes Enabled or Disabled on every switch, and hides the save note after a while.
void __fastcall TFormConfiguration::Timer1Timer(TObject *Sender)
{
    if (!Visible)
        return;
    if (status_ticks < 50)
    {
        status_ticks++;
        if (status_ticks >= 49)
            problemText->Visible = false;
    }
    // Every switch shows Enabled or Disabled beside it.
    if (checkBackup->Checked)
        checkBackup->Caption = WideString(" Backup" + enabled_text);
    else
        checkBackup->Caption = WideString(" Backup" + disabled_text);
    if (checkShowRecycled->Checked)
        checkShowRecycled->Caption = WideString(enabled_text);
    else
        checkShowRecycled->Caption = WideString(disabled_text);
    if (checkCreateRecycled->Checked)
        checkCreateRecycled->Caption = WideString(enabled_text);
    else
        checkCreateRecycled->Caption = WideString(disabled_text);
    if (checkDeleteWarning->Checked)
        checkDeleteWarning->Caption = WideString(enabled_text);
    else
        checkDeleteWarning->Caption = WideString(disabled_text);
    if (checkPasteWarning->Checked)
        checkPasteWarning->Caption = WideString(enabled_text);
    else
        checkPasteWarning->Caption = WideString(disabled_text);
    if (checkViewAll->Checked)
        checkViewAll->Caption = WideString(enabled_text);
    else
        checkViewAll->Caption = WideString(disabled_text);
    if (checkListNumbers->Checked)
        checkListNumbers->Caption = WideString(enabled_text);
    else
        checkListNumbers->Caption = WideString(disabled_text);
    if (checkNameHints->Checked)
        checkNameHints->Caption = WideString(enabled_text);
    else
        checkNameHints->Caption = WideString(disabled_text);
    if (checkRowSelect->Checked)
        checkRowSelect->Caption = WideString(enabled_text);
    else
        checkRowSelect->Caption = WideString(disabled_text);
    if (checkSmartFilter->Checked)
        checkSmartFilter->Caption = WideString(enabled_text);
    else
        checkSmartFilter->Caption = WideString(disabled_text);
    if (checkSmartEditors->Checked)
        checkSmartEditors->Caption = WideString(enabled_text);
    else
        checkSmartEditors->Caption = WideString(disabled_text);
    if (checkShowZeros->Checked)
        checkShowZeros->Caption = WideString(enabled_text);
    else
        checkShowZeros->Caption = WideString(disabled_text);
    if (checkShowBasics->Checked)
        checkShowBasics->Caption = WideString(enabled_text);
    else
        checkShowBasics->Caption = WideString(disabled_text);
    if (checkMouseQuickedit->Checked)
        checkMouseQuickedit->Caption = WideString(enabled_text);
    else
        checkMouseQuickedit->Caption = WideString(disabled_text);
    if (checkMouseQuicksave->Checked)
        checkMouseQuicksave->Caption = WideString(enabled_text);
    else
        checkMouseQuicksave->Caption = WideString(disabled_text);
    if (checkAutoCapitalization->Checked)
        checkAutoCapitalization->Caption = WideString(enabled_text);
    else
        checkAutoCapitalization->Caption = WideString(disabled_text);
    if (checkDarkmode->Checked)
        checkDarkmode->Caption = WideString(enabled_text);
    else
        checkDarkmode->Caption = WideString(disabled_text);
    if (checkFullscreen->Checked)
        checkFullscreen->Caption = WideString(enabled_text);
    else
        checkFullscreen->Caption = WideString(disabled_text);
    if (checkExportEmpty->Checked)
        checkExportEmpty->Caption = WideString(enabled_text);
    else
        checkExportEmpty->Caption = WideString(disabled_text);
    if (checkExitSave->Checked)
        checkExitSave->Caption = WideString(enabled_text);
    else
        checkExitSave->Caption = WideString(disabled_text);
    if (checkHotkeys->Checked)
        checkHotkeys->Caption = WideString(enabled_text);
    else
        checkHotkeys->Caption = WideString(disabled_text);
    if (checkExpertMode->Checked)
        checkExpertMode->Caption = WideString(enabled_text);
    else
        checkExpertMode->Caption = WideString(disabled_text);
}

void __fastcall TFormConfiguration::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}

void __fastcall TFormConfiguration::checkExpertModeClick(TObject *Sender)
{
    TElCheckBox *check_box = (TElCheckBox *)Sender;
    if (check_box->Checked)
    {
        FormWarningExpertMode->CenterOver(this);
        if (FormWarningExpertMode->ShowModal() == mrOk)
        {
            check_box->Checked = true;
            return;
        }
        check_box->Checked = false;
    }
}

// Sets the export folder to the pub folder, or back to the old export folder when they are the same.
void __fastcall TFormConfiguration::ElPopupButton1Click(TObject *Sender)
{
    if (!ImagePubFileOk->Visible)
    {
        FormWarning->ShowWarning(this, "Sync Path Error",
            "The pub read folder contains problems. Fix issues before syncing this path to export.");
        return;
    }
    if (editExportPath->Caption != editPubPath->Caption)
        old_export_path = editExportPath->Caption;
    else
    {
        if (old_export_path != "")
            editExportPath->Caption = old_export_path;
        return;
    }
    editExportPath->Caption = editPubPath->Caption;
    CheckPaths();
}

// Saves only when the gfx folder is good.
void __fastcall TFormConfiguration::buttonOkClick(TObject *Sender)
{
    if (String(buttonOk->Caption) == "CANNOT SAVE")
        return;
    if (FormAbout->Label5->Caption[1] != 'v')
        return;
    ModalResult = mrOk;
}

void __fastcall TFormConfiguration::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
