#ifndef UnitToolsItemsH
#define UnitToolsItemsH

#include <Classes.hpp>
#include <ElCheckCtl.hpp>
#include <ElCombos.hpp>
#include <ElPgCtl.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTree.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

#include "Datagridcol.h"

// The item tools: gives weapons and armour a rarity, and cleans a column.
class TFormToolsItems : public TForm
{
__published:
    TPanel *Panel1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TElPageControl *ElPageControl1;
    TElTabSheet *tabUniquifier;
    TPanel *Panel4;
    TPanel *Panel2;
    TPanel *Panel3;
    TPanel *Panel5;
    TPanel *Panel8;
    TLabel *Label18;
    TLabel *Label19;
    TLabel *Label20;
    TLabel *Label21;
    TElSpinEdit *spinEditStats1;
    TElSpinEdit *spinEditStats2;
    TElSpinEdit *spinEditStats3;
    TElSpinEdit *spinEditStats4;
    TElTabSheet *tabCleaner;
    TPanel *Panel15;
    TLabel *Label28;
    TLabel *Label29;
    TLabel *Label30;
    TPanel *Panel16;
    TLabel *Label32;
    TShape *Shape1;
    TElCheckBox *ElCheckBox1;
    TElCheckBox *ElCheckBox2;
    TImage *Image1;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElSpinEdit *spinEditDmg1;
    TLabel *Label22;
    TElSpinEdit *spinEditDmg2;
    TLabel *Label23;
    TElSpinEdit *spinEditDmg3;
    TLabel *Label24;
    TElSpinEdit *spinEditDmg4;
    TLabel *Label25;
    TElComboBox *comboItemType;
    TLabel *Label2;
    TLabel *labelUniqueTargets1;
    TLabel *Label6;
    TLabel *Label10;
    TLabel *Label11;
    TLabel *Label7;
    TLabel *labelUniqueTargets2;
    TLabel *Label3;
    TLabel *Label12;
    TLabel *Label8;
    TLabel *labelUniqueTargets3;
    TLabel *Label4;
    TLabel *Label13;
    TLabel *Label9;
    TLabel *labelUniqueTargets4;
    TLabel *Label5;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TLabel *Label26;
    TLabel *Label27;
    TElComboBox *comboTargetItems;
    TLabel *Label31;
    TElCheckBox *checkCleanerOOB;
    TElCheckBox *checkCleanerAll;
    TElComboBox *comboCleanColumn;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall comboItemTypeChange(TObject *Sender);
    void __fastcall comboTargetItemsChange(TObject *Sender);
    void __fastcall spinEditDmg1Change(TObject *Sender);
    void __fastcall ElCheckBox1Click(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall ElCheckBox1KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall checkCleanerAllClick(TObject *Sender);
private:
public:
    int unique_count;
    int legendary_count;
    int rare_count;
    int normal_count;
    DataGridCol *str_column;
    DataGridCol *int_column;
    DataGridCol *wis_column;
    DataGridCol *agi_column;
    DataGridCol *con_column;
    DataGridCol *cha_column;
    DataGridCol *damage_column;
    DataGridCol *armor_column;
    int changed_count;

    __fastcall TFormToolsItems(TComponent *owner);
    void Open(TFormMain *main);
    void UniquifyItems(char count_only);
    bool TryRarity(char count_only, TElTreeItem *row, DataGridCol *unique_column,
    TElCheckBox *stats_check, TElSpinEdit *stats_needed, TElCheckBox *power_check,
    TElSpinEdit *power_needed, unsigned char rarity);
    void SuggestPowerSteps(String column_name);
    int CleanColumn(char out_of_bounds, char all_values);
};

extern PACKAGE TFormToolsItems *FormToolsItems;

#endif
