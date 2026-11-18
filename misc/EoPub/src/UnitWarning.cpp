#include <vcl.h>
#pragma hdrstop

#include "UnitWarning.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormWarning *FormWarning;

__fastcall TFormWarning::TFormWarning(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

DEFINE_FORM_SHOW(TFormWarning)

// Shows the warning with its title and text, centred over the form.
void TFormWarning::ShowWarning(TForm *parent, String title, String text)
{
    Caption = " Warning - please read";
    Label3->Caption = title;
    Label4->Caption = text;
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
    ShowModal();
}

DEFINE_FORM_FORM_KEY_DOWN(TFormWarning)

void __fastcall TFormWarning::buttonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

// Escape or End closes the warning.
void __fastcall TFormWarning::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ModalResult = mrOk;
}
