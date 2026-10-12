#ifndef Ctrl_lawH
#define Ctrl_lawH

#include <Classes.hpp>

// The law npc's window: a menu with two requests, each with a text page whose link line opens the input box
// for it.
struct LawController
{
    char close_hovered;
    char back_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    String npc_name;
    int page;
    int reset_value;
    int link_line_index;
    int npc_session_id;
    char pad_0x24[1];
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *unused_lines;
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
    char pad_0x58[4];

    LawController();
    ~LawController();
    void AddText(String text, bool add_blank_line);
    void Open(int session_id);
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
    void ClearText();
};

#endif
