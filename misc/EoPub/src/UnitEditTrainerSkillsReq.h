#ifndef UnitEditTrainerSkillsReqH
#define UnitEditTrainerSkillsReqH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElHeader.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeSpinEdit.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormTrainerEditSkills;

// Edits the skills one trainer skill requires: up to four.
class TFormTrainerEditSkillsReq : public TForm
{
__published:
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *Panel1;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewShopCraft;
    TSpeedButton *buttonCleanList;
    TElTreeStringGrid *dropGrid;
    TPanel *Panel2;
    TPanel *Panel6;
    TPanel *Panel7;
    TShape *Shape2;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TLabel *hintIDCaption;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TElTreeInplaceSpinEdit *ElTreeInplaceSpinEdit1;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupMoveUp;
    TElMenuItem *popupMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall dropGridEnter(TObject *Sender);
    void __fastcall dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall dropGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall buttonNewShopCraftClick(TObject *Sender);
    void __fastcall dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall dropGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
private:
public:
    TElTreeStringGrid *pub_grid;
    TElTreeItem *learn_row;
    int hint_row;
    int edit_column;
    bool rows_deleted;

    __fastcall TFormTrainerEditSkillsReq(TComponent *owner);
    void Open(TFormTrainerEditSkills *skills_form, String skill_id, String skill_name, TElTreeItem *row);
    void LayoutCaptions();
    void ShowCount();
    void Renumber();
    void FillGrid();
    TElTreeItem * FindSkillRow(String skill_id);
    void ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id);
    void ShowSkillHint(int x, int y);
};

extern PACKAGE TFormTrainerEditSkillsReq *FormTrainerEditSkillsReq;

#endif
