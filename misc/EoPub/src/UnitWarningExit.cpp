#include <vcl.h>
#pragma hdrstop

#include "UnitWarningExit.h"
#include "UnitMain.h"
#include <stdlib.h>

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormWarningExit *FormWarningExit;

__fastcall TFormWarningExit::TFormWarningExit(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

DEFINE_FORM_SHOW(TFormWarningExit)

void __fastcall TFormWarningExit::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk && ModalResult != mrAbort)
        ModalResult = mrCancel;
}

// Asks whether to save before exit, telling how long the editor was open.
void TFormWarningExit::ShowExitWarning(TFormMain *parent)
{
    int seconds = FormMain->timer_ticks / 2;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    minutes = minutes % 60;
    String time_text = "";
    if (hours > 0)
        time_text += IntToStr(hours) + " hours and ";
    time_text += IntToStr(minutes) + " minutes ";
    if (minutes >= 0)
    {
        Label4->Caption =
            "You spent " + time_text +
                "in the editor doing all kinds of good things. Excellent!   Would you like to save your progress?";
        Left = parent->Left + (parent->Width - Width) / 2;
        Top = parent->Top + (parent->Height - Height) / 2;
        Timer1->Enabled = false;
        home_left = Left;
        ShowModal();
    }
}

void __fastcall TFormWarningExit::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrAbort;
}

void __fastcall TFormWarningExit::buttonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

// Escape or End shakes the window.
void __fastcall TFormWarningExit::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
    {
        Timer1->Enabled = true;
        shake_ticks = 5;
    }
}

// Shakes the window side to side for a few ticks, then puts it back.
void __fastcall TFormWarningExit::Timer1Timer(TObject *Sender)
{
    if (shake_ticks > 0)
    {
        shake_ticks--;
        Left = random(4) + home_left - 2;
        if (shake_ticks <= 0)
        {
            Timer1->Enabled = false;
            Left = home_left;
        }
    }
}
