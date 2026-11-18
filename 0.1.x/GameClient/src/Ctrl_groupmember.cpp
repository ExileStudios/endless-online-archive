#include <vcl.h>
#pragma hdrstop

#include "Ctrl_groupmember.h"

#pragma package(smart_init)

GroupMember::GroupMember(int id, String member_name, int member_level, int leader_flag, int initial_health_percent)
{
    player_id = id;
    name = member_name;
    level = member_level;
    is_leader = leader_flag;
    health_percent = initial_health_percent;
    removable = false;
    is_hovered = false;
}

GroupMember::~GroupMember()
{
}
