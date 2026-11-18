#ifndef UnitEditNpcH
#define UnitEditNpcH

#include <Classes.hpp>
#include <Controls.hpp>
#include <ElCombos.hpp>
#include <ElHeader.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElStrUtils.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Dataform.h"
#include "Datagridcol.h"

// The npc editor: creates, edits or duplicates one npc of the npc file, with its behaviour's fields.
class TFormNpcEdit : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *Panel4;
    TLabel *captionCurrentType;
    TLabel *labelEditGridVisible;
    TLabel *Label1;
    TLabel *Label2;
    TElSpinEdit *spinEditGfx;
    TPanel *Panel7;
    TPanel *Panel6;
    TPanel *Panel1;
    TEdit *editName;
    TPanel *panelImage;
    TImage *imageNpc;
    TLabel *Label11;
    TLabel *Label6;
    TLabel *Label7;
    TElSpinEdit *spinEditLvl;
    TElSpinEdit *spinEditExp;
    TElComboBox *comboEditBehaviour;
    TLabel *Label3;
    TLabel *Label4;
    TElSpinEdit *spinEditBehaviourID;
    TElTreeStringGrid *editStringGrid;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TImage *imageWarnDupe;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall panelImageClick(TObject *Sender);
    void __fastcall Panel6Click(TObject *Sender);
    void __fastcall Panel7Click(TObject *Sender);
    void __fastcall spinEditGfxChange(TObject *Sender);
    void __fastcall comboEditBehaviourChange(TObject *Sender);
    void __fastcall spinEditGfxEnter(TObject *Sender);
    void __fastcall comboEditBehaviourEnter(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall editStringGridEnter(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item,
    TElHeaderSection *Section, TElFString &Text, THintWindow *HintWindow, TPoint &MousePos,
    bool &DoShowHint);
    void __fastcall editStringGridItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall comboEditBehaviourExit(TObject *Sender);
    void __fastcall comboEditBehaviourKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall editStringGridItemFocused(TObject *Sender);
    void __fastcall editStringGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall comboEditBehaviourKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditGfxExit(TObject *Sender);
private:
public:
    bool data_form_made;
    DataForm *data_form;
    TCustomElTree *pub_grid;
    TCustomElTree *edit_grid;
    DataGridCol *gfx_column;
    DataGridCol *name_column;
    DataGridCol *level;
    DataGridCol *experience;
    DataGridCol *behaviour_column;
    DataGridCol *behaviour_id_column;
    bool combos_filled;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormNpcEdit(TComponent *owner);
    void Open(int operation, int index);
    void ShowOperation(int operation);
    void LoadFields(int index, unsigned char operation);
    void FillCombos();
    void FindNewId();
    void FindDupeId();
    String ResultText();
    void CheckDuplicateName();
    void TypeChosen();
};

extern PACKAGE TFormNpcEdit *FormNpcEdit;

#endif
