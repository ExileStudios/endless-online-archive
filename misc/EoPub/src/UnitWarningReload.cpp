#include <vcl.h>
#pragma hdrstop

#include "UnitWarningReload.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormReloadWarning *FormReloadWarning;

__fastcall TFormReloadWarning::TFormReloadWarning(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

void __fastcall TFormReloadWarning::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    Label6->Caption = FormMain->config->GetPubPath();
    buttonTabCatch->SetFocus();
}

// Asks whether to reload the pubs now or next restart, after a pub path change or the reload hotkey.
void TFormReloadWarning::ShowReloadWarning(TFormMain *parent, unsigned char by_hotkey)
{
    Caption = " Warning - please read";
    if (!by_hotkey)
        labelReloadTxt->Caption =
            "You made changes to the pub read path int the configuration, would you like to reload the data now or "
                "next restart ?";
    if (by_hotkey >= 1)
        labelReloadTxt->Caption =
            "The reload hotkey Ctrl + R was pressed. Would you like to reload all data now or next restart ?";
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
    ShowModal();
}

void __fastcall TFormReloadWarning::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
    {
        ModalResult = mrCancel;
        ((TCustomForm *)this)->Close();
    }
}

void __fastcall TFormReloadWarning::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        ModalResult = mrCancel;
}

// Opens the shown pub folder in Explorer when it exists.
void __fastcall TFormReloadWarning::Label6Click(TObject *Sender)
{
    if (FormMain->config_check->PathExists(((TLabel *)Sender)->Caption))
    {
        String folder = ((TLabel *)Sender)->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
    }
}

// Closes the warning without reloading.
void __fastcall TFormReloadWarning::Label1Click(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

void __fastcall TFormReloadWarning::buttonCancelClick(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

void __fastcall TFormReloadWarning::buttonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

void __fastcall TFormReloadWarning::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}
