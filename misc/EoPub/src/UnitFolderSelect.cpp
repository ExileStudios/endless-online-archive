#include <vcl.h>
#pragma hdrstop

#include "UnitFolderSelect.h"
#include "UnitMain.h"
#include "UnitWarning.h"
#include "UnitNewFolder.h"
#include "UnitPathScan.h"
#include <dir.h>
#include <io.h>

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElMenus"
#pragma resource "*.dfm"

TFormFolderSelect *FormFolderSelect;

__fastcall TFormFolderSelect::TFormFolderSelect(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Sets up the picker for a gfx, pub, export or backup folder, starting at the folder, sized over the owner.
void TFormFolderSelect::Open(TForm *owner, String caption, String folder, unsigned char kind, char scan)
{
    path_pasted = false;
    folder = FullPath(folder);
    String start_folder = WithoutSlash(folder);
    if (!FormMain->config_check->PathExists(start_folder))
    {
        start_folder = FormMain->config->GetAppPath();
        start_folder = WithoutSlash(start_folder);
    }
    old_folder = start_folder;
    last_drive = DriveComboBox1->Drive;
    imageGotoOldPath->Hint = "goto current " + caption + " (old path)";
    if (!kind)
    {
        DirectoryListBox1->Directory = start_folder;
        DriveComboBox1->Drive = DirectoryListBox1->Drive;
    }
    // Kind 1 shows the parent folder, with the start folder selected in it.
    if (kind == 1)
    {
        DirectoryListBox1->Directory = start_folder;
        try
        {
            if (DirectoryListBox1->ItemIndex > 0)
            {
                DirectoryListBox1->ItemIndex = DirectoryListBox1->ItemIndex - 1;
                DirectoryListBox1->OpenCurrent();
                String folder_name = ParentFolder(start_folder);
                for (int i = 0; i < DirectoryListBox1->Items->Count; i++)
                {
                    if (DirectoryListBox1->Items->Strings[i] == folder_name)
                    {
                        DirectoryListBox1->ItemIndex = i;
                        break;
                    }
                }
            }
        }
        catch (...)
        {
            DirectoryListBox1->Directory = start_folder;
        }
        DriveComboBox1->Drive = DirectoryListBox1->Drive;
    }
    labelSelectCaption->Caption = caption;
    editFolderPath->Text = start_folder;
    folder_kind = scan;
    if (folder_kind == 0 || folder_kind == 1)
    {
        DriveComboBox1->PopupMenu = (TPopupMenu *)popupAutoScan;
        ScanDrive();
    }
    else
        DriveComboBox1->PopupMenu = 0;
    LayoutButtons();
    Width = owner->Width - 20;
    Height = owner->Height - 20;
    Left = (owner->Width - Width) / 2 + owner->Left;
    Top = (owner->Height - Height) / 2 + owner->Top;
    IsPubFolder(editFolderPath->Text);
}

// Dark theme colours for the form, drive box and folder list when chosen; starts the follow timer.
void __fastcall TFormFolderSelect::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        imageFolderArrow->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap;
        Color = (TColor)0x525252;
        DriveComboBox1->Color = (TColor)0x525252;
        DriveComboBox1->Font->Color = (TColor)0xAAAAAA;
        DirectoryListBox1->Color = (TColor)0x525252;
        DirectoryListBox1->Font->Color = (TColor)0xDDDDDD;
        DriveComboBox1->Font->Color = (TColor)0xDDDDDD;
    }
    DirectoryListBox1->SetFocus();
    Timer1->Enabled = true;
}

// Unless a folder was chosen, the list goes back to the program's folder.
void __fastcall TFormFolderSelect::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        DirectoryListBox1->Directory = FormMain->config->GetAppPath();
    Timer1->Enabled = false;
}

// Takes the typed path as the chosen folder.
void __fastcall TFormFolderSelect::SpeedButtonCancelClick()
{
    chosen_folder = editFolderPath->Text;
    DirectoryListBox1->Directory = FormMain->config->GetAppPath();
    ModalResult = mrOk;
}

// Checks the folder for the files its kind needs and shows the result; the hint lists what is missing.
char TFormFolderSelect::IsPubFolder(String folder)
{
    String path = WithSlash(folder);
    char found = false;
    // The error icon's hint counts the missing files and lists each one.
    if (!folder_kind)
        found = FormMain->config_check->CheckGfxPath(path);
    if (folder_kind == 1)
        found = FormMain->config_check->CheckPubPath(path);
    if (folder_kind == 2)
        found = FormMain->config_check->CheckExportPath(path);
    if (folder_kind == 3)
        found = FormMain->config_check->CheckBackupPath(path);
    if (!folder_kind)
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
        imageFileError->Hint = IntToStr(missing_gfx) + gfx_problems;
    }
    else if (folder_kind == 1)
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
        imageFileError->Hint = IntToStr(missing_pubs) + pub_problems;
    }
    else
        imageFileError->Hint = String("Folder might not exist");
    if (found)
    {
        ImageFileOk->Visible = true;
        imageFileError->Visible = false;
    }
    else
    {
        ImageFileOk->Visible = false;
        imageFileError->Visible = true;
    }
    return found;
}

void __fastcall TFormFolderSelect::DirectoryListBox1Change(TObject *Sender)
{
    OpenSelectedFolder();
}

