#include <vcl.h>
#pragma hdrstop

#include "Ctrl_group.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#pragma package(smart_init)

#define GROUP_VISIBLE_ROWS 7

GroupController::GroupController()
{
    invite_cooldown = 0;
    list_request_cooldown = 0;
    is_leader = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_dirty = 0;
    list_dirty = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    Clear();
}

// True when the player is in the group.
bool GroupController::HasMember(int player_id)
{
    bool found = 0;
    vector<GroupMember>::iterator it;
    for (it = members.begin(); it != members.end(); it++)
    {
        if (player_id == it->player_id)
        {
            found = 1;
            break;
        }
    }
    return found;
}

// The member's leader flag, or -1 when the player is not in the group.
int GroupController::GetMemberLeaderFlag(int player_id)
{
    int leader_flag = -1;
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
    {
        if (player_id == it->player_id)
        {
            leader_flag = it->is_leader;
            break;
        }
    }
    return leader_flag;
}

// The member's name, or an empty text when the player is not in the group.
String GroupController::GetMemberName(int player_id)
{
    String name;
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
    {
        if (player_id == it->player_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

bool GroupController::IsListRequestCoolingDown()
{
    bool cooling_down = 0;
    if (list_request_cooldown > 0)
        cooling_down = 1;
    return cooling_down;
}

void GroupController::SetListRequestCooldown(int cooldown)
{
    list_request_cooldown = cooldown;
}

bool GroupController::IsInviteCoolingDown()
{
    bool cooling_down = 0;
    if (invite_cooldown > 0)
        cooling_down = 1;
    return cooling_down;
}

// Empties the group and scrolls back to the top.
void GroupController::Clear()
{
    int __slot1;
    int __slot2;
    members.clear();
    is_leader = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_dirty = 1;
    list_dirty = 1;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
}

void GroupController::SetInviteCooldown(int cooldown)
{
    invite_cooldown = cooldown;
}

int GroupController::GetRequesterId()
{
    return requester_id;
}

void GroupController::SetRequesterId(int player_id)
{
    requester_id = player_id;
}

// Scrolls back to the top.
GroupController::~GroupController()
{
    top_row = 0;
    scroll_thumb_offset = 0;
}

// Adds a member with its level, leader flag and health, and whether the own character may remove it.
void GroupController::AddMember(int player_id, String name, int level, int leader_flag, int health_percent,
    char removable)
{
    GroupMember handle(player_id, name, level, leader_flag, health_percent);
    handle.removable = removable;
    members.insert(members.end(), handle);
    OnMemberAdded();
    list_dirty = 1;
}

// Lets every member be removed.
void GroupController::MarkMembersRemovable()
{
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
        it->removable = 1;
    list_dirty = 1;
}

// Sets a member's health.
void GroupController::SetMemberHealth(int player_id, int health_percent)
{
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->health_percent = health_percent;
            list_dirty = 1;
            return;
        }
    }
}

// Takes a player out of the group.
void GroupController::RemoveMember(int player_id)
{
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
    {
        if (player_id == it->player_id)
        {
            members.erase(it);
            list_dirty = 1;
            break;
        }
    }
    list_dirty = 1;
}

void GroupController::OnMemberAdded()
{

}

int GroupController::GetTopRow()
{
    return top_row;
}

int GroupController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// Counts down the invite and list-request waits and repeats a held scroll arrow.
void GroupController::Tick()
{
    if (invite_cooldown > 0)
        invite_cooldown--;
    if (list_request_cooldown > 0)
        list_request_cooldown--;
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scroll_up_held)
            ScrollUp();
        if (scroll_down_held)
            ScrollDown();
    }
}

// Scrolls the members up one row and moves the scroll thumb to match.
void GroupController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)members.size() > GROUP_VISIBLE_ROWS && top_row > 0)
    {
        top_row--;
        scroll_fraction = (double)top_row / (double)(members.size() - 7);
        thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// Scrolls the members down one row and moves the scroll thumb to match.
void GroupController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)members.size() - 7 > top_row)
    {
        top_row++;
        scroll_fraction = (double)top_row / (double)(members.size() - 7);
        thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top row from its position.
void GroupController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_row;
    int __slot1;
    int __slot2;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 50)
            thumb_offset = 50;
        if (members.size() > GROUP_VISIBLE_ROWS)
        {
            thumb_fraction = thumb_offset / PANEL_THUMB_TRAVEL;
            scaled_row = (members.size() - 7.0) * thumb_fraction;
            int new_top_row = scaled_row;
            if (new_top_row != top_row)
                top_row = new_top_row;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// A click in the member column on a member the own character may remove asks the server to take it out; also
// the scroll bar.
void GroupController::MouseDown(int x, int y)
{
    if (x >= 440 && x <= 520)
    {
        int row = -1;
        if (y >= 350 && y <= 428)
            row = top_row + (y - 350) / 14;
        if (row >= 0 && (int)members.size() > row && members[row].removable)
        {
            MainForm->sfx->PlaySfx(3);
            MainForm->net_client->SendPacket(AC_DELETE, OP_GROUP,
                Client::EncodeNumber(MainForm->net_client, members[row].player_id, 2));
            list_dirty = 1;
        }
    }
    if (x >= 569 && x <= 585)
    {
        if (y >= scroll_thumb_offset + 366 && y <= scroll_thumb_offset + 381)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 350 && y <= 365)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
}

// Marks the member under the mouse in the member column.
void GroupController::UpdateMemberHover(int x, int y)
{
    int hovered_row = -1;
    int index = -1;
    if (x >= 440 && x <= 520 && y >= 350 && y <= 428)
        hovered_row = top_row + (y - 350) / 14;
    for (vector<GroupMember>::iterator it = members.begin(); it != members.end(); it++)
    {
        if (hovered_row == ++index)
        {
            if (!it->is_hovered)
            {
                it->is_hovered = true;
                list_dirty = 1;
            }
        }
        else if (it->is_hovered)
        {
            it->is_hovered = false;
            list_dirty = 1;
        }
    }
}

// Lights the scroll arrow under the mouse, lets go of an arrow the mouse has left, and drags a grabbed thumb.
void GroupController::UpdateScrollHover(int x, int y)
{
    char changed = 0;
    if (x >= 569 && x <= 585)
    {
        if (y >= 365 && y <= 432)
            DragScrollThumb(y);
        if (y >= 350 && y <= 365)
        {
            if (!scroll_up_hovered)
            {
                scroll_up_hovered = 1;
                changed = 1;
            }
        }
        else if (scroll_up_hovered)
        {
            scroll_up_hovered = 0;
            changed = 1;
            scroll_up_held = 0;
        }
        if (y >= 432 && y <= 447)
        {
            if (!scroll_down_hovered)
            {
                scroll_down_hovered = 1;
                changed = 1;
            }
        }
        else if (scroll_down_hovered)
        {
            scroll_down_hovered = 0;
            scroll_down_held = 0;
            changed = 1;
        }
    }
    else if (scroll_up_hovered || scroll_down_hovered)
    {
        scroll_up_hovered = 0;
        changed = 1;
        scroll_down_hovered = 0;
        scroll_up_held = 0;
        scroll_down_held = 0;
    }
    if (changed)
        scroll_dirty = 1;
}

// Lets go of the held scroll arrows and the grabbed thumb.
void GroupController::MouseUp()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

void GroupController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
    UpdateMemberHover(x, y);
}

DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(GroupController)

DEFINE_TEST_AND_CLEAR_LIST_DIRTY(GroupController)

void GroupController::MarkListDirty()
{
    list_dirty = 1;
}
