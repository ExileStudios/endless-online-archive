#ifndef UnitWarningReloadH
#define UnitWarningReloadH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Asks whether to read the pubs again now or at the next start.
class TFormReloadWarning : public TForm
{
__published:
    TLabel *Label3;
    TLabel *labelReloadTxt;
    TImage *Image2;
    TLabel *Label1;
    TPanel *Panel4;
    TLabel *problemText;
    TLabel *Label2;
    TPanel *Panel1;
    TLabel *Label5;
    TLabel *Label6;
    TPanel *Panel2;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Label6Click(TObject *Sender);
    void __fastcall Label1Click(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormReloadWarning(TComponent *owner);
    void ShowReloadWarning(TFormMain *parent, unsigned char by_hotkey);
};

extern PACKAGE TFormReloadWarning *FormReloadWarning;

#endif
