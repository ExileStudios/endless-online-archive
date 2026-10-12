#ifndef Ctrl_playerlistH
#define Ctrl_playerlistH

#include <Classes.hpp>

enum PlayerFilter
{
    PlayerFilter_All = 0,
    PlayerFilter_Friends = 1,
    PlayerFilter_Marked = 2,
    PlayerFilter_Guild = 3
};

// The online list panel: every player the server listed, and the shown list after the filter (all, friends, by
// icon, or the own guild), each name with its icon, title, guild tag and class.
struct PlayerlistController
{
    TStringList *shown_record_values;
    TStringList *shown_icon_types;
    TStringList *shown_names;
    TStringList *shown_second_texts;
    TStringList *shown_guild_tags;
    TStringList *shown_class_names;
    TStringList *all_record_values;
    TStringList *all_icon_types;
    TStringList *all_names;
    TStringList *all_second_texts;
    TStringList *all_guild_tags;
    TStringList *all_class_names;
    char scroll_up_hovered;
    char scroll_down_hovered;
    char scroll_dirty;
    char list_dirty;
    int top_line;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    int scroll_repeat_delay;
    int filter_mode;

    PlayerlistController();
    ~PlayerlistController();
    void CycleFilter();
    void SetFilter(int mode);
    int GetFilter();
    void Clear();
    void ApplyFilter();
    void AddPlayer(String record_value, int icon_type, String name, String second_text, String guild_tag,
        String class_name);
    int GetTopLine();
    int GetScrollThumbOffset();
    void Tick();
    void ScrollUp();
    void ScrollDown();
    void DragScrollThumb(int y);
    void MouseDown(int x, int y, int button);
    void UpdateScrollHover(int x, int y);
    void MouseUp();
    void MouseMove(int x, int y);
    bool TestAndClearScrollDirty();
    bool TestAndClearListDirty();
    static String Capitalize(PlayerlistController *self, String text);
    void MarkListDirty();
};

#endif
