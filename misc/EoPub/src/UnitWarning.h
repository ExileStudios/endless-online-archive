#ifndef UnitWarningH
#define UnitWarningH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// A warning with a title and a text.
class TFormWarning : public TForm
{
__published:
    TImage *Image2;
    TLabel *Label3;
    TLabel *Label4;
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel2;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormWarning(TComponent *owner);
    void ShowWarning(TForm *parent, String title, String text);
};

extern PACKAGE TFormWarning *FormWarning;

#endif
