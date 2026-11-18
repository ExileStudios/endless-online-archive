#ifndef Ctrl_bookH
#define Ctrl_bookH

#include <Classes.hpp>

#define BOOK_THUMB_TRAVEL 188.0

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
    String player_name;
    String home_name;
    String partner_name;
    String title;
    String class_name;
    String guild_name;
    String guild_rank_name;
    int icon_type;
    int frame_index;
    int admin_level;
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
    void Open(int frame_row, String name_text, String home_text, String partner_text, String title_text,
    String player_class_name, String guild_text, String guild_rank_text, int new_icon_type, int new_admin_level);
    void Close();
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
