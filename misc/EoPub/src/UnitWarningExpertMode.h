#ifndef UnitWarningExpertModeH
#define UnitWarningExpertModeH

#include <Classes.hpp>
#include <ElCheckCtl.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormConfiguration;

// Asks the user to confirm being an expert before expert mode is on.
class TFormWarningExpertMode : public TForm
{
__published:
    TImage *Image2;
    TLabel *Label3;
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel2;
    TImage *Image1;
    TPanel *Panel1;
    TElCheckBox *checkboxIamExpert;
    TLabel *Label1;
    TImage *Image3;
    TImage *Image4;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TLabel *Label2;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall checkboxIamExpertKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormWarningExpertMode(TComponent *owner);
    void CenterOver(TFormConfiguration *parent);
};

extern PACKAGE TFormWarningExpertMode *FormWarningExpertMode;

#endif
