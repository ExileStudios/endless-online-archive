#ifndef Ctrl_tradeH
#define Ctrl_tradeH

#include <vector.h>
#define NPC_THUMB_TRAVEL 152.0

#include "Iteminventory.h"

// The trade window: both players' offers in two scrolling lists, each side's status text, and the wait before the
// agree button works again.
struct TradeController
{
    vector<InventoryItem> partner_items;
    vector<InventoryItem> own_items;
    bool agree_hovered;
    bool cancel_hovered;
    char is_visible;
    int unused_index;
    int hovered_row;
    int window_x;
    int window_y;
    String own_name;
    String partner_name;
    String own_status_text;
    String partner_status_text;
    int own_player_id;
    int partner_player_id;
    int partner_removal_count;
    bool removal_warning_armed;
    bool own_agreed;
    bool trade_locked;
    bool partner_scroll_up_hovered;
    bool partner_scroll_down_hovered;
    bool own_scroll_up_hovered;
    bool own_scroll_down_hovered;
    char unused_flag;
    char scroll_changed;
    int accept_delay;
    int partner_top_row;
    int own_top_row;
    int partner_scroll_thumb_offset;
    int own_scroll_thumb_offset;
    bool partner_scroll_up_held;
    bool partner_scroll_down_held;
    bool own_scroll_up_held;
    bool own_scroll_down_held;
    bool partner_thumb_grabbed;
    bool own_thumb_grabbed;
    int partner_thumb_grab_offset;
    int own_thumb_grab_offset;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    int partner_scroll_repeat_delay;
    int own_scroll_repeat_delay;
    char pad_0xb4[4];

    TradeController();
    ~TradeController();
    void AddItem(int side, int item_id, int amount);
    void SetAcceptDelay(int delay_ticks);
    bool IsAcceptDelayed();
    void Tick();
    void Open(String my_name, String their_name, int my_player_id, int their_player_id);
    void Close();
    int GetPartnerScrollThumbOffset();
    int GetOwnScrollThumbOffset();
    int GetPartnerTopRow();
    int GetOwnTopRow();
    void ResetScroll();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragPartnerScrollThumb(int y);
    void DragOwnScrollThumb(int y);
    void ScrollPartnerUp();
    void ScrollPartnerDown();
    void ScrollOwnUp();
    void ScrollOwnDown();
    void MouseUp();
};

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of a text page from its position.
// Shared body: a class defines its DragScrollThumb with DEFINE_DRAG_SCROLL_THUMB(TheClass); each class has its own
// thumb_grabbed, thumb_grab_offset, window_y, page, lines, top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_DRAG_SCROLL_THUMB(Class) \
void Class::DragScrollThumb(int y) \
{ \
    double thumb_fraction; \
    double scaled_line; \
    int __slot1; \
    if (thumb_grabbed) \
    { \
        int thumb_offset = y - thumb_grab_offset - window_y; \
        if (thumb_offset < 0) \
            thumb_offset = 0; \
        if (thumb_offset > 152) \
            thumb_offset = 152; \
        if (page >= 4) \
        { \
            int line_count = lines->Count; \
            if (line_count > 12) \
            { \
                thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL; \
                scaled_line = (line_count - 12.0) * thumb_fraction; \
                int new_top_line = scaled_line; \
                if (new_top_line != top_line) \
                    top_line = new_top_line; \
            } \
        } \
        scroll_thumb_offset = thumb_offset; \
        scroll_changed = 1; \
        MainForm->redraw_needed = true; \
    } \
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
// Shared body: a class defines its DragScrollThumb with DEFINE_MEMO_DRAG_SCROLL_THUMB(TheClass); each class has its
// own thumb_grabbed, thumb_grab_offset, window_y, lines, top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_MEMO_DRAG_SCROLL_THUMB(Class) \
void Class::DragScrollThumb(int y) \
{ \
    double thumb_fraction; \
    double scaled_line; \
    int __slot1; \
    if (thumb_grabbed) \
    { \
        int thumb_offset = y - thumb_grab_offset - window_y; \
        if (thumb_offset < 0) \
            thumb_offset = 0; \
        if (thumb_offset > 152) \
            thumb_offset = 152; \
        int line_count = lines->Count; \
        if (line_count > 12) \
        { \
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL; \
            scaled_line = (line_count - 12.0) * thumb_fraction; \
            int new_top_line = scaled_line; \
            if (new_top_line != top_line) \
                top_line = new_top_line; \
        } \
        scroll_thumb_offset = thumb_offset; \
        scroll_changed = 1; \
        MainForm->redraw_needed = true; \
    } \
}

// Scrolls a text page down one line and moves the scroll thumb to match.
// Shared body: a class defines its ScrollDown with DEFINE_SCROLL_DOWN(TheClass); each class has its own page, lines,
// top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_SCROLL_DOWN(Class) \
void Class::ScrollDown() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    if (page >= 4) \
    { \
        int line_count = lines->Count; \
        if (line_count - 12 > top_line) \
        { \
            top_line++; \
            scroll_fraction = (double)top_line / (double)(line_count - 12); \
            thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction; \
            scroll_thumb_offset = thumb_pos; \
            scroll_changed = 1; \
            MainForm->redraw_needed = true; \
        } \
    } \
}

// Scrolls a text page up one line and moves the scroll thumb to match.
// Shared body: a class defines its ScrollUp with DEFINE_SCROLL_UP(TheClass); each class has its own page, lines,
// top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_SCROLL_UP(Class) \
void Class::ScrollUp() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    if (page >= 4) \
    { \
        int line_count = lines->Count; \
        if (line_count > 12 && top_line > 0) \
        { \
            top_line--; \
            scroll_fraction = (double)top_line / (double)(line_count - 12); \
            thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction; \
            scroll_thumb_offset = thumb_pos; \
            scroll_changed = 1; \
            MainForm->redraw_needed = true; \
        } \
    } \
}

// Shared body: a class defines its ScrollDown with DEFINE_MEMO_SCROLL_DOWN(TheClass); each class has its own lines,
// top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_MEMO_SCROLL_DOWN(Class) \
void Class::ScrollDown() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    int result = lines->Count; \
    if (result - 12 > top_line) \
    { \
        top_line++; \
        scroll_fraction = (double)top_line / (double)(result - 12); \
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction; \
        scroll_thumb_offset = thumb_pos; \
        scroll_changed = 1; \
        MainForm->redraw_needed = true; \
    } \
}

// Shared body: a class defines its ScrollUp with DEFINE_MEMO_SCROLL_UP(TheClass); each class has its own lines,
// top_line, scroll_thumb_offset, scroll_changed.
#define DEFINE_MEMO_SCROLL_UP(Class) \
void Class::ScrollUp() \
{ \
    double scroll_fraction; \
    double thumb_pos; \
    int line_count = lines->Count; \
    if (line_count > 12 && top_line > 0) \
    { \
        top_line--; \
        scroll_fraction = (double)top_line / (double)(line_count - 12); \
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction; \
        scroll_thumb_offset = thumb_pos; \
        scroll_changed = 1; \
        MainForm->redraw_needed = true; \
    } \
}

#endif
