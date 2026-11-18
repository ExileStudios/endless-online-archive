#ifndef UnitWarningClsCorruptionH
#define UnitWarningClsCorruptionH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Warns that items refer to classes that are missing, and offers to add them.
class TFormClassCorruption : public TForm
{
__published:
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TImage *Image2;
    TLabel *Label3;
    TLabel *Label4;
    TPanel *Panel1;
    TLabel *classErrTxt;
    TLabel *Label2;
    TLabel *Label5;
    TLabel *Label6;
    TSpeedButton *SpeedButton2;
    TShape *Shape1;
    TShape *Shape2;
    TShape *Shape3;
    TLabel *Label7;
    TLabel *Label8;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormClassCorruption(TComponent *owner);
    void ShowCorruption(TFormMain *parent);
};

extern PACKAGE TFormClassCorruption *FormClassCorruption;

#endif
