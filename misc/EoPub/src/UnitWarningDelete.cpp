#include <vcl.h>
#pragma hdrstop

#include "UnitWarningDelete.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormDeleteWarning *FormDeleteWarning;

__fastcall TFormDeleteWarning::TFormDeleteWarning(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

DEFINE_FORM_SHOW(TFormDeleteWarning)

// Sets the title, text and button of the warning and places it over the middle of the grid.
void TFormDeleteWarning::PlaceWarning(TForm *parent, int grid_width, String title, String text, String button_text)
{
    int dialog_top;
    labelDeleteTitle->Caption = title;
    labelDeleteTxt->Caption = text;
    buttonOk->Caption = WideString(button_text);
    int dialog_left = parent->Left;
    dialog_top = parent->Top;
    dialog_left += (grid_width + 20) / 2;
    dialog_top += (parent->Height + 40) / 2;
    dialog_left -= Width / 2;
    dialog_top -= Height / 2;
    Left = dialog_left;
    Top = dialog_top;
}

// Enter confirms; Escape or End cancels.
void __fastcall TFormDeleteWarning::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
        ModalResult = mrOk;
    if (Key == VK_ESCAPE || Key == VK_END)
    {
        ModalResult = mrCancel;
        ((TCustomForm *)this)->Close();
    }
}

void __fastcall TFormDeleteWarning::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrAbort;
}

void __fastcall TFormDeleteWarning::buttonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

void __fastcall TFormDeleteWarning::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ModalResult = mrAbort;
}
