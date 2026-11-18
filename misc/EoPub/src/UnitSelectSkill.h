#ifndef UnitSelectSkillH
#define UnitSelectSkillH

#include <Classes.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// Picks a skill from a list of the skills tab, by type and search, with a hint of who teaches it.
class TFormSelectSkill : public TForm
{
__published:
    TPanel *Panel1;
    TPanel *Panel4;
    TLabel *Label5;
    TLabel *labelChangeSkillView;
    TImage *imageChangeSkillView;
    TLabel *labelSelectedSkillName;
    TEdit *searchEdit;
    TElTreeStringGrid *itemGrid;
    TPanel *Panel3;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TPanel *hintPanel;
    TLabel *hintCaption;
    TLabel *hintSubString;
    TLabel *hintIDCaption;
    TLabel *hintLearnCountString;
    TPanel *hintImagePanel;
    TImage *hintImage;
    TTimer *Timer1;
    TImage *hintLearnCountImage;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall itemGridItemFocused(TObject *Sender);
    void __fastcall itemGridDblClick(TObject *Sender);
    void __fastcall buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall itemGridMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall itemGridSortBegin(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall labelChangeSkillViewClick(TObject *Sender);
    void __fastcall searchEditChange(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
private:
public:
    TElPopupMenu *view_menu;
    int hint_row;
    String view_caption;
    int selected_id;
    String selected_name;

    __fastcall TFormSelectSkill(TComponent *owner);
    void Open(TForm *owner, String caption, int kind);
    void FillGrid();
    void AlignColumns();
    void ShowSkillImage(int icon);
    void __fastcall ViewMenuClick(TObject *sender);
    void ShowView(String caption, String filter);
    TElTreeItem * FindSkill(String id_text);
};

extern PACKAGE TFormSelectSkill *FormSelectSkill;

#endif
