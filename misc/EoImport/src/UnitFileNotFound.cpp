#include <vcl.h>
#pragma hdrstop

#include "UnitFileNotFound.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TFormFileNotFound *FormFileNotFound;

__fastcall TFormFileNotFound::TFormFileNotFound(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

void __fastcall TFormFileNotFound::Panel3Click(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

// Tells the user which .ini files the picked folder should hold, centred on the main window.
void TFormFileNotFound::ShowMissing(TMainForm *main_form, String folder)
{
    String hint = "selected path should contain one of these files:\n\n";
    hint += " - drops.ini ( or npc_drops )\n";
    hint += " - chats.ini ( or npc_chat.ini )\n";
    hint += " - home.ini\n";
    hint += " - shops.ini\n";
    hint += " - skills.ini\n";
    hint += " - inn.ini ( or home.ini )";
    labelPath->Hint = hint;
    panelPath->Hint = hint;
    labelPath->Caption = folder;
    labelPath->Hint = String(" open read folder \n");
    labelPath->Hint += " " + folder;
    Left = main_form->Left + (main_form->Width - Width) / 2;
    Top = main_form->Top + (main_form->Height - Height) / 2;
    ShowModal();
}

void __fastcall TFormFileNotFound::labelPathClick(TObject *Sender)
{
    String folder = labelPath->Caption;
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
}
