#include <vcl.h>
#pragma hdrstop

#include "UnitWarningExpertMode.h"
#include "UnitMain.h"
#include "UnitConfiguration.h"

#pragma package(smart_init)
#pragma link "ElCheckCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormWarningExpertMode *FormWarningExpertMode;

__fastcall TFormWarningExpertMode::TFormWarningExpertMode(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Centres the warning over the configuration form.
void TFormWarningExpertMode::CenterOver(TFormConfiguration *parent)
{
    Left = parent->Width / 2 + parent->Left - Width / 2;
    Top = parent->Height / 2 + parent->Top - Height / 2;
}

// Themes the form and clears the expert check box, which takes the focus.
void __fastcall TFormWarningExpertMode::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Image3->Visible = false;
        Image4->Visible = true;
        Color = (TColor)0x525252;
    }
    checkboxIamExpert->Checked = false;
    checkboxIamExpert->SetFocus();
}

void __fastcall TFormWarningExpertMode::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

// Expert mode is only accepted with the box ticked.
void __fastcall TFormWarningExpertMode::buttonOkClick(TObject *Sender)
{
    if (checkboxIamExpert->Checked)
        ModalResult = mrOk;
}

void __fastcall TFormWarningExpertMode::checkboxIamExpertKeyDown(TObject *Sender, unsigned short &Key,
    TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}
