#ifndef Gfx_mapitemsH
#define Gfx_mapitemsH

#include <vector.h>

#include "Gfx_mapitem.h"
#include "Gfx_useritem.h"
struct GraphicSize;

// The item pictures: one list for items lying on the map and one for the inventory and the game windows.
// Both load from the item library, one entry of each list per call, and are freed when unused.
struct ItemGraphics
{
    vector<ItemWrapper> items;
    int last_index;
    int pending_count;
    int queue_drained;
    char pad_0x2c[4];
    vector<SubItemWrapper> sub_items;
    int sub_item_last_index;
    int sub_item_pending_count;
    int sub_items_drained;
    HMODULE library;
    char library_open;
    char keep_library_open;
    char pad_0x62[6];

    ItemGraphics(char preload);
    ~ItemGraphics();
    void ReopenLibrary();
    void AgeGraphics();
    void AgeSubItemGraphics();
    void LoadPendingGraphics();
    bool PurgeUnusedGraphics();
    void ReleaseGraphics();
    void ReleaseSubItemGraphics();
    bool RequestGraphic(int graphic_id);
    bool RequestSubItemGraphic(int graphic_id);
    ItemWrapper GetGraphic(int graphic_id);
    SubItemWrapper GetSubItemGraphic(int graphic_id);
    void QueueGraphic(int graphic_id);
    void QueueSubItemGraphic(int graphic_id);
    unsigned int GetGraphicCount();
    unsigned int GetSubItemGraphicCount();
    GraphicSize GetGraphicSize(int graphic_id);
};

#endif
