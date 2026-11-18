#ifndef ShopdataadapterH
#define ShopdataadapterH

// Keeps the shops in step with the shop grid's edits, and counts the trades and crafts of an item.
struct ShopDataAdapter
{
    char unused_bytes[16];

    ShopDataAdapter();
    ~ShopDataAdapter();
    int ItemTradeCount(int item_id);
    int ItemCraftCount(int item_id);
    void DeleteAt(int index);
    void InsertAt(int index);
    void CopyLists(String source_id, String target_id);
};

#endif
