#ifndef UnitEditTrainerSkillsH
#define UnitEditTrainerSkillsH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElHeader.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeComboBox.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeSpinEdit.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"
#include "Learnitem.h"

// Edits the skills one trainer teaches, with their price, level, class and requirements.
class TFormTrainerEditSkills : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *trainerNameOriginal;
    TLabel *trainerNameOriginalID;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *Panel1;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewTrainerSkill;
    TElTreeStringGrid *dropGrid;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TLabel *hintIDCaption;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TPanel *panelImage;
    TImage *imageShop;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupMoveUp;
    TElMenuItem *popipMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    TElTreeInplaceSpinEdit *ElTreeInplaceSpinEdit1;
    TSpeedButton *buttonCleanList;
    TElTreeInplaceComboBox *ElTreeInplaceComboBox1;
    TPanel *hintPanelReq1;
    TLabel *labelCaptionReq1;
    TLabel *labelCaptionIDReq1;
    TLabel *hintReq1AddHint;
    TPanel *panelReq1;
    TLabel *hintCaptionReq1;
    TLabel *hintSubStringReq1;
    TLabel *hintIDCaptionReq1;
    TPanel *hintImagePanelReq1;
    TImage *hintImageReq1;
    TPanel *panelReq2;
    TLabel *hintCaptionReq2;
    TLabel *hintSubStringReq2;
    TLabel *hintIDCaptionReq2;
    TPanel *hintImagePanelReq2;
    TImage *hintImageReq2;
    TPanel *panelReq3;
    TLabel *hintCaptionReq3;
    TLabel *hintSubStringReq3;
    TLabel *hintIDCaptionReq3;
    TPanel *hintImagePanelReq3;
    TImage *hintImageReq3;
    TPanel *panelReq4;
    TLabel *hintCaptionReq4;
    TLabel *hintSubStringReq4;
    TLabel *hintIDCaptionReq4;
    TPanel *hintImagePanelReq4;
    TImage *hintImageReq4;
    TPanel *hintPanelReq2;
    TLabel *hintReq2Caption;
    TLabel *hintReq2ID;
    TLabel *hintReq2AddHint;
    TPanel *hintReq2Holder;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label5;
    TLabel *Label4;
    TLabel *Label3;
    TPanel *Panel4;
    TPanel *hintStr;
    TPanel *Panel7;
    TPanel *hintWis;
    TPanel *Panel9;
    TPanel *hintInt;
    TPanel *Panel12;
    TPanel *hintAgi;
    TPanel *Panel14;
    TPanel *hintCon;
    TPanel *Panel16;
    TPanel *hintCha;
    TPanel *Panel2;
    TPanel *Panel6;
    TPanel *Panel8;
    TShape *Shape1;
    TPanel *Panel10;
    TShape *Shape3;
    TPanel *Panel15;
    TShape *Shape4;
    TPanel *Panel18;
    TShape *Shape6;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label9;
    TPanel *panelNoAdds;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall dropGridEnter(TObject *Sender);
    void __fastcall ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall buttonNewTrainerSkillClick(TObject *Sender);
    void __fastcall buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall dropGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeInplaceComboBox1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall ElTreeInplaceComboBox1ValidateResult(TObject *Sender, bool &InputValid);
    void __fastcall dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    TElTreeStringGrid *pub_grid;
    char unused_bytes[4];
    DataGridCol *name_column;
    int record_index;
    int hint_row;
    int requirement_hint_row;
    int stats_hint_row;
    int edit_column;
    bool rows_deleted;

    __fastcall TFormTrainerEditSkills(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void ShowCount();
    void Renumber();
    void FillGrid();
    String ClassName(int class_id);
    String SkillRequirementCount(LearnItemVal *learn_item);
    String StatsSummary(LearnItemVal *learn_item);
    TElTreeItem * FindSkillRow(String skill_id);
    void ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id);
    void ShowSkillHint(int x, int y);
    void ShowRequiredSkillsHint(int x, int y);
    void ShowStatsHint(int x, int y);
};

extern PACKAGE TFormTrainerEditSkills *FormTrainerEditSkills;

#endif
