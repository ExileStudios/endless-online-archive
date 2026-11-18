#include <vcl.h>
#pragma hdrstop

#include "UnitFileExist.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TFormFileExist *FormFileExist;

__fastcall TFormFileExist::TFormFileExist(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
}

void __fastcall TFormFileExist::Panel1Click(TObject *Sender)
{
    ModalResult = mrAbort;
}

// Asks whether to overwrite the pub file, centred on the main window.
void TFormFileExist::AskOverwrite(TMainForm *main_form, String file_name)
{
    labelDeleteTxt->Caption = "File " + file_name + " already exist, overwrite this pub file?";
    Left = (main_form->Width - Width) / 2 + main_form->Left;
    Top = (main_form->Height - Height) / 2 + main_form->Top;
    ShowModal();
}

void __fastcall TFormFileExist::Panel3Click(TObject *Sender)
{
    ModalResult = mrOk;
}
