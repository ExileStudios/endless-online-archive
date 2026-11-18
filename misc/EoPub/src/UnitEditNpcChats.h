#ifndef UnitEditNpcChatsH
#define UnitEditNpcChatsH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElSpin.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeStdEditors.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// Edits the chat lines of one npc, with how often and how likely it speaks.
class TFormNpcEditChats : public TForm
{
__published:
    TPanel *Panel5;
    TLabel *labelOperation;
    TImage *imageOperation;
    TLabel *itemNameOriginal;
    TLabel *itemNameOriginalID;
    TPanel *panelImage;
    TImage *imageNpc;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TLabel *panelLabel;
    TElTreeInplaceEdit *ElTreeInplaceEdit1;
    TPanel *Panel1;
    TPanel *Panel2;
    TLabel *Label1;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TElSpinEdit *spinInterval;
    TElSpinEdit *spinChance;
    TPanel *Panel7;
    TPanel *Panel4;
    TPanel *Panel6;
    TPanel *Panel8;
    TPanel *Panel9;
    TPanel *Panel10;
    TElTreeStringGrid *lineGrid;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewNpc;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupDelete;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupMoveDown;
    TElMenuItem *popupMoveUp;
    TSpeedButton *buttonCleanList;
    TPanel *Panel12;
    TPanel *Panel14;
    TPanel *Panel15;
    TShape *Shape2;
    TPanel *panelNoAdds;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Panel7Click(TObject *Sender);
    void __fastcall Panel1Click(TObject *Sender);
    void __fastcall Panel2Click(TObject *Sender);
    void __fastcall Panel4Click(TObject *Sender);
    void __fastcall Panel6Click(TObject *Sender);
    void __fastcall buttonNewNpcClick(TObject *Sender);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall spinIntervalEnter(TObject *Sender);
    void __fastcall spinIntervalExit(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall lineGridEnter(TObject *Sender);
    void __fastcall ElTreeInplaceEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall lineGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeInplaceEdit1BeforeOperation(TObject *Sender, bool &DefaultConversion);
    void __fastcall lineGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    TElTreeStringGrid *pub_grid;
    DataGridCol *gfx_column;
    DataGridCol *name_column;
    int record_index;
    unsigned char chat_interval;
    unsigned char chat_chance;
    bool rows_deleted;

    __fastcall TFormNpcEditChats(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void Renumber();
    void FillGrid();
    void ShowNpcGraphic(int gfx);
    void ShowCount();
};

extern PACKAGE TFormNpcEditChats *FormNpcEditChats;

#endif
