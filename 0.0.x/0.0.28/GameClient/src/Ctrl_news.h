#ifndef Ctrl_newsH
#define Ctrl_newsH

#include <Classes.hpp>

// The news panel: the news items from the server's welcome reply, with a scroll bar and a blinking hint.
struct NewslistController
{
    TStringList *lines;
    TStringList *line_markers;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char scroll_dirty;
    char text_dirty;
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    int scroll_repeat_delay;
    bool scroll_hint_active;
    int scroll_hint_frame;

    NewslistController();
    ~NewslistController();
    int GetLineCount();
    void Clear();
    int GetTopLine();
    int GetScrollThumbOffset();
    void MouseDown(int x, int y);
    void AddNewsItem(String text);
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
    void StartScrollHint();
    bool IsScrollHintVisible();
};

#endif
