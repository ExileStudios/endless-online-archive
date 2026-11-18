#ifndef Ctrl_groupmemberH
#define Ctrl_groupmemberH

// One member of the own group, as the group list shows them.
struct GroupMember
{
    int player_id;
    String name;
    int level;
    int is_leader;
    int health_percent;
    char removable;
    bool is_hovered;

    GroupMember(int id, String member_name, int member_level, int leader_flag, int initial_health_percent);
    ~GroupMember();
};

#endif
