#ifndef DataformH
#define DataformH

#include <ElCombos.hpp>
#include <ElTree.hpp>
#include <ElTreeComboBox.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeSpinEdit.hpp>
#include <ElTreeStdEditors.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

struct DataComboItem;
struct DataGridCol;
struct DataGridUI;

// An edit form for one record: a grid with a line per column, its editors and its graphics.
struct DataForm
{
    TElTreeInplaceEdit *text_editor;
    TElTreeInplaceSpinEdit *spin_editor;
    TElTreeInplaceComboBox *combo_editor;
    TCustomElTree *main_grid;
    TElTreeStringGrid *form_grid;
    DataGridUI *grid_ui;
    int edit_section;
    int edit_row;
    DataGridCol *edit_column;
    int gfx_first;
    int gfx_step;
    char transparent;
    HINSTANCE__ *gfx_library;
    int animation_first;
    int animation_step;
    HINSTANCE__ *animation_library;
    String name;

    DataForm(String title, DataGridUI *ui, int tree, TElTreeStringGrid *grid);
    ~DataForm();
    void SetGraphics(HINSTANCE__ *library, int first, int step);
    void SetSecondGraphics(HINSTANCE__ *library, int first, int step);
    void SetTransparent(char on);
    bool ShowGraphic(TPanel *panel, TImage *image, TLabel *label, int number);
    void ShowAnimation(TPanel *panel, TImage *image, TLabel *label, int number);
    bool DrawFrames(TImage *image, int tree, int number);
    bool DrawFrame(TImage *image, int number, int frame, char drawn);
    void FillCombo(TElComboBox *combo, String column_name);
    void SelectFirstRow();
    char EditCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor, String column_name);
    void CreateEditors();
    void __fastcall SpinEditorKeyPress(TObject *sender, char &key);
    void __fastcall EditorKeyDown(TObject *sender, bool &handled);
    void __fastcall TextEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall SpinEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall ComboEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall TextEditorAfter(TObject *sender, bool &changed);
    void __fastcall SpinEditorAfter(TObject *sender, bool &changed);
    void __fastcall ComboEditorAfter(TObject *sender, bool &changed);
    int FillGrid(TElTreeStringGrid *grid, DataComboItem *kind, unsigned char operation, String kind_text);
    int DigitsOnly(String text);
    String HideZeroText(String text);
    String NumberText(int value);
};

#endif
