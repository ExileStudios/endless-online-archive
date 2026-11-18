#ifndef DatacomboitemH
#define DatacomboitemH

// One choice of a list column: its number, its text and the groups of columns it shows.
struct DataComboItem
{
    int index;
    int id;
    String text;
    int max_value;
    unsigned char groups[1];
    char second_group;
    char third_group;
    char fourth_group;
    char fifth_group;
    char sixth_group;
    char seventh_group;
    char eighth_group;
    char ninth_group;
    char unused_bytes[1];
    unsigned char group_count;

    DataComboItem(int item_index);
    ~DataComboItem();
    bool HasGroup(int group);
};

#endif
