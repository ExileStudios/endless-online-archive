#ifndef Sys_editH
#define Sys_editH

// One text box: its place and size, how it filters and cases what is typed, its lines when multi-line, and its text.
struct Edit
{
    int x;
    int y;
    int width;
    int height;
    int extra_mode;
    int case_mode;
    char multi_line;
    int first_visible_line;
    int line_count;
    int visible_line_count;
    int pending_scroll;
    int scroll_offset;
    bool numeric;
    int min_value;
    int max_value;
    int char_filter;
    int max_length;
    char password;
    char border_visible;
    char background_visible;
    char editable;
    char focused;
    char dirty;
    bool change_notify;
    char visible;
    int border_color;
    int background_color;
    int caret_color;
    int text_color;
    int id;
    String label;
    String screen_group;
    String text;
    String line_buffer;

    Edit(int edit_id, int group, int left, int top, int box_width, int box_height);
    ~Edit();
    void AppendText(String typed_text);
    void AppendChar(char ch);
    void ClearText();
    void RemoveLastChar();
};

#endif
