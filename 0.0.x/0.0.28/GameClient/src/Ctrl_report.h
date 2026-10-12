#ifndef Ctrl_reportH
#define Ctrl_reportH

#include <Classes.hpp>
#define REPORT_MAX_LINKS 3

// The help window opened from the help button: lines of text, up to three of them links, with a scroll bar.
struct ReportController
{
    char close_hovered;
    char is_visible;
    int hovered_line;
    int window_x;
    int window_y;
    String title;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    int link_line_indices[REPORT_MAX_LINKS];
    TStringList *lines;
    TStringList *line_values;
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

    ReportController();
    ~ReportController();
    void AddText(String text, String right_text, bool add_blank_line);
    void Open(String window_title);
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
