#include <vcl.h>
#pragma hdrstop

#include "UnitExportResultForm.h"
#include "UnitMain.h"
#include "UnitExportForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TFormExportResult *FormExportResult;

__fastcall TFormExportResult::TFormExportResult(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

void __fastcall TFormExportResult::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
}

// Shows whether the export worked, with the files written, at the lower right of the main window.
void TFormExportResult::ShowResult(String summary)
{
    String failure_note = "";
    if (summary.Length() > 2)
    {
        imageExportOk->Visible = true;
        ImageExportError->Visible = false;
        labelExportCaption->Caption = "Export Success";
        labelExportFIles->Caption = "locate files";
        summary.Delete(summary.Length() - 1, 2);
    }
    if (summary.Length() <= 2)
    {
        imageExportOk->Visible = false;
        ImageExportError->Visible = true;
        labelExportCaption->Caption = "Export Failed";
        labelExportFIles->Caption = "locate problem";
        summary = "Failed all write..";
    }
    else if (FormExport->export_error == 2)
    {
        imageExportOk->Visible = false;
        ImageExportError->Visible = true;
        labelExportCaption->Caption = "Export Failed";
        labelExportFIles->Caption = "locate problem";
        failure_note = ", [aborted]  Data sort error..";
    }
    else if (FormExport->export_error == 1)
    {
        imageExportOk->Visible = false;
        ImageExportError->Visible = true;
        labelExportCaption->Caption = "Export Failed";
        labelExportFIles->Caption = "locate problem";
        failure_note = " + File(s) write error(s)";
    }
    Label2->Caption = summary + failure_note;
    // The result pops up in the bottom right corner of the main window.
    Left = FormMain->Left + FormMain->Width - Width - 20;
    Top = FormMain->Top + FormMain->Height - Height - 20;
    Visible = true;
    Timer1->Enabled = true;
}

// Hides the result again after a while.
void __fastcall TFormExportResult::Timer1Timer(TObject *Sender)
{
    Visible = false;
    Timer1->Enabled = false;
}

void __fastcall TFormExportResult::labelExportFIlesClick(TObject *Sender)
{
    String export_path = FormMain->config->GetExportPath();
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", export_path.c_str(), 0, SW_SHOWNORMAL);
}
