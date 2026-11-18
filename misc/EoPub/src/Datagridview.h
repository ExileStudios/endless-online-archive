#ifndef DatagridviewH
#define DatagridviewH

// A named view of a grid: the groups of columns it shows.
struct DataGridView
{
    String name;
    char groups[10];
    unsigned char group_count;

    DataGridView(String view_name);
    ~DataGridView();
    void AddGroup(int group);
};

#endif
