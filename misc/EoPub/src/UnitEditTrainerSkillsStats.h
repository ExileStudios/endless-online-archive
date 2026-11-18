#ifndef UnitEditTrainerSkillsStatsH
#define UnitEditTrainerSkillsStatsH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTree.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormTrainerEditSkills;

// Edits the six stat requirements of one skill a trainer teaches.
class TFormTrainerEditSkillsStats : public TForm
{
__published:
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label10;
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TElSpinEdit *spinEditStr;
    TElSpinEdit *spinEditInt;
    TElSpinEdit *spinEditWis;
    TElSpinEdit *spinEditAgi;
    TElSpinEdit *spinEditCon;
    TElSpinEdit *spinEditCha;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall spinEditStrKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditStrEnter(TObject *Sender);
    void __fastcall spinEditStrExit(TObject *Sender);
private:
public:
    TElTreeItem *learn_row;

    __fastcall TFormTrainerEditSkillsStats(TComponent *owner);
    void Open(TFormTrainerEditSkills *skills_form, String skill_id, String skill_name, TElTreeItem *row);
    void LayoutCaptions();
    void LoadStats();
    String StatsSummary();
};

extern PACKAGE TFormTrainerEditSkillsStats *FormTrainerEditSkillsStats;

#endif
