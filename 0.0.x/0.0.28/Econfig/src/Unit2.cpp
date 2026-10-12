#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"

#pragma package(smart_init)

#pragma resource "*.dfm"
TForm2 *Form2;

__fastcall TForm2::TForm2(Classes::TComponent *Owner)
    : Forms::TForm(Owner)
{
}

void __fastcall TForm2::SpeedButton2Click(System::TObject *Sender)
{
    Close();
}
