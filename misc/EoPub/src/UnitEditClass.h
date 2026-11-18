#ifndef UnitEditClassH
#define UnitEditClassH

#include <Classes.hpp>
#include <ElCombos.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTree.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// The class editor: creates, edits or duplicates one class of the class file.
class TFormEditClass : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TPanel *Panel3;
    TLabel *Label1;
    TPanel *Panel1;
    TEdit *editName;
    TElComboBox *comboEditItemType;
    TLabel *Label3;
    TElComboBox *comboEditItemParent;
    TLabel *Label2;
    TElSpinEdit *spinEditStr;
    TLabel *Label4;
    TElSpinEdit *spinEditInt;
    TLabel *Label5;
    TElSpinEdit *spinEditWis;
    TLabel *Label6;
    TElSpinEdit *spinEditAgi;
    TLabel *Label7;
    TElSpinEdit *spinEditCon;
    TLabel *Label8;
    TElSpinEdit *spinEditCha;
    TLabel *Label10;
    TLabel *itemNameOriginalID;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TImage *imageWarnDupe;
    void __fastcall comboEditItemTypeEnter(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall spinEditStrEnter(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall comboEditItemTypeExit(TObject *Sender);
    void __fastcall comboEditItemTypeKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall comboEditItemTypeKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditStrExit(TObject *Sender);
    void __fastcall comboEditItemParentKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    TCustomElTree *names_grid;
    TCustomElTree *pub_grid;
    DataGridCol *name_column;
    bool combos_filled;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormEditClass(TComponent *owner);
    void Open(int operation, int index);
    void ShowOperation(int operation);
    void LoadFields(int index, unsigned char operation);
    void FillCombos();
    void FillComboFromColumn(TElComboBox *combo, String column_name);
    void FillParentCombo();
    void FindNewId();
    void FindDupeId();
    String ResultText();
    int NumberOrZero(String text);
    String HideZeroText(String text);
    void CheckDuplicateName();
    void TypeChosen();
};

extern PACKAGE TFormEditClass *FormEditClass;

#endif
