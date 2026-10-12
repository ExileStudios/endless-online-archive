#ifndef Ctrl_guildH
#define Ctrl_guildH

#include <Classes.hpp>

enum GuildPage
{
    GuildPage_Main = 0,
    GuildPage_Membership = 1,
    GuildPage_Manage = 2,
    GuildPage_Lookup = 3,
    GuildPage_Join = 4,
    GuildPage_Leave = 5,
    GuildPage_Create = 6,
    GuildPage_Info = 7,
    GuildPage_Description = 8,
    GuildPage_RemoveMember = 9,
    GuildPage_Disband = 10,
    GuildPage_Bank = 11,
    GuildPage_Rank = 12,
    GuildPage_MemberList = 15
};
#define NPC_TEXT_LINE_HEIGHT 16

// The guild master's window: menu pages for guild information, joining, creating and managing a guild, and text
// pages whose link line starts the chosen request, with a scroll bar for the text.
struct GuildController
{
    char close_hovered;
    char back_hovered;
    char is_visible;
    int hovered_row;
    int window_x;
    int window_y;
    String title;
    int page;
    int guild_bank_gold;
    int link_line_index;
    int npc_session_id;
    char pad_0x24[1];
    char scroll_up_hovered;
    char scroll_down_hovered;
    char unused_flag;
    TStringList *lines;
    TStringList *member_rank_titles;
    String new_guild_tag;
    String new_guild_name;
    String unused_text_a;
    String unused_text_b;
    String new_guild_description;
    int request_player_id;
    String target_member_name;
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
    char pad_0x74[4];

    GuildController();
    ~GuildController();
    void AddText(String text, bool add_blank_line);
    void Open(String window_title, int session_id);
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

// Moves a dragged window, kept on screen, and tracks the hovered line and the close button.
// Shared body: a class defines its MouseMove with DEFINE_MOUSE_MOVE(TheClass); each class has its own
// UpdateScrollHover, is_dragging, window_x, drag_offset_x, window_y, drag_offset_y, lines, hovered_line, close_hovered.
#define DEFINE_MOUSE_MOVE(Class) \
void Class::MouseMove(int x, int y) \
{ \
    int line; \
    int local; \
    UpdateScrollHover(x, y); \
    if (is_dragging) \
    { \
        window_x = x - drag_offset_x; \
        window_y = y - drag_offset_y; \
        if (window_x <= 10) \
            window_x = 10; \
        if (window_y <= 8) \
            window_y = 8; \
        if (window_x >= 346) \
            window_x = 346; \
        if (window_y >= 19) \
            window_y = 19; \
        MainForm->redraw_needed = true; \
    } \
    if (!(y <= window_y + 43 || y >= window_y + 243 || x <= window_x + 17 || x >= window_x + 252 || is_dragging)) \
    { \
        /* The hovered line is a row on screen, not a line of the text */ \
        line = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT; \
        if (line > 11) \
            line = -1; \
        if (lines->Count <= line) \
            line = -1; \
        if (line != hovered_line) \
        { \
            hovered_line = line; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else \
    { \
        if (hovered_line >= 0) \
        { \
            hovered_line = -1; \
            MainForm->redraw_needed = true; \
        } \
    } \
    if (!(x < window_x + 92 || x > window_x + 182 || y < window_y + 253 || y > window_y + 279)) \
    { \
        if (!close_hovered) \
        { \
            close_hovered = 1; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else \
    { \
        if (close_hovered) \
        { \
            close_hovered = 0; \
            MainForm->redraw_needed = true; \
        } \
    } \
}

#endif
