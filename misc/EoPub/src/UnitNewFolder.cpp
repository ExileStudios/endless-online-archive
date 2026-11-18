#include <vcl.h>
#pragma hdrstop

#include "UnitNewFolder.h"
#include "UnitMain.h"
#include "UnitFolderSelect.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormNewFolder *FormNewFolder;

__fastcall TFormNewFolder::TFormNewFolder(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Shows the folder the new one goes in and centres the form over the folder picker.
void TFormNewFolder::PlaceOver(TFormFolderSelect *parent, String folder)
{
    Label3->Caption = folder;
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
}

void __fastcall TFormNewFolder::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Keeps only the letters and digits of the typed name as the new folder's name.
void __fastcall TFormNewFolder::buttonOkClick(TObject *Sender)
{
    folder_name = "";
    for (int i = 1; i <= editName->Text.Length(); i++)
    {
        if (editName->Text[i] >= '0' && editName->Text[i] <= '9')
            folder_name += (char)editName->Text[i];
        if (editName->Text[i] >= 'A' && editName->Text[i] <= 'Z')
            folder_name += (char)editName->Text[i];
        if (editName->Text[i] >= 'a' && editName->Text[i] <= 'z')
            folder_name += (char)editName->Text[i];
    }
    ModalResult = mrOk;
}

String TFormNewFolder::GetNewFolderPath()
{
    return Label3->Caption + "\\" + folder_name;
}

// Themes the form and offers NewPubFolder as the name.
void __fastcall TFormNewFolder::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    editName->Text = "NewPubFolder";
    editName->SetFocus();
}

// Escape takes the name as it is.
void __fastcall TFormNewFolder::editNameKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE)
        ModalResult = mrOk;
}

// Opens the folder the new one goes in, in Explorer.
void __fastcall TFormNewFolder::Image1Click(TObject *Sender)
{
    String folder = Label3->Caption;
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
}
