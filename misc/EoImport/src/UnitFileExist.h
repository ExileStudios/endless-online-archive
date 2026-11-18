#ifndef UnitFileExistH
#define UnitFileExistH

#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TMainForm;

// Asks before a pub file that is already there is overwritten.
class TFormFileExist : public TForm
{
__published:
    TLabel *labelDeleteTxt;
    TLabel *labelDeleteTitle;
    TImage *Image1;
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel1;
    TPanel *Panel3;
    void __fastcall Panel1Click(TObject *Sender);
    void __fastcall Panel3Click(TObject *Sender);
private:
public:

    __fastcall TFormFileExist(TComponent *owner);
    void AskOverwrite(TMainForm *main_form, String file_name);
};

extern PACKAGE TFormFileExist *FormFileExist;

#endif
