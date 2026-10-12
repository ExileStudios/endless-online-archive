#ifndef Ctrl_learnH
#define Ctrl_learnH

#include <Classes.hpp>
#include <vector.h>

#include "Learnitem.h"

enum LearnPage
{
    LearnPage_Menu = 0,
    LearnPage_SkillList = 1,
    LearnPage_SkillInfo = 2,
    LearnPage_Reset = 3,
    LearnPage_Unknown15 = 15
};

// The skill master's window: a menu page, the list of skills on offer, a skill's details and an info text page,
// with one scroll bar for the list or the text.
struct LearnController
{
    vector<LearnItem> shown_skills;
    vector<LearnItem> offered_skills;
    vector<LearnItem> unused_items;
    char close_hovered;
    char back_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    String npc_name;
    int page;
    int npc_session_id;
    char pad_0x7c[4];
    int link_line_index;
    int selected_skill_id;
    char pad_0x88[8];
    char unused_cleared_flag;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *line_right_texts;
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
    char pad_0xc4[4];

    LearnController();
    ~LearnController();
    void AddText(String text, bool add_blank_line);
    void RemoveSkill(int skill_id);
    void Open(String npc, int session_id);
    void Close();
    void AddSkill(int add_mode, int skill_id, int required_level, int required_class, int price,
        int first_required_skill, int second_required_skill, int third_required_skill, int fourth_required_skill,
        int required_str, int required_wis, int required_int, int required_agi, int required_con, int required_cha);
    void Tick();
    int GetScrollThumbOffset();
    int GetTopLine();
    void ClearText();
    void MouseDown(int x, int y, int button);
    void MouseMove(int x, int y);
    void UpdateScrollHover(int x, int y);
    void DragScrollThumb(int y);
    void ScrollUp();
    void ScrollDown();
    void MouseUp();
};

#endif
