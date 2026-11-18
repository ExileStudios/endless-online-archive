#include <vcl.h>
#pragma hdrstop

#include "UnitPathScan.h"
#include "UnitMain.h"
#include "UnitFolderSelect.h"
#include <dirent.h>

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormPathScan *FormPathScan;

__fastcall TFormPathScan::TFormPathScan(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Starts the search of the drive for the folder holding the file, centred over the folder picker.
void TFormPathScan::ShowScan(TFormFolderSelect *parent, char drive_letter, String file_name)
{
    Left = parent->Width / 2 + parent->Left - Width / 2;
    Top = parent->Height / 2 + parent->Top - Height / 2;
    target_file = file_name;
    drive = drive_letter;
    Caption = " Searching..";
    labelScanDrive->Caption = "Scanning for gfx folder on drive " + UpperCase(drive);
    ShowModal();
}

// DONE closes with the folder found; ABORT stops the search, and a second press cancels.
void __fastcall TFormPathScan::buttonOkClick(TObject *Sender)
{
    if (found)
    {
        ModalResult = mrOk;
        return;
    }
    if (!stopped)
    {
        aborted = true;
        stopped = true;
        return;
    }
    ModalResult = mrCancel;
}

void __fastcall TFormPathScan::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    buttonTabCatch->SetFocus();
    Timer1->Enabled = true;
}

// Searches the folder and its subfolders, four deep, for the file; stops at the first folder that has it.
void TFormPathScan::ScanFolder(String folder, unsigned char depth)
{
    dirent *entry;
    if (depth > 4)
        return;
    int directory = (int)opendir(folder.c_str());
    if (!directory)
        return;
    while ((entry = readdir((DIR *)directory)) != 0)
    {
        if (!entry || stopped)
            break;
        String entry_name = (char *)entry;
        if (entry_name == ".")
            continue;
        if (entry_name == "..")
            continue;
        // A name with a dot is taken as a file, anything else as a folder to enter.
        if (entry_name.Pos(".") > 0)
        {
            labelCurrentFolder->Caption = folder;
            labelCurrentFile->Caption = entry_name;
            if (entry_name == target_file)
            {
                Caption = "";
                labelScanStatus->Caption = "found";
                buttonOk->Caption = WideString("DONE");
                found = true;
                stopped = true;
                found_folder = folder;
                break;
            }
            update_ticks++;
            if (update_ticks > 12)
            {
                update_ticks = 0;
                buttonOk->Update();
                Update();
            }
            continue;
        }
        if (entry_name.Length() > 1)
            ScanFolder(folder + entry_name + "\\", depth + 1);
    }
    closedir((DIR *)directory);
}

// Runs the search from the drive's root, then tells whether it was found, aborted or not found.
void __fastcall TFormPathScan::Timer1Timer(TObject *Sender)
{
    Timer1->Enabled = false;
    labelScanStatus->Caption = "not found yet";
    buttonOk->Caption = WideString("ABORT");
    stopped = false;
    found = false;
    found_folder = "";
    aborted = false;
    Update();
    ScanFolder(String(drive) + ":\\", 0);
    stopped = true;
    if (!found)
    {
        if (aborted)
            buttonOk->Caption = WideString("ABORTED");
        if (!aborted)
            buttonOk->Caption = WideString("NOT FOUND");
    }
}

// Opens the folder being searched in Explorer.
void __fastcall TFormPathScan::labelCurrentFolderClick(TObject *Sender)
{
    String folder = labelCurrentFolder->Caption;
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
}
