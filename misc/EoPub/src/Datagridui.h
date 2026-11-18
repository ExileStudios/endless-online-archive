#ifndef DatagriduiH
#define DatagriduiH

#include <Classes.hpp>
#include <Controls.hpp>
#include <ElMenus.hpp>
#include <ElTree.hpp>
#include <ElTreeComboBox.hpp>
#include <ElTreeGrids.hpp>
#include <ElTreeSpinEdit.hpp>
#include <ElTreeStdEditors.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

struct DataComboItem;
struct DataGrid;
struct DataGridCol;
struct DataGridView;

// One pub tab's grids and controls: editors, search, views, filter, info labels, gfx viewer and messages.
struct DataGridUI
{
    bool is_ready;
    bool is_syncing;
    String filter_text;
    int shown_graphic;
    int main_section;
    int main_row;
    int sub_section;
    int sub_row;
    TElTreeStringGrid *edit_grid;
    DataGridCol *edit_column;
    TElTreeStringGrid *main_grid;
    TElTreeStringGrid *sub_grid;
    DataGrid *data_grid;
    TStringList *copied_row;
    TElTreeInplaceSpinEdit *main_spin_editor;
    int gfx_first;
    int icon_first;
    String table_name;
    String record_name;
    bool has_changes;
    char header_locked;
    bool first_column_locked;
    bool source_edited;
    String source_text;
    DataGridCol *source_column;
    int gfx_step;
    int gfx_base;
    HINSTANCE__ *gfx_library;
    unsigned char gfx_transparent;
    String gfx_column_name;
    int icon_step;
    int icon_base;
    HINSTANCE__ *icon_library;
    unsigned char icon_transparent;
    String icon_column_name;
    bool gfx_collapsed;
    bool has_copied_row;
    TElTreeInplaceEdit *main_text_editor;
    TElTreeInplaceComboBox *main_combo_editor;
    TElTreeInplaceEdit *sub_text_editor;
    TElTreeInplaceSpinEdit *sub_spin_editor;
    TElTreeInplaceComboBox *sub_combo_editor;
    TPanel *edit_panel;
    TEdit *search_edit;
    bool is_searching;
    TLabel *view_label;
    TElPopupMenu *view_menu;
    int view_menu_left;
    int view_menu_top;
    TLabel *filter_label;
    TImage *filter_image;
    TElPopupMenu *filter_menu;
    int filter_menu_left;
    int filter_menu_top;
    String filter_column_name;
    TLabel *info_value_label;
    TLabel *info_id_label;
    TLabel *info_name_label;
    TImage *info_image;
    String info_column_name;
    TImage *gfx_image;
    TLabel *gfx_id_label;
    TLabel *gfx_name_label;
    TPanel *gfx_panel;
    TImage *icon_image;
    TLabel *column_label;
    String label_column_name;
    TPanel *message_back;
    TPanel *message_panel;
    TTimer *message_timer;
    short message_ticks;

    DataGridUI(String name, String caption, TElTreeStringGrid *grid,
    TElTreeStringGrid *detail_grid, String index_name);
    ~DataGridUI();
    int SetMainGraphics(HINSTANCE__ *library, String name, int width, int first, int step, char transparent);
    int SetSubGraphics(HINSTANCE__ *library, String name, int width, int first, int step, char transparent);
    void BeginUpdate();
    void EndUpdate();
    void SetHeaderLock(int unused, char locked);
    void LockFirstColumnOnScroll();
    void ShowColumns(char all, int unused);
    void SetItemHints(char on, int unused);
    void FillSubGrid(char clear);
    bool IsChanged();
    void SortById();
    void ClearChanged();
    void SetEditPanel(TPanel *panel);
    void SetSearchEdit(TEdit *edit);
    void SetViewMenu(TElPopupMenu *menu, TLabel *label, int left, int top);
    void SetFilterMenu(TElPopupMenu *menu, TLabel *label, TImage *image, int left, int top,
    String column_name, String caption);
    void SetInfoLabels(TLabel *name_label, TLabel *id_label, TLabel *value_label,
    TImage *image, String column_name);
    void SetGfxViewer(TImage *image, TPanel *panel, TLabel *id_label,
    TLabel *name_label);
    void SetIconImage(TImage *image);
    void SetColumnLabel(TLabel *label, String column_name);
    void SetMessagePanel(TPanel *back, TPanel *panel);
    void __fastcall MessageTimerTick(TObject *sender);
    void ShowMessage(String text);
    void CreateEditors();
    unsigned char EditMainCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor);
    unsigned char EditSubCell(TElTreeItem *item, int section, TElTreeInplaceEditor *&editor);
    void __fastcall SpinEditorKeyPress(TObject *sender, char &key);
    void __fastcall EditorKeyDown(TObject *sender, bool &handled);
    void __fastcall TextEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall SpinEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall ComboEditorValidate(TObject *sender, bool &accept, bool &default_op);
    void __fastcall TextEditorAfter(TObject *sender, bool &changed);
    void __fastcall SpinEditorAfter(TObject *sender, bool &changed);
    void __fastcall ComboEditorAfter(TObject *sender, bool &changed);
    void __fastcall SearchChange(TObject *sender);
    int Search(String filter, String text);
    void ShowView(DataGridView *view);
    void __fastcall ViewLabelMouseUp(TObject *sender, TMouseButton button,
    Set<Classes__1,0,6> shift, int x, int y);
    void __fastcall ViewMenuClick(TObject *sender);
    void __fastcall FilterLabelMouseUp(TObject *sender, TMouseButton button,
    Set<Classes__1,0,6> shift, int x, int y);
    void __fastcall FilterMenuClick(TObject *sender);
    void SelectionChange(TObject *sender, bool reset);
    void ShowInfo(TElTreeStringGrid *grid);
    void ApplyFilter(DataComboItem *filter);
    void ShowGfxInfo(TElTreeStringGrid *grid, DataGridCol *gfx_column, DataGridCol *id_column);
    bool DrawFrame(int index, int frame, char drawn);
    void ShowFrames(TElTreeStringGrid *grid, int index);
    void ShowGraphic(TElTreeStringGrid *grid, int index);
    void ShowIcon(TElTreeStringGrid *grid, int index);
    void ShowColumnText(TElTreeStringGrid *grid);
    void ToggleGfxViewer(TControl *button, TPanel *panel);
    void DestroyRow(TElTreeStringGrid *grid, int index);
    void InjectRow(TElTreeStringGrid *grid, int index);
    void CreateColumnOrder();
    void CopyRow();
    void CopyToClipboard(String text);
    void PasteRow();
    int DigitsOnly(String text);
    String HideZeroText(String text);
    void Unacquire();
    void Acquire();
    void ClearRowColors();
};

#endif
