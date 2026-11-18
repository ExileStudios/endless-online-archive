#ifndef DatagridcolH
#define DatagridcolH

#include <ElTreeGrids.hpp>
#include <new>
#include <vector.h>

struct DataComboItem;
struct DataLinkItem;

// One column of a pub grid: its header, kind, limits, group, choices, links and variants.
struct DataGridCol
{
    int index;
    int text_index;
    char group;
    unsigned char kind;
    unsigned char alignment;
    String header_text;
    String caption_text;
    String hint_text;
    int min_value;
    int max_value;
    unsigned char text_format;
    short width;
    bool is_out_of_bounds;
    bool is_link_out_of_bounds;
    unsigned char largest_link_value;
    DataGridCol *parent;
    String sub_name;
    bool disabled;
    vector<DataComboItem *> combo_items;
    vector<DataLinkItem *> links;
    vector<DataGridCol *> sub_columns;

    DataGridCol(String column_name, String column_caption, short column_width, char column_kind, String column_hint,
    int column_min, int column_max, char column_group, char column_alignment, char column_format);
    ~DataGridCol();
    unsigned char ComboItemCount();
    DataComboItem * AddComboItem(int id, String text);
    DataComboItem * AddComboItemWithGroup(int id, String text, int first_group);
    DataComboItem * AddComboItemWithTwoGroups(int id, String text, int first_group, int second_group);
    DataComboItem * AddComboItemWithThreeGroups(int id, String text, int first_group, int second_group,
        int third_group);
    DataComboItem * AddComboItemWithFourGroups(int id, String text, int first_group, int second_group, int third_group,
        int fourth_group);
    DataComboItem * AddComboItemWithFiveGroups(int id, String text, int first_group, int second_group, int third_group,
        int fourth_group,
    int fifth_group);
    DataComboItem * AddComboItemWithSevenGroups(int id, String text, int first_group, int second_group, int third_group,
        int fourth_group,
    int fifth_group, int sixth_group, int seventh_group);
    DataComboItem * AddComboItemWithEightGroups(int id, String text, int first_group, int second_group, int third_group,
        int fourth_group,
    int fifth_group, int sixth_group, int seventh_group, int eighth_group);
    DataComboItem * AddComboItemWithNineGroups(int id, String text, int first_group, int second_group, int third_group,
        int fourth_group,
    int fifth_group, int sixth_group, int seventh_group, int eighth_group, int ninth_group);
    DataComboItem * GetComboItem(short index);
    DataComboItem * FindComboItem(String text);
    int ComboIndexOf(String text);
    int ParseOutOfBounds(String text);
    void WidenForComboItems(char wider);
    unsigned char LinkCount();
    DataLinkItem * AddLink(TElTreeStringGrid *grid, String column_name);
    DataLinkItem * GetLink(short index);
    int LinkedText(String id_text, unsigned char link_index);
    char SubColumnCount();
    DataGridCol * AddSubColumn(String name, String caption);
    DataGridCol * AddSubColumnWithHint(String name, String caption, String hint);
    DataGridCol * FindSubColumn(String name);
    String NumberedText(int number, String text, int count);
};

#endif
