#ifndef UnitEditNpcDropsH
#define UnitEditNpcDropsH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElHeader.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeSpinEdit.hpp>
#include <ElTreeStdEditors.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// Edits the item drops of one npc: each item, its least and most amount, and its rate.
class TFormNpcEditDrops : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *panelImage;
    TImage *imageNpc;
    TLabel *panelLabel;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *Panel1;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewNpc;
    TElTreeStringGrid *dropGrid;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupMoveUp;
    TElMenuItem *popupMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    TElTreeInplaceSpinEdit *ElTreeInplaceSpinEdit1;
    TLabel *hintIDCaption;
    TElTreeInplaceEdit *ElTreeInplaceEdit1;
    TSpeedButton *buttonCleanList;
    TPanel *Panel12;
    TPanel *Panel14;
    TPanel *Panel15;
    TShape *Shape2;
    TPanel *Panel2;
    TShape *Shape1;
    TPanel *Panel4;
    TShape *Shape3;
    TPanel *Panel6;
    TShape *Shape4;
    TPanel *panelNoAdds;
    TPanel *panelTotalPerc;
    TLabel *labelPercTotal;
    TImage *imageBadTotal;
    TTimer *timerUpdate;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall dropGridEnter(TObject *Sender);
    void __fastcall ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall buttonNewNpcClick(TObject *Sender);
    void __fastcall ElTreeInplaceEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall timerUpdateTimer(TObject *Sender);
private:
public:
    TElTreeStringGrid *pub_grid;
    DataGridCol *gfx_column;
    DataGridCol *name_column;
    int record_index;
    int hint_row;
    int edit_column;
    bool rows_deleted;

    __fastcall TFormNpcEditDrops(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void ShowCount();
    void ShowRateTotal();
    void Renumber();
    void FillGrid();
    TElTreeItem * FindItemRow(String item_id);
    void ShowNpcGraphic(int gfx);
    void ShowItemGraphic(TImage *image, TPanel *panel, int gfx);
    void ShowItemHint(int x, int y);
    String FormatRate(String text, short limit);
    int ParseNumber(String text);
};

extern PACKAGE TFormNpcEditDrops *FormNpcEditDrops;

#endif
