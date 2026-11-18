#ifndef Ctrl_hotkeysH
#define Ctrl_hotkeysH

struct Tooltip;

#include "Itemhotkey.h"

struct HotkeysController
{
    bool close_hovered;
    HotkeyItem slots[16];
    Tooltip *tooltip;
    bool is_visible;
    int window_x;
    int window_y;
    int slot_width;
    int slot_height;
    int left_grid_x;
    int right_grid_x;
    int grid_y;
    int last_column;
    int last_row;
    bool is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int hovered_column;
    int hovered_row;
    char unused_bytes[4];

    HotkeysController();
    ~HotkeysController();
    void DropItem(int item_id, int x, int y);
    void DropSpell(int spell_id, int x, int y);
    void Open();
    void Close();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateTooltip(int x, int y);
    void HitTest(int x, int y);
    bool IsHotkeyItem(int item_id);
    void UseHotkey(int column, int row);
    void MouseUp();
    void LoadHotkeys();
    int ParseNumber(String text);
    void SaveHotkeys();
};

#endif
