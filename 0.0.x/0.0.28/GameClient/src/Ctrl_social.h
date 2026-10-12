#ifndef Ctrl_socialH
#define Ctrl_socialH

#include <Classes.hpp>

// The friend and ignore lists, kept in config/friends.ini and config/ignore.ini, and the window that shows them.
struct SocialController
{
    bool close_button_hover;
    bool add_button_hover;
    char is_open;
    int hover_row;
    int window_x;
    int window_y;
    String title;
    int list_kind;
    String unused_text_at_24;
    char pad_0x1c[1];
    char scroll_up_hover;
    char scroll_down_hover;
    bool unread_flag_at_31;
    TStringList *ignored_names;
    TStringList *friend_names;
    TStringList *online_names;
    bool scroll_moved_flag;
    int unread_int_at_48;
    int scroll_offset;
    int scrollbar_position;
    char scrolling_up;
    char scrolling_down;
    char is_dragging_thumb;
    int thumb_grab_offset;
    char is_dragging_window;
    int drag_offset_x;
    int drag_offset_y;
    int scroll_repeat_delay;
    char pad_0x54[4];

    SocialController();
    ~SocialController();
    void OpenWindow();
    void CloseWindow();
    void LoadIgnoreList();
    void LoadFriendList();
    void ToggleIgnore(String name);
    void AddIgnore(String name);
    void ToggleFriend(String name);
    void AddFriend(String name);
    void ClearOnlineNames();
    void AddOnlineName(String name);
    bool IsOnline(String name);
    void SaveIgnoreList();
    void SaveFriendList();
    bool IsIgnored(String name);
    bool IsFriend(String name);
    static String UnscrambleString(SocialController *self, int divisor, unsigned char *data_begin,
        unsigned char *data_end);
    void OnTimer();
    int GetScrollbarPosition();
    int GetScrollOffset();
    void OnMouseClick(int mouse_x, int mouse_y, int button);
    void OnMouseMove(int mouse_x, int mouse_y);
    void UpdateScrollbarHover(int mouse_x, int mouse_y);
    void DragScrollbar(int mouse_y);
    void ScrollUp();
    void ScrollDown();
    void StopDragging();
};

#endif
