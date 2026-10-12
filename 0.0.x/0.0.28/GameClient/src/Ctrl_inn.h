#ifndef Ctrl_innH
#define Ctrl_innH

#include <Classes.hpp>

// The innkeeper's window: a menu to sleep or to see the citizenship page, and text pages to become a citizen,
// answering the innkeeper's questions, or to give citizenship up.
struct InnController
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
    int is_citizen;
    char pad_0x28[1];
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *unused_lines;
    String questions[3];
    String answers[3];
    int question_index;
    int sleep_request_cooldown;
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
    char pad_0x7c[4];

    InnController();
    ~InnController();
    void AddText(String text, bool add_blank_line);
    void Open(int session_id, int citizen_flag);
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
