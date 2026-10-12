#ifndef Ctrl_groupH
#define Ctrl_groupH

#include <vector.h>

#include "Ctrl_groupmember.h"

// The group panel: the group's members with level, health and leader mark, the waits between invites and between
// list requests, and the player whose invite is waiting.
struct GroupController
{
    char scroll_up_hovered;
    char scroll_down_hovered;
    char pad_0x2[6];
    vector<GroupMember> members;
    char is_leader;
    int requester_id;
    int invite_cooldown;
    int list_request_cooldown;
    char scroll_dirty;
    char list_dirty;
    int top_row;
    int scroll_thumb_offset;
    char scroll_up_held;
    char scroll_down_held;
    char thumb_grabbed;
    int thumb_grab_offset;
    int scroll_repeat_delay;

    GroupController();
    ~GroupController();
    bool HasMember(int player_id);
    int GetMemberLeaderFlag(int player_id);
    String GetMemberName(int player_id);
    bool IsListRequestCoolingDown();
    void SetListRequestCooldown(int cooldown);
    bool IsInviteCoolingDown();
    void Clear();
    void SetInviteCooldown(int cooldown);
    int GetRequesterId();
    void SetRequesterId(int player_id);
    void AddMember(int player_id, String name, int level, int leader_flag, int health_percent, char removable);
    void MarkMembersRemovable();
    void SetMemberHealth(int player_id, int health_percent);
    void RemoveMember(int player_id);
    void OnMemberAdded();
    int GetTopRow();
    int GetScrollThumbOffset();
    void Tick();
    void ScrollUp();
    void ScrollDown();
    void DragScrollThumb(int y);
    void MouseDown(int x, int y);
    void UpdateMemberHover(int x, int y);
    void UpdateScrollHover(int x, int y);
    void MouseUp();
    void MouseMove(int x, int y);
    bool TestAndClearScrollDirty();
    bool TestAndClearListDirty();
    void MarkListDirty();
};

#endif
