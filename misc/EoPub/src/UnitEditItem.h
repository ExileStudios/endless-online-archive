#ifndef UnitEditItemH
#define UnitEditItemH

enum EifItemType
{
    EifItemType_Weapon = 10,
    EifItemType_Shield = 11,
    EifItemType_Armor = 12,
    EifItemType_Hat = 13,
    EifItemType_Boots = 14
};

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

// The item editor: creates, edits or duplicates one item of the item file, with its type's fields.
class TFormItemEdit : public TForm
{
__published:
    TLabel *Label1;
    TPanel *Panel1;
    TEdit *editName;
    TLabel *Label2;
    TPanel *panelImage;
    TElSpinEdit *spinEditGfx;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TLabel *Label7;
    TLabel *Label8;
    TElTreeStringGrid *editStringGrid;
    TPanel *Panel3;
    TElSpinEdit *spinEditWeight;
    TElComboBox *comboEditItemType;
    TElComboBox *comboEditSubType;
    TElComboBox *comboEditUnique;
    TElComboBox *comboEditSize;
    TImage *imageItem;
    TPanel *Panel4;
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *captionCurrentType;
    TLabel *Label11;
    TLabel *labelEditGridVisible;
    TPanel *Panel6;
    TPanel *Panel7;
    TShape *Shape1;
    TLabel *itemNameOriginalID;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TImage *imageWarnDupe;
    void __fastcall Panel5Click();
    void __fastcall Panel4Click();
    void __fastcall comboEditItemTypeEnter(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall comboEditItemTypeChange(TObject *Sender);
    void __fastcall spinEditGfxChange(TObject *Sender);
    void __fastcall panelImageClick(TObject *Sender);
    void __fastcall editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall Panel6Click(TObject *Sender);
    void __fastcall Panel7Click(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditDuraEnter(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall editStringGridEnter(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item,
    TElHeaderSection *Section, TElFString &Text, THintWindow *HintWindow, TPoint &MousePos,
    bool &DoShowHint);
    void __fastcall editStringGridItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall comboEditItemTypeExit(TObject *Sender);
    void __fastcall comboEditItemTypeKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall editStringGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editStringGridItemFocused(TObject *Sender);
    void __fastcall comboEditItemTypeKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditGfxExit(TObject *Sender);
private:
public:
    bool data_form_made;
    DataForm *data_form;
    TCustomElTree *pub_grid;
    TCustomElTree *edit_grid;
    DataGridCol *gfx_column;
    DataGridCol *name_column;
    DataGridCol *type_column;
    DataGridCol *subtype_column;
    DataGridCol *unique_column;
    DataGridCol *weight_column;
    DataGridCol *size_column;
    bool combos_filled;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormItemEdit(TComponent *owner);
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

extern PACKAGE TFormItemEdit *FormItemEdit;

#endif
