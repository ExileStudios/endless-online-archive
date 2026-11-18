#ifndef UnitWarningCannotOpenH
#define UnitWarningCannotOpenH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Tells which pub file could not be opened, and offers to try again or quit.
class TFormWarningCannotOpen : public TForm
{
__published:
    TImage *Image2;
    TLabel *Label3;
    TLabel *Label4;
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonTabCatch;
    TPanel *Panel5;
    TImage *imageBadPubItems;
    TLabel *labelPubErrorFile;
    TLabel *Label8;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall labelPubErrorFileClick(TObject *Sender);
private:
public:

    __fastcall TFormWarningCannotOpen(TComponent *owner);
    void ShowCannotOpen(TFormMain *parent);
};

extern PACKAGE TFormWarningCannotOpen *FormWarningCannotOpen;

#endif
