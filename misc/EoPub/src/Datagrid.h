#ifndef DatagridH
#define DatagridH

enum ColumnKind
{
    ColumnKind_Number = 10,
    ColumnKind_Text = 11,
    ColumnKind_Combo = 12,
    ColumnKind_Link = 13,
    ColumnKind_LinkSource = 14,
    ColumnKind_Graphic = 15,
    ColumnKind_Index = 16,
    ColumnKind_SmartCombo = 17,
    ColumnKind_PaperdollGraphic = 18,
    ColumnKind_FirstEditorForm = 30,
    ColumnKind_NpcDrops = 30,
    ColumnKind_NpcChats = 31,
    ColumnKind_ShopTrades = 32,
    ColumnKind_ShopCrafts = 33,
    ColumnKind_TrainerSkills = 34,
    ColumnKind_InnQuiz = 35
};

enum ColumnAlign
{
    ColumnAlign_Left = 0,
    ColumnAlign_Center = 1,
    ColumnAlign_Right = 2
};

enum ColumnFormat
{
    ColumnFormat_None = 0,
    ColumnFormat_Capitalized = 1,
    ColumnFormat_Upper = 2,
    ColumnFormat_Lower = 3,
    ColumnFormat_KeepZero = 4
};

#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <new>
#include <vector.h>

struct DataGridCol;
struct DataGridView;

// The columns and views of one pub's grid, and the copying of a row's values to and from its details.
struct DataGrid
{
    bool has_out_of_bounds;
    bool has_link_out_of_bounds;
    TElTreeItem *edit_row;
    TElTreeItem *edit_detail_item;
    TElTreeItem *store_row;
    vector<DataGridCol *> columns;
    vector<DataGridView *> views;
    TElTreeStringGrid *row_grid;
    TElTreeStringGrid *detail_grid;
    char view_all;
    String table_name;
    String index_column_name;
    DataGridCol *index_column;

    DataGrid(String name, char show_all, String index_name);
    ~DataGrid();
    DataGridCol * GetIndexColumn();
    void SetGrids(TElTreeStringGrid *main_grid, TElTreeStringGrid *sub_grid);
    DataGridCol * AddColumn(String column_name, String column_caption, short column_width, char column_kind,
    String column_hint, int column_min, int column_max, char column_group, char column_alignment, char column_format);
    void BuildHeader(TElTreeStringGrid *grid);
    void AddEmptyRow(TElTreeStringGrid *grid);
    String ZeroIfEmpty(String text);
    String ColumnWidth(DataGridCol *column);
    String CellText(String column_name, int value);
    String CellTextOrDash(String column_name, int value);
    int ColumnIndexOf(String name);
    DataGridCol * FindColumn(String name);
    DataGridCol * GetColumn(int index);
    int ColumnCount();
    DataGridView * AddView(String name);
    DataGridView * GetView(int index);
    int ViewCount();
    bool ShowRowDetails();
    bool RefreshRowDetails();
    bool StoreDetailValue();
    String VisibleColumnNames();
    DataGridCol * GetKeyColumn();
    int DigitsOnly(String text);
};

#endif
