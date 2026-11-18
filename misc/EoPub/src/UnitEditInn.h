#ifndef UnitEditInnH
#define UnitEditInnH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTree.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// The inn editor: creates, edits or duplicates one inn of the inn file.
class TFormEditInn : public TForm
{
__published:
    TLabel *Label1;
    TLabel *Label4;
    TLabel *Label2;
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *Panel1;
    TImage *imageWarnDupe;
    TEdit *editName;
    TElSpinEdit *spinSleepMap;
    TElSpinEdit *spinSleepX;
    TLabel *Label3;
    TElSpinEdit *spinSleepY;
    TLabel *Label5;
    TLabel *Label6;
    TElSpinEdit *spinHomeMap;
    TElSpinEdit *spinHomeX;
    TLabel *Label7;
    TElSpinEdit *spinHomeY;
    TLabel *Label8;
    TLabel *Label9;
    TElSpinEdit *spinHomeHiMap;
    TElSpinEdit *spinHomeHiX;
    TLabel *Label10;
    TElSpinEdit *spinHomeHiY;
    TLabel *Label11;
    TElSpinEdit *spinHomeHi;
    TLabel *Label12;
    TElSpinEdit *spinCost;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall spinSleepMapEnter(TObject *Sender);
    void __fastcall spinSleepMapExit(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonOkClick(TObject *Sender);
private:
public:
    TCustomElTree *names_grid;
    TCustomElTree *pub_grid;
    DataGridCol *name_column;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    String tag;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormEditInn(TComponent *owner);
    void Open(int operation, int index);
    void ShowOperation(int operation);
    void LoadFields(int index, unsigned char operation);
    void FindNewId();
    void FindDupeId();
    int NumberOrZero(String text);
    String HideZeroText(String text);
    String ResultText();
    void CheckDuplicateName();
};

extern PACKAGE TFormEditInn *FormEditInn;

#endif