void TFormFolderSelect::OpenSelectedFolder()
{
    String path = DirectoryListBox1->Directory;
    if (DirectoryListBox1->ItemIndex > 0)
    {
        String selected_name = DirectoryListBox1->Items->Strings[DirectoryListBox1->ItemIndex];
        DirectoryListBox1->ItemIndex;
        int folder_count = DirectoryListBox1->Items->Count;
        String current_name = ParentFolder(path);
        if (current_name != selected_name)
            path = WithoutSlash(path) + "\\" + selected_name;
    }
    if (editFolderPath->Text != path)
    {
        editFolderPath->Text = path;
        IsPubFolder(editFolderPath->Text);
    }
}

// Shows the new drive's folders and keeps it as the last good drive.
void __fastcall TFormFolderSelect::DriveComboBox1Change(TObject *Sender)
{
    ScanDrive();
    editFolderPath->Text = DirectoryListBox1->Directory;
    last_drive = DriveComboBox1->Drive;
}

// Shows the drive's folders and names the scan of it in the popup; a drive that fails goes back to the last one.
void TFormFolderSelect::ScanDrive()
{
    try
    {
        DirectoryListBox1->Drive = DriveComboBox1->Drive;
        String target = "gfx";
        if (folder_kind == 1)
            target = "pub";
        String scan_caption = "Scan drive " + String(DriveComboBox1->Drive) + " for " + target + " folder";
        popupAutoScan->Items->Items[0]->Caption = WideString(scan_caption);
    }
    catch (...)
    {
        // A drive that cannot be read puts the last good drive back before the warning.
        DriveComboBox1->Drive = last_drive;
        FormWarning->ShowWarning(this, "Drive Error", "Cannot access selected drive.");
    }
}

String TFormFolderSelect::ParentFolder(String path)
{
    if (path.Length() < 1)
        return "";
    if (path[path.Length()] == '\\')
        path.Delete(path.Length(), 1);
    for (int i = path.Length(); i > 0; i--)
    {
        if (path[i] == '\\')
            return path.SubString(i + 1, path.Length() - i);
    }
    return path;
}

// The path made absolute when it starts with a dot.
String TFormFolderSelect::FullPath(String path)
{
    if (path.Length() > 0 && path[1] == '.')
    {
        path.Delete(1, 1);
        path = FormMain->config->GetAppPath() + path;
    }
    return path;
}

String TFormFolderSelect::WithoutSlash(String path)
{
    if (!path.Length())
        return "";
    if (path[path.Length()] == '\\' || path[path.Length()] == '/')
        path.Delete(path.Length(), 1);
    return path;
}

// The path with a backslash at its end.
String TFormFolderSelect::WithSlash(String path)
{
    if (!path.Length())
        return "";
    if (path[path.Length()] != '\\' && path[path.Length()] != '/')
        path = path + '\\';
    return path;
}

String TFormFolderSelect::SelectedFolder()
{
    return WithSlash(chosen_folder);
}

// Sizes the bottom panel to the folder list and centres the two buttons.
void TFormFolderSelect::LayoutButtons()
{
    Panel3->Width = DirectoryListBox1->Width;
    buttonCancel->Left = Width / 2 - 10 - buttonCancel->Width;
    buttonOk->Left = Width / 2 + 10;
}

void __fastcall TFormFolderSelect::FormResize(TObject *Sender)
{
    LayoutButtons();
}

void __fastcall TFormFolderSelect::buttonCancelClick(TObject *Sender)
{
    DirectoryListBox1->Directory = FormMain->config->GetAppPath();
    ModalResult = mrCancel;
}

void __fastcall TFormFolderSelect::buttonOkClick(TObject *Sender)
{
    chosen_folder = editFolderPath->Text;
    DirectoryListBox1->Directory = FormMain->config->GetAppPath();
    ModalResult = mrOk;
}

// Goes to a pasted folder when it exists, and keeps the path in step with the list.
void __fastcall TFormFolderSelect::Timer1Timer(TObject *Sender)
{
    if (path_pasted)
    {
        path_pasted = false;
        String path = WithSlash(editFolderPath->Text);
        if (!access(path.c_str(), 0))
        {
            DirectoryListBox1->Directory = editFolderPath->Text;
            DirectoryListBox1->Update();
            IsPubFolder(editFolderPath->Text);
        }
    }
    if (!Visible)
        return;
    OpenSelectedFolder();
}

// Makes a new folder, named in a small form, inside the current one.
void __fastcall TFormFolderSelect::ElPopupButton1Click(TObject *Sender)
{
    FormNewFolder->PlaceOver(this, editFolderPath->Text);
    if (FormNewFolder->ShowModal() == mrOk)
    {
        try
        {
            mkdir(FormNewFolder->GetNewFolderPath().c_str());
            DirectoryListBox1->Update();
        }
        catch (...)
        {
        }
    }
}

// Goes back to the folder the picker started at.
void __fastcall TFormFolderSelect::imageGotoOldPathClick(TObject *Sender)
{
    DirectoryListBox1->Directory = old_folder;
}

void __fastcall TFormFolderSelect::imageGotoHomeClick(TObject *Sender)
{
    DirectoryListBox1->Directory = FormMain->config->GetAppPath();
}

// Searches the drive for the gfx or pub folder and goes to it when found.
void __fastcall TFormFolderSelect::ElMenuItem1Click(TObject *Sender)
{
    if (!folder_kind)
        FormPathScan->ShowScan(this, DriveComboBox1->Drive, "gfx025.egf");
    if (folder_kind == 1)
        FormPathScan->ShowScan(this, DriveComboBox1->Drive, "dtn001.enf");
    if (FormPathScan->ModalResult == mrOk && FormPathScan->found)
        DirectoryListBox1->Directory = FormPathScan->found_folder;
}

// Notes a paste with Ctrl+V, so the timer goes to the pasted folder.
void __fastcall TFormFolderSelect::editFolderPathKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == 'V')
        path_pasted = true;
}
