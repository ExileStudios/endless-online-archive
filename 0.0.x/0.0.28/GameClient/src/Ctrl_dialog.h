#ifndef Ctrl_dialogH
#define Ctrl_dialogH

#include <Classes.hpp>
#define DIALOG_BUTTON_OK_FRAME 4
#define DIALOG_MAX_LINKS 10
#define QUEST_REQUEST_COOLDOWN_TICKS 16

// The quest dialog window: the pages of an npc's dialog with previous and next buttons, the reply links on the last
// page, and a list of the npc's other quests to switch to.
struct DialogController
{
    bool prev_button_hovered;
    bool next_button_hovered;
    int prev_button_kind;
    int next_button_kind;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    int request_cooldown;
    String npc_name;
    String quest_name;
    int dialog_echo_a;
    int quest_id;
    int dialog_echo_b;
    int npc_index;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    bool has_quest_list;
    bool showing_quest_list;
    TStringList *lines;
    TStringList *line_right_texts;
    TStringList *unused_lines;
    int page_index;
    int page_count;
    TStringList *pages;
    TStringList *quest_names;
    int quest_ids[10];
    TStringList *link_texts;
    int link_ids[DIALOG_MAX_LINKS];
    int link_line_indices[DIALOG_MAX_LINKS];
    char pad_0xd8[40];
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
    char pad_0x128[4];

    DialogController();
    ~DialogController();
    void AddText(String text, String right_text, bool add_blank_line);
    void ShowPage();
    void PreviousPage();
    void NextPage();
    void AddPage(String page_text);
    void AddLink(int link_id, String link_text);
    void Open(String first_quest_name);
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
