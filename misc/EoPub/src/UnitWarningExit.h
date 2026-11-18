#ifndef UnitWarningExitH
#define UnitWarningExitH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Asks whether to save before the editor closes.
class TFormWarningExit : public TForm
{
__published:
    TLabel *Label3;
    TLabel *Label4;
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel2;
    TImage *Image1;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    TTimer *Timer1;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall Timer1Timer(TObject *Sender);
private:
public:
    int shake_ticks;
    int home_left;

    __fastcall TFormWarningExit(TComponent *owner);
    void ShowExitWarning(TFormMain *parent);
};

extern PACKAGE TFormWarningExit *FormWarningExit;

#endif
