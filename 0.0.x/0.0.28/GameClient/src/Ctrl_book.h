#ifndef Ctrl_bookH
#define Ctrl_bookH

#include <Classes.hpp>

// The book window: a player's info lines and class with a scrolling list of entries.
struct BookController
{
    char close_hovered;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char is_visible;
    int window_x;
    int window_y;
    TStringList *lines;
    String info_line_1;
    String info_line_2;
    String info_line_4;
    String info_line_5;
    String class_name;
    String info_line_7;
    String info_line_8;
    int icon_type;
    int frame_index;
    int extra_value;
    int drag_mode;
    int unused_index;
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    int scroll_repeat_delay;
    char is_dragging;
    int drag_offset_x;
    int drag_offset_y;
    char thumb_grabbed;
    int thumb_grab_offset;
    char unused_flag;

    BookController();
    ~BookController();
    void Open(int frame_byte, String line_1_text, String line_2_text, String line_4_text, String line_5_text,
        String player_class_name, String line_7_text, String line_8_text, int new_icon_type, int unused_value);
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    int GetScrollThumbOffset();
    int GetTopLine();
    void MouseUp();
};

#endif
