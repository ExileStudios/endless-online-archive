#include <vcl.h>
#pragma hdrstop

#include "UnitWarningClsCorruption.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormClassCorruption *FormClassCorruption;

__fastcall TFormClassCorruption::TFormClassCorruption(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

DEFINE_FORM_SHOW(TFormClassCorruption)

// Shows the class file corruption warning a little below the middle of the main window.
void TFormClassCorruption::ShowCorruption(TFormMain *parent)
{
    Caption = " Warning - corruption detect";
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
    Top = Top + 60;
    ShowModal();
}

void __fastcall TFormClassCorruption::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        ModalResult = mrCancel;
}

// Accepts adding the missing classes.
void __fastcall TFormClassCorruption::SpeedButton2Click(TObject *Sender)
{
    ModalResult = mrOk;
}

void __fastcall TFormClassCorruption::buttonCancelClick(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

void __fastcall TFormClassCorruption::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}
