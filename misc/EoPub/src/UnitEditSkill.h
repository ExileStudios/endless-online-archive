#ifndef UnitEditSkillH
#define UnitEditSkillH

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

// The skill editor: creates, edits or duplicates one skill of the skill file, with its type's fields.
class TFormSkillEdit : public TForm
{
__published:
    TPanel *Panel3;
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *Panel4;
    TLabel *captionCurrentType;
    TLabel *labelEditGridVisible;
    TLabel *Label1;
    TPanel *Panel1;
    TEdit *editName;
    TLabel *Label2;
    TElSpinEdit *spinEditGfx;
    TElSpinEdit *spinIconGfx;
    TPanel *panelImage;
    TImage *imageAnim;
    TLabel *Label11;
    TImage *imageIcon;
    TImage *imageCorner;
    TElTreeStringGrid *editStringGrid;
    TPanel *Panel10;
    TEdit *editChant;
    TLabel *Label4;
    TLabel *Label3;
    TLabel *Label5;
    TElComboBox *comboEditNature;
    TLabel *Label6;
    TElComboBox *comboEditType;
    TLabel *Label7;
    TElComboBox *comboEditTarget;
    TElComboBox *comboEditTarget2;
    TLabel *Label8;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TImage *imageWarnDupe;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editStringGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall spinEditGfxChange(TObject *Sender);
    void __fastcall comboEditTypeChange(TObject *Sender);
    void __fastcall spinEditGfxEnter(TObject *Sender);
    void __fastcall comboEditNatureEnter(TObject *Sender);
    void __fastcall imageIconClick(TObject *Sender);
    void __fastcall spinIconGfxChange(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall imageAnimClick(TObject *Sender);
    void __fastcall editStringGridEnter(TObject *Sender);
    void __fastcall editNameExit(TObject *Sender);
    void __fastcall editStringGridShowLineHint(TObject *Sender, TElTreeItem *Item,
    TElHeaderSection *Section, TElFString &Text, THintWindow *HintWindow, TPoint &MousePos,
    bool &DoShowHint);
    void __fastcall editStringGridItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall comboEditNatureExit(TObject *Sender);
    void __fastcall comboEditNatureKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editNameChange(TObject *Sender);
    void __fastcall editStringGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall editStringGridItemFocused(TObject *Sender);
    void __fastcall comboEditNatureKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall spinEditGfxExit(TObject *Sender);
    void __fastcall editChantExit(TObject *Sender);
private:
public:
    bool data_form_made;
    DataForm *data_form;
    TCustomElTree *pub_grid;
    TCustomElTree *edit_grid;
    DataGridCol *icon_gfx_column;
    DataGridCol *effect_gfx_column;
    DataGridCol *name_column;
    DataGridCol *chant_column;
    DataGridCol *nature_column;
    DataGridCol *type_column;
    DataGridCol *target_column;
    DataGridCol *target_type_column;
    bool combos_filled;
    String record_id;
    int name_font_color;
    bool font_color_saved;
    int record_index;
    int save_mode;
    int open_operation;

    __fastcall TFormSkillEdit(TComponent *owner);
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

extern PACKAGE TFormSkillEdit *FormSkillEdit;

#endif
