#ifndef UnitEditShopItemsH
#define UnitEditShopItemsH

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

#include "Datagridcol.h"

// Edits the trade list of one shop: each item with its buy and sell price and most amount.
class TFormShopEditItems : public TForm
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
    TPanel *Panel1;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewShopItem;
    TElTreeStringGrid *dropGrid;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TLabel *hintIDCaption;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupMoveUp;
    TElMenuItem *popupMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    TElTreeInplaceSpinEdit *ElTreeInplaceSpinEdit1;
    TPanel *panelImage;
    TImage *imageShop;
    TSpeedButton *buttonCleanList;
    TPanel *Panel2;
    TPanel *Panel4;
    TShape *Shape1;
    TPanel *Panel6;
    TPanel *Panel7;
    TShape *Shape2;
    TPanel *Panel8;
    TShape *Shape3;
    TPanel *Panel9;
    TShape *Shape4;
    TPanel *panelNoAdds;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall dropGridEnter(TObject *Sender);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall dropGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonNewShopItemClick(TObject *Sender);
    void __fastcall dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall dropGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    TElTreeStringGrid *pub_grid;
    DataGridCol *name_column;
    int record_index;
    int hint_row;
    int edit_column;
    bool rows_deleted;

    __fastcall TFormShopEditItems(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void ShowCount();
    void Renumber();
    void FillGrid();
    TElTreeItem * FindItemRow(String item_id);
    void ShowItemGraphic(TImage *image, TPanel *panel, int gfx);
    void ShowItemHint(int x, int y);
};

extern PACKAGE TFormShopEditItems *FormShopEditItems;

#endif
