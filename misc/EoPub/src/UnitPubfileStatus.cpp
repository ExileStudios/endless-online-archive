#include <vcl.h>
#pragma hdrstop

#include "UnitPubfileStatus.h"
#include "UnitMain.h"

#pragma package(smart_init)
#pragma link "ElPgCtl"
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormPubstatus *FormPubstatus;

__fastcall TFormPubstatus::TFormPubstatus(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Shows the health summary of the pub files of one kind; files that disagree count as bad truncation.
void TFormPubstatus::Open(TFormMain *main, String caption)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
        imagePub->Picture->Bitmap = FormMain->imageChangeItemView->Picture->Bitmap;
    }
    Left = main->Left + (main->Width - 420 - Width) / 2;
    Top = main->Top + (main->Height - Height) / 2 + 50;
    Label4->Hint = caption + " pub file(s) health summary";
    labelPubTitle->Hint = Label4->Hint;
    labelPubTitle->Caption = caption;
    ElTabSheet1->Caption = WideString("File(s) : " + caption);
    Caption = " File-health: " + caption;
    String first_entry = "";
    for (int i = 0; i < listPubfiles->Items->Count; i++)
    {
        // Files whose checksums differ come from different exports.
        if (first_entry == "")
            first_entry = listPubfiles->Items->Item[i]->SubItems->Strings[0];
        else if (first_entry != listPubfiles->Items->Item[i]->SubItems->Strings[0])
            SetHealth(3);
    }
    ElPageControl1->ActivePageIndex = 0;
    ShowModal();
}

void TFormPubstatus::ResetHealth()
{
    health_level = 0;
    labelPubHealth->Caption = "Good Health";
    imageBadHealth->Visible = false;
    imageGoodHealth->Visible = true;
}

void TFormPubstatus::SetHealth(unsigned char level)
{
    if (level < (unsigned char)health_level)
        return;
    health_level = level;
    if (!level)
        labelPubHealth->Caption = "Good Health";
    if (level == 1)
        labelPubHealth->Caption = "Acceptable";
    if (level == 2)
    {
        labelPubHealth->Caption = "Data Loss";
        imageBadHealth->Visible = true;
        imageGoodHealth->Visible = false;
    }
    if (level == 3)
    {
        labelPubHealth->Caption = "Bad Truncation";
        imageBadHealth->Visible = true;
        imageGoodHealth->Visible = false;
    }
    if (level == 4)
    {
        labelPubHealth->Caption = "Corrupt";
        imageBadHealth->Visible = true;
        imageGoodHealth->Visible = false;
    }
}

void __fastcall TFormPubstatus::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}

void TFormPubstatus::SetCorruption(unsigned char good, String text)
{
    imageCorruptGood->Visible = false;
    imageCorruptBad->Visible = false;
    labelCorrupt->Hint = String("");
    if (!good)
    {
        imageCorruptBad->Visible = true;
        labelCorrupt->Caption = text;
    }
    if (good == 1)
    {
        imageCorruptGood->Visible = true;
        labelCorrupt->Caption = text;
    }
}

void TFormPubstatus::SetCorruptionHint(String hint)
{
    labelCorrupt->Hint = hint;
    imageCorruptBad->Hint = hint;
}

void TFormPubstatus::SetDataLoss(unsigned char good, String text)
{
    imageDataLossGood->Visible = false;
    imageDataLossBad->Visible = false;
    labelDataLoss->Hint = String("");
    if (!good)
    {
        imageDataLossBad->Visible = true;
        labelDataLoss->Caption = text;
    }
    if (good == 1)
    {
        imageDataLossGood->Visible = true;
        labelDataLoss->Caption = text;
    }
}

void TFormPubstatus::SetDataLossHint(String hint)
{
    labelDataLoss->Hint = hint;
    imageDataLossBad->Hint = hint;
}

void TFormPubstatus::SetTruncation(unsigned char good, String text)
{
    imageTruncationGood->Visible = false;
    imageTruncationBad->Visible = false;
    labelTruncation->Hint = String("");
    if (!good)
    {
        imageTruncationBad->Visible = true;
        labelTruncation->Caption = text;
    }
    if (good == 1)
    {
        imageTruncationGood->Visible = true;
        labelTruncation->Caption = text;
    }
}

void TFormPubstatus::SetTruncationHint(String hint)
{
    labelTruncation->Hint = hint;
    imageTruncationBad->Hint = hint;
}

void TFormPubstatus::SetOutOfBounds(unsigned char good, String text)
{
    imageOOGood->Visible = false;
    imageOOBbad->Visible = false;
    labelOOB->Hint = String("");
    if (!good)
    {
        imageOOBbad->Visible = true;
        labelOOB->Caption = text;
    }
    if (good == 1)
    {
        imageOOGood->Visible = true;
        labelOOB->Caption = text;
    }
}

void TFormPubstatus::SetOutOfBoundsHint(String hint)
{
    labelOOB->Hint = hint;
    imageOOBbad->Hint = hint;
}

void __fastcall TFormPubstatus::buttonOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}

void __fastcall TFormPubstatus::FormShow(TObject *Sender)
{
    buttonTabCatch->SetFocus();
}

void __fastcall TFormPubstatus::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ModalResult = mrOk;
}
