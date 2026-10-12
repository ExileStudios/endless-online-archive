#ifndef Ctrl_inventoryH
#define Ctrl_inventoryH

#include <vector.h>

#include "Iteminventory.h"
#define ITEM_SIZE_MAX_NARROW 3

// The inventory grid: the items and their cells, dragging between cells and from the paperdoll, the drop and junk
// buttons, and the layout saved in the registry.
struct InventoryController
{
    bool left_button_hovered;
    bool right_button_hovered;
    bool paperdoll_button_hovered;
    char pad_0x3[5];
    vector<InventoryItem> items;
    vector<InventoryItem> layout_backup;
    vector<InventoryItem> trial_backup;
    vector<InventoryItem> loaded_items;
    vector<InventoryItem> saved_slots;
    String hover_text;
    int hover_item_special;
    int hover_item_size;
    int hover_item_id;
    int hover_col;
    int hover_row;
    int hover_col_end;
    int hover_row_end;
    char has_hover;
    bool drop_blocked;
    int hover_text_width;
    int pending_use_item_id;
    int drag_x;
    int drag_y;
    int drag_graphic;
    int drag_item_size;
    int drag_item_id;
    char is_dragging;
    bool dragging_from_paperdoll;
    int paperdoll_slot_graphic;
    int paperdoll_sub_slot;
    char drag_started;
    int last_mouse_x;
    int last_mouse_y;
    int drag_delay_ticks;
    bool input_enabled;
    char pad_0x105[23];
    bool hover_dirty;
    bool items_dirty;
    char drag_dirty;
    char pad_0x11f[5];
    int click_ticks;
    int click_col;
    int click_row;
    int hover_message_delay;
    int change_item_ids[20];
    int change_cols[20];
    int change_rows[20];
    int change_count;
    bool changes_overflowed;
    char pad_0x229[7];

    InventoryController();
    ~InventoryController();
    void Tick();
    void SuppressHoverMessage();
    void Clear();
    char TestAddItem(int item_id);
    char CanFitTradeItems();
    bool TestAndClearHoverDirty();
    bool TestAndClearItemsDirty();
    bool TestAndClearDragDirty();
    void MarkHoverDirty();
    void MarkItemsDirty();
    void MarkDragDirty();
    void UseItem(int item_id);
    void MouseMove(int x, int y);
    void UpdateItemHover(int x, int y);
    void MouseDown(int x, int y, int button);
    void CancelDrag();
    void BackupLayout();
    void RestoreLayoutBackup();
    void RevertDragLayout();
    void MouseUp();
    bool UpdateButtonHover(int x, int y);
    void ResetLayoutChanges();
    void RecordLayoutChange(int item_id, int col, int row);
    void SaveLayout();
    void SaveLayoutChanges();
    char HasKey(int key_id);
    char HasRoomForItem(int item_id);
    bool AddItem(int item_id, int amount);
    char RemoveItem(int item_id, int amount);
    int GetTotalWeight();
    void SetItemAmount(int item_id, int amount);
    int GetItemAmount(int item_id);
    void AddLoadedItem(int item_id, int amount);
    void PlaceLoadedItems();
    void PlaceRemainingItems();
};

#endif
