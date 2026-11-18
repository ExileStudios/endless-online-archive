#include <vcl.h>
#pragma hdrstop

#include "UnitWarningOutOfBound.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormWarningOutOfBound *FormWarningOutOfBound;

__fastcall TFormWarningOutOfBound::TFormWarningOutOfBound(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Dark theme colours for the form and its memo when chosen; the focus goes to the hidden tab catcher.
void __fastcall TFormWarningOutOfBound::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Memo1->Color = (TColor)0x444343;
        Color = (TColor)0x525252;
    }
    buttonTabCatch->SetFocus();
}

DEFINE_FORM_FORM_KEY_DOWN(TFormWarningOutOfBound)

void TFormWarningOutOfBound::ShowOutOfBounds(TFormMain *parent, bool as_list, String text)
{
    Caption = " Warning - please read";
    if (!as_list)
    {
        Label1->Caption = text;
        Label1->Visible = true;
        Memo1->Visible = false;
    }
    else
    {
        Memo1->Lines->Clear();
        Label1->Visible = false;
        Memo1->Visible = true;
        int position = 0;
        while (text.Length() > 0)
        {
            position++;
            if (text[position] == '\n' || position == text.Length())
            {
                String line = text.SubString(1, position - 1);
                Memo1->Lines->Add("- " + line);
                text.Delete(1, position);
                position = 1;
            }
            if (position > 250)
                break;
        }
    }
    Left = parent->Left + (parent->Width - Width) / 2;
    Top = parent->Top + (parent->Height - Height) / 2;
    ShowModal();
}

void __fastcall TFormWarningOutOfBound::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrOk;
}

void __fastcall TFormWarningOutOfBound::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ModalResult = mrOk;
}
