#ifndef UnitWarningOutOfBoundH
#define UnitWarningOutOfBoundH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Warns about problems found in the pub files, as one text or as a list.
class TFormWarningOutOfBound : public TForm
{
__published:
    TImage *Image2;
    TLabel *labelCaption;
    TLabel *labelInfo;
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel1;
    TMemo *Memo1;
    TLabel *Label1;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormWarningOutOfBound(TComponent *owner);
    void ShowOutOfBounds(TFormMain *parent, bool as_list, String text);
};

extern PACKAGE TFormWarningOutOfBound *FormWarningOutOfBound;

#endif
