#ifndef UnitFileNotFoundH
#define UnitFileNotFoundH

#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TMainForm;

// Tells the user that a picked folder has none of the .ini files.
class TFormFileNotFound : public TForm
{
__published:
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel3;
    TLabel *labelDeleteTxt;
    TLabel *labelDeleteTitle;
    TImage *Image1;
    TPanel *panelPath;
    TLabel *labelPath;
    TLabel *Label3;
    void __fastcall Panel3Click(TObject *Sender);
    void __fastcall labelPathClick(TObject *Sender);
private:
public:

    __fastcall TFormFileNotFound(TComponent *owner);
    void ShowMissing(TMainForm *main_form, String folder);
};

extern PACKAGE TFormFileNotFound *FormFileNotFound;

#endif
