#ifndef UnitEditTrainerH
#define UnitEditTrainerH

#include <Classes.hpp>
#include <ElCombos.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// The trainer editor: creates, edits or duplicates one trainer of the trainer file.
class TFormEditTrainer : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TLabel *Label1;
    TPanel *Panel1;
    TImage *imageWarnDupe;
    TEdit *editName;
    TLabel *Label4;
    TElSpinEdit *spinEditMin;
    TLabel *Label2;
    TElSpinEdit *spinEditMax;
    TElComboBox *comboEditClass;
    TLabel *Label3;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditMinEnter(TObject *Sender);
    void __fastcall comboEditClassEnter(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall spinEditMinExit(TObject *Sender);
    void __fastcall comboEditClassKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonOkClick(TObject *Sender);
private:
public:
    TCustomElTree *names_grid;
    TCustomElTree *pub_grid;
    DataGridCol *name_column;
    bool combos_filled;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    String tag;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormEditTrainer(TComponent *owner);
    void Open(int operation, int index);
    void ShowOperation(int operation);
    void LoadFields(int index, unsigned char operation);
    void FillCombos();
    void FillComboFromGrid(TElComboBox *combo, TElTreeStringGrid *grid, String column_name);
    void FindNewId();
    void FindDupeId();
    int NumberOrZero(String text);
    String HideZeroText(String text);
    String ResultText();
    void CheckDuplicateName();
};

extern PACKAGE TFormEditTrainer *FormEditTrainer;

#endif
