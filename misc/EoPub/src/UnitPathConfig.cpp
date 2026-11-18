#include <vcl.h>
#pragma hdrstop

#include "UnitPathConfig.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitFolderSelect.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormPathConfig *FormPathConfig;

__fastcall TFormPathConfig::TFormPathConfig(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Closing without saving counts as cancel, which ends the program.
void __fastcall TFormPathConfig::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        ModalResult = mrCancel;
}

// Shows the paths, then offers the game's own gfx and pub folders when it finds them.
void __fastcall TFormPathConfig::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    editGfxPath->Caption = FormMain->config->GetGfxPath();
    editPubPath->Caption = FormMain->config->GetPubPath();
    editExportPath->Caption = FormMain->config->GetExportPath();
    editGfxPath->Hint = editGfxPath->Caption + "\nclick to open path";
    editPubPath->Hint = editPubPath->Caption + "\nclick to open path";
    editExportPath->Hint = editExportPath->Caption + "\nclick to open path";
    buttonTabCatch->SetFocus();
    CheckPaths();
    String client_gfx_path = "";
    try
    {
        // The game's install folder in the registry leads to its gfx folder.
        TRegistry *registry = new TRegistry();
        registry->RootKey = HKEY_CURRENT_USER;
        registry->OpenKey("\\Software\\EndlessOnline", true);
        client_gfx_path = registry->ReadString("Directory");
        registry->CloseKey();
        if (client_gfx_path.Length() > 0)
        {
            if (client_gfx_path[client_gfx_path.Length()] != '\\')
                client_gfx_path += '\\';
            client_gfx_path += "gfx\\";
        }
    }
    catch (...)
    {
    }
    bool client_found = false;
    if (client_gfx_path.Length() > 0 && FormMain->config_check->CheckGfxPath(client_gfx_path))
    {
        editGfxPath->Caption = client_gfx_path;
        client_found = true;
    }
    if (FormMain->config_check->CheckPubPath(".\\data\\pub\\"))
    {
        editPubPath->Caption = ".\\data\\pub\\";
        editExportPath->Caption = ".\\data\\pub\\";
        client_found = true;
    }
    if (client_found)
        CheckPaths();
}

// Checks the three folders and shows which files are missing; the gfx folder must be found to save.
void TFormPathConfig::CheckPaths()
{
    problemText->Caption = "Settings will be saved to " + FormMain->config->GetExeName() + ".ini";
    buttonOk->Caption = WideString("SAVE");
    if (FormMain->config_check->CheckPubPath(editPubPath->Caption))
    {
        ImagePubFileOk->Visible = true;
        imagePubFileError->Visible = false;
        editPubPath->Cursor = crDrag;
    }
    else
    {
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
        problemText->Caption = "Editor will create new pub list(s) because no pub file(s) found.";
        buttonOk->Caption = WideString("SAVE BUT CANNOT READ PUB(S)");
        editPubPath->Cursor = crHandPoint;
        ImagePubFileOk->Visible = false;
        imagePubFileError->Visible = true;
    }
    // Without the gfx files nothing can be saved; missing pubs are only made new.
    if (FormMain->config_check->CheckGfxPath(editGfxPath->Caption))
    {
        pubPathProblemCaption->Caption = "Found Endless/Gfx folder, please check the following setup.";
        pubPathInforCaption->Caption = "Please [save] the location or select another folder for a more advanced setup.";
        ImageGfxFileOk->Visible = true;
        imageGfxFileError->Visible = false;
        editGfxPath->Cursor = crDrag;
    }
    else
    {
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
        pubPathProblemCaption->Caption = "Pub Editor cannot read Endless gfx files.";
        pubPathInforCaption->Caption = "Fix this by entering the path to your Endless / gfx folder below.";
        problemText->Caption = "Must enter the gfx folder to continue.";
        buttonOk->Caption = WideString("CANNOT SAVE");
        editGfxPath->Cursor = crHandPoint;
        ImageGfxFileOk->Visible = false;
        imageGfxFileError->Visible = true;
    }
    if (FormMain->config_check->CheckExportPath(editExportPath->Caption))
    {
        ImagePubExportOk->Visible = true;
        ImagePubExportError->Visible = false;
        editExportPath->Cursor = crDrag;
    }
    else
    {
        ImagePubExportOk->Visible = false;
        ImagePubExportError->Visible = true;
        editExportPath->Cursor = crHandPoint;
    }
}

// Picks the gfx folder.
void __fastcall TFormPathConfig::SpeedButton3Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Read gfx folder", FormMain->config->GetAppPath(), 0, false);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editGfxPath->Caption = FormFolderSelect->SelectedFolder();
        editGfxPath->Hint = editGfxPath->Caption + "\nclick to open path";
        CheckPaths();
    }
}

void __fastcall TFormPathConfig::SpeedButton4Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Read pub folder", FormMain->config->GetAppPath(), 0, true);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editPubPath->Caption = FormFolderSelect->SelectedFolder();
        editPubPath->Hint = editPubPath->Caption + "\nclick to open path";
        CheckPaths();
    }
}

// Picks the folder pubs are exported to.
void __fastcall TFormPathConfig::SpeedButton5Click(TObject *Sender)
{
    FormFolderSelect->Open(this, "Export pub folder", FormMain->config->GetAppPath(), 0, 2);
    if (FormFolderSelect->ShowModal() == mrOk)
    {
        editExportPath->Caption = FormFolderSelect->SelectedFolder();
        editExportPath->Hint = editExportPath->Caption + "\nclick to open path";
        CheckPaths();
    }
}

// Opens the pub folder in Explorer when it is good; otherwise picks another.
void __fastcall TFormPathConfig::editPubPathClick(TObject *Sender)
{
    if (ImagePubFileOk->Visible)
    {
        String path = editPubPath->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton4Click((TObject *)SpeedButton4);
}

void __fastcall TFormPathConfig::editExportPathClick(TObject *Sender)
{
    if (ImagePubExportOk->Visible)
    {
        String path = editExportPath->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton5Click((TObject *)SpeedButton5);
}

void __fastcall TFormPathConfig::editGfxPathClick(TObject *Sender)
{
    if (ImageGfxFileOk->Visible)
    {
        String path = editGfxPath->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
    else
        SpeedButton3Click((TObject *)SpeedButton3);
}

void __fastcall TFormPathConfig::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Without a gfx folder the settings cannot be saved.
void __fastcall TFormPathConfig::buttonOkClick(TObject *Sender)
{
    if (String(buttonOk->Caption) == "CANNOT SAVE")
    {
        FormWarning->ShowWarning(this, "Gfx Missing",
            "Please enter the folder of your Endless/Gfx files. Pub Editor cannot run without Graphics Location");
        return;
    }
    ModalResult = mrOk;
}
