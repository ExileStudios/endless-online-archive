#include <vcl.h>
#pragma hdrstop

#include "UnitWarningCannotOpen.h"
#include "UnitMain.h"
#include "Classvalues.h"
#include "Itemvalues.h"
#include "Npcvalues.h"
#include "Skillvalues.h"

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma resource "*.dfm"

TFormWarningCannotOpen *FormWarningCannotOpen;

__fastcall TFormWarningCannotOpen::TFormWarningCannotOpen(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
}

// Dark theme colours when chosen; the focus goes to the hidden tab catcher.
DEFINE_FORM_SHOW(TFormWarningCannotOpen)

void __fastcall TFormWarningCannotOpen::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrOk;
}

// Closing the window counts as quitting the program.
void __fastcall TFormWarningCannotOpen::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (ModalResult != mrOk)
        ModalResult = mrAbort;
}

// Shows which pub file could not be opened, centred over the main window.
void TFormWarningCannotOpen::ShowCannotOpen(TFormMain *parent)
{
    if (FormMain->items_tab->file_unreadable)
        labelPubErrorFile->Caption = FormMain->items_tab->item_values->file_path;
    if (FormMain->npcs_tab->file_unreadable)
        labelPubErrorFile->Caption = FormMain->npcs_tab->npc_values->file_path;
    if (FormMain->skills_tab->file_unreadable)
        labelPubErrorFile->Caption = FormMain->skills_tab->skill_values->file_path;
    if (FormMain->classes_tab->file_unreadable)
        labelPubErrorFile->Caption = FormMain->classes_tab->class_values->part_file_path;
    Left = (parent->Width - Width) / 2 + parent->Left;
    Top = (parent->Height - Height) / 2 + parent->Top;
    ShowModal();
}

void __fastcall TFormWarningCannotOpen::labelPubErrorFileClick(TObject *Sender)
{
    String pub_path = FormMain->config->GetPubPath();
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", pub_path.c_str(), 0, SW_SHOWNORMAL);
}
