#ifndef UnitEditInnQuizH
#define UnitEditInnQuizH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeStdEditors.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Datagridcol.h"

// Edits the quiz of one inn: its questions and their answers.
class TFormEditInnQuiz : public TForm
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
    TPanel *Panel10;
    TElTreeStringGrid *lineGrid;
    TPanel *Panel11;
    TLabel *labelGridCaption;
    TSpeedButton *buttonNewQuiz;
    TSpeedButton *buttonCleanList;
    TPanel *Panel12;
    TPanel *Panel14;
    TPanel *Panel15;
    TShape *Shape2;
    TPanel *panelNoAdds;
    TElTreeInplaceEdit *ElTreeInplaceEdit1;
    TElPopupMenu *ElPopupMenu1;
    TElMenuItem *popupMoveUp;
    TElMenuItem *popupMoveDown;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupDelete;
    TPanel *Panel2;
    TShape *Shape1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonCleanListClick(TObject *Sender);
    void __fastcall buttonNewQuizClick(TObject *Sender);
    void __fastcall popupMoveDownClick(TObject *Sender);
    void __fastcall popupMoveUpClick(TObject *Sender);
    void __fastcall popupDeleteClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall lineGridEnter(TObject *Sender);
    void __fastcall ElTreeInplaceEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion);
    void __fastcall lineGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeInplaceEdit1BeforeOperation(TObject *Sender, bool &DefaultConversion);
    void __fastcall lineGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    TCustomElTree *pub_grid;
    DataGridCol *name_column;
    int record_index;
    bool rows_deleted;

    __fastcall TFormEditInnQuiz(TComponent *owner);
    void Open(int index);
    void LayoutCaptions();
    void Renumber();
    void FillGrid();
    void ShowCount();
};

extern PACKAGE TFormEditInnQuiz *FormEditInnQuiz;

#endif
