#ifndef UnitEditShopCraftsH
#define UnitEditShopCraftsH

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
#include "Shopcraft.h"

// Edits the crafts of one shop: each item it crafts and the ingredients it takes.
class TFormShopEditCrafts : public TForm
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
    TSpeedButton *buttonNewShopCraft;
    TSpeedButton *buttonCleanList;
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
    TElMenuItem *popupMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    TElTreeInplaceSpinEdit *ElTreeInplaceSpinEdit1;
    TPanel *hintPanelReq1;
    TPanel *panelReq1;
    TPanel *hintImagePanelReq1;
    TImage *hintImageReq1;
    TLabel *hintCaptionReq1;
    TLabel *hintSubStringReq1;
    TLabel *hintIDCaptionReq1;
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
    TLabel *labelCaptionReq1;
    TLabel *labelCaptionIDReq1;
    TLabel *hintCraftCount1;
    TLabel *hintCraftCount2;
    TLabel *hintCraftCount3;
    TLabel *hintCraftCount4;
    TLabel *hintReq1AddHint;
    TPanel *Panel12;
    TPanel *Panel14;
    TPanel *Panel2;
    TShape *Shape1;
    TPanel *Panel4;
    TShape *Shape3;
    TPanel *panelNoAdds;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall dropGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall dropGridEnter(TObject *Sender);
    void __fastcall dropGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall buttonNewShopCraftClick(TObject *Sender);
    void __fastcall dropGridInplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall dropGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
    void __fastcall dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall popupMoveUpClick(TObject *Sender);
private:
public:
    TElTreeStringGrid *pub_grid;
    DataGridCol *name_column;
    int record_index;
    int hint_row;
    int ingredient_hint_row;
    int edit_column;
    bool rows_deleted;

    __fastcall TFormShopEditCrafts(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void ShowCount();
    void Renumber();
    void FillGrid();
    String IngredientCount(ShopCraftVal *craft);
    TElTreeItem * FindItemRow(String item_id);
    void ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id);
    void ShowItemHint(int x, int y);
    void ShowIngredientHint(int x, int y);
};

extern PACKAGE TFormShopEditCrafts *FormShopEditCrafts;

#endif
