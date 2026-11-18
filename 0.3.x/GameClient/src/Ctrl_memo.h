#ifndef Ctrl_memoH
#define Ctrl_memoH

#include <Classes.hpp>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct ItemValue;
struct NpcValue;
struct Tooltip;

// The memo window: a scrolling page of lines, each with an optional value on the right, used for a character's
// stats, an inventory and locker listing, and notices.
struct MemoController
{
    char close_hovered;
    char is_visible;
    unsigned char memo_kind;
    int preview_graphic;
    int hovered_line;
    int window_x;
    int window_y;
    String title;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *line_values;
    TStringList *item_entries;
    Tooltip *tooltip;
    TDx_DrawSurface *preview_surface;
    TRect preview_rect;
    char scroll_changed;
    int unused_value;
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int scroll_repeat_delay;
    char unused_bytes[4];
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MemoController();
    ~MemoController();
    void AddItemList(String item_data, int record_size, int amount_size);
    void AddCharacterStats(String stats_data);
    void AddText(String text, String right_text, bool add_blank_line);
    void Show();
    void ShowItem(ItemValue *item);
    void ShowNpc(NpcValue *npc);
    String BehaviourName(int behaviour);
    void Open(String heading);
    void Close();
    void Tick();
    int GetScrollThumbOffset();
    int GetTopLine();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
};

#endif
