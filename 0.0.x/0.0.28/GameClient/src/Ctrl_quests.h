#ifndef Ctrl_questsH
#define Ctrl_questsH

#include <Classes.hpp>

// The quest book window: the character's current quests, each with an icon, a step text and its progress, or the
// list of finished quests, with a scroll bar.
struct QuestController
{
    char close_hovered;
    bool history_button_hovered;
    char unused_cleared_flag;
    int showing_history;
    bool history_requested;
    char is_visible;
    int hovered_line;
    int window_x;
    int window_y;
    String title;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *line_icon_types;
    TStringList *line_detail_texts;
    TStringList *line_progress_texts;
    TStringList *quest_entries;
    TStringList *history_lines;
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
    char pad_0x60[4];

    QuestController();
    ~QuestController();
    void LoadQuestHistory(String reply_data, String separator);
    void LoadQuestList(String reply_data, String separator);
    void AddText(String icon_type, String text, String detail_text, String progress_text, bool add_blank_line);
    void Open();
    void Close();
    int GetScrollThumbOffset();
    int GetTopLine();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
    void ResetScroll();
};

#endif
