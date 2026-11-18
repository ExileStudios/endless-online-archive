#ifndef UnitWarningDeleteH
#define UnitWarningDeleteH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// Asks before a row is deleted, freed, destroyed or injected.
class TFormDeleteWarning : public TForm
{
__published:
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TLabel *labelDeleteTxt;
    TLabel *labelDeleteTitle;
    TImage *Image1;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:

    __fastcall TFormDeleteWarning(TComponent *owner);
    void PlaceWarning(TForm *parent, int grid_width, String title, String text, String button_text);
};

extern PACKAGE TFormDeleteWarning *FormDeleteWarning;

#endif
