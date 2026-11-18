#ifndef UnitSelectItemH
#define UnitSelectItemH

#include <Classes.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// Picks an item from a list of the items tab, by type and search, with a hint of where it is used.
class TFormSelectItem : public TForm
{
__published:
    TPanel *Panel1;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *Panel4;
    TElTreeStringGrid *itemGrid;
    TLabel *Label5;
    TEdit *searchEdit;
    TLabel *labelChangeItemView;
    TImage *imageChangeItemView;
    TLabel *labelSelectedItemName;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TLabel *hintIDCaption;
    TTimer *Timer1;
    TImage *hintDropCountImage;
    TLabel *hintDropCountString;
    TImage *hintShopCountImage;
    TLabel *hintShopCountString;
    TImage *hintCraftCountImage;
    TLabel *hintCraftCountString;
    void __fastcall itemGridItemFocused(TObject *Sender);
    void __fastcall itemGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall itemGridSortBegin(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall labelChangeItemViewClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall searchEditChange(TObject *Sender);
    void __fastcall itemGridDblClick(TObject *Sender);
    void __fastcall buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall Timer1Timer(TObject *Sender);
private:
public:
    TElPopupMenu *view_menu;
    int hint_row;
    String view_caption;
    int selected_id;
    String selected_name;

    __fastcall TFormSelectItem(TComponent *owner);
    void Open(TForm *owner, String caption, int kind);
    void FillGrid();
    void AlignColumns();
    void ShowItemImage(int gfx);
    void __fastcall ViewMenuClick(TObject *sender);
    void ShowView(String caption, String filter);
    TElTreeItem * FindItem(String id_text);
};

extern PACKAGE TFormSelectItem *FormSelectItem;

#endif
