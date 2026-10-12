#ifndef Ctrl_chatH
#define Ctrl_chatH

#include <Classes.hpp>
#define MAX_WRAP_PIECES 10

enum ChatIcon
{
    ChatIcon_Announce = 4,
    ChatIcon_Party = 11,
    ChatIcon_Admin = 12,
    ChatIcon_Guild = 19,
    ChatIcon_HelpRequest = 22
};
#define PANEL_THUMB_TRAVEL 50.0

// The chat panel: six tabs (two private, local, global, group, system), each with its lines, colors and scrolling.
struct ChatController
{
    TStringList *system_lines;
    TStringList *system_colors;
    TStringList *system_icons;
    TStringList *spare_line_list;
    TStringList *local_lines;
    TStringList *local_colors;
    TStringList *local_icons;
    TStringList *local_senders;
    TStringList *local_indents;
    TStringList *global_lines;
    TStringList *global_colors;
    TStringList *global_icons;
    TStringList *global_senders;
    TStringList *global_indents;
    TStringList *group_lines;
    TStringList *group_colors;
    TStringList *group_icons;
    TStringList *group_senders;
    TStringList *private_lines[3];
    TStringList *private_colors[2];
    TStringList *private_indents[2];
    char tab_has_unread[6];
    bool global_chat_open;
    bool auto_scroll;
    int active_tab;
    String private_name_1;
    String private_name_2;
    char scroll_up_hover;
    char scroll_down_hover;
    bool scrollbar_dirty;
    char text_dirty;
    int scroll_offsets[6];
    int scrollbar_positions[6];
    char scrolling_up;
    char scrolling_down;
    char is_dragging_thumb;
    int drag_grab_offset;
    int drunk_ticks;
    int scroll_repeat_delay;

    ChatController();
    ~ChatController();
    int FindPrivateTab(String name);
    void OpenPrivateTab(String name);
    void ClosePrivateTab(int tab);
    void ResetChat();
    int GetScrollOffset();
    int GetScrollbarPosition();
    void OnTimer();
    void ScrollUp();
    void ScrollDown();
    void DragScrollbar(int mouse_y);
    void AddSystemMessage(int icon, int color, String text, bool silent);
    void AddPrivateMessage(int tab_index, int color, String sender, String text, bool silent);
    void AddLocalMessage(int icon, int color, String sender, String text, bool silent);
    void AddGroupMessage(int icon, int color, String sender, String text, bool silent);
    void AddGlobalMessage(int icon, int color, String sender, String text, bool silent, bool skip_duplicates);
    void OnMouseClick(int mouse_x, int mouse_y, int button);
    void StopScrolling();
    void OnMouseMove(int mouse_x, int mouse_y);
    void UpdateScrollbarHover(int mouse_x, int mouse_y);
    bool TakeScrollbarDirty();
    bool TakeTextDirty();
    void Invalidate();
    void SetDrunkTime(int duration);
    void SyncGlobalSubscription();
    void SubscribeGlobalChat();
    void UnsubscribeGlobalChat();
    bool IsDrunk();
};

#endif
