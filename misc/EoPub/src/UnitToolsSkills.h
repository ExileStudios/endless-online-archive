#ifndef UnitToolsSkillsH
#define UnitToolsSkillsH

#include <Classes.hpp>
#include <ElCheckCtl.hpp>
#include <ElCombos.hpp>
#include <ElPgCtl.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// The skill tools: cleans a column of the skill grid.
class TFormToolsSkills : public TForm
{
__published:
    TPanel *Panel1;
    TImage *Image1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TElPageControl *ElPageControl1;
    TElTabSheet *tabCleaner;
    TLabel *Label29;
    TLabel *Label30;
    TPanel *Panel15;
    TLabel *Label28;
    TLabel *Label32;
    TShape *Shape1;
    TElComboBox *comboCleanColumn;
    TPanel *Panel16;
    TLabel *Label31;
    TElCheckBox *checkCleanerOOB;
    TElCheckBox *checkCleanerAll;
    TPanel *Panel4;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall comboCleanColumnKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall checkCleanerAllClick(TObject *Sender);
private:
public:
    int cleaned_count;

    __fastcall TFormToolsSkills(TComponent *owner);
    void ShowTools(TFormMain *parent);
    int CleanColumn(char out_of_bounds, char all_values);
};

extern PACKAGE TFormToolsSkills *FormToolsSkills;

#endif
