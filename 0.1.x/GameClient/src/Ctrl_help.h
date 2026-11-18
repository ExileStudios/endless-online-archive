#ifndef Ctrl_helpH
#define Ctrl_helpH

#include <Classes.hpp>

// The help panel: a page read from a help file, as lines with a bold flag, an icon flag and an optional link to
// another page, with a scroll bar.
struct HelpController
{
    TStringList *lines;
    TStringList *link_targets;
    TStringList *line_flags;
    TStringList *line_icons;
    String clicked_line_text;
    char scroll_up_hovered;
    char scroll_down_hovered;
    TStringList *file_lines;
    char scroll_dirty;
    char is_dirty;
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    int scroll_repeat_delay;

    HelpController();
    ~HelpController();
    void Clear();
    int GetTopLine();
    int GetScrollThumbOffset();
    void MouseDown(int x, int y);
    void LoadPage(String page_name);
    void AddText(String text, String link_target, char line_flag, char hide_icon);
    void Tick();
    void ScrollUp();
    void ScrollDown();
    void DragScrollThumb(int y);
    void MouseUp();
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    bool TestAndClearScrollDirty();
    bool TestAndClearTextDirty();
    void MarkTextDirty();
};

#endif
