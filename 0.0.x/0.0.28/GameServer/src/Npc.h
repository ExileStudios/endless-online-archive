#ifndef NpcH
#define NpcH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include "Constants.h"



struct Npc;

// An npc standing on a map.
struct Npc
{
    unsigned short index;
    unsigned short id;
    unsigned short x;
    unsigned short y;
    unsigned short direction;
    int min_damage;
    int max_damage;
    int armor;
    int accuracy;
    int evade;
    int hp;
    int max_hp;
    int hp_regen;
    short hp_percent;
    int element_weakness;
    short element_weakness_damage_table[ELEMENT_COUNT];
    bool alive;
    int base_act_ticks;
    TTimeStamp death_stamp;
    unsigned short base_spawn_time;
    unsigned short spawn_time;
    unsigned short spawn_x;
    unsigned short spawn_y;
    unsigned short drop_item_id;
    unsigned short drop_amount;
    int act_ticks;
    unsigned short act_counter;
    unsigned short move_cooldown;
    unsigned short facing;
    bool pos_pending;
    String pos_buffer;
    bool talk_pending;
    String talk_buffer;
    bool attack_pending;
    String attack_buffer;
    unsigned short boss;
    unsigned short child;
    bool aggressive;
    bool always_aggressive;
    unsigned short leash_timer;
    int target_player_id;
    int chase_target_id;
    int stuck_x;
    int stuck_y;

    Npc(int npc_index, short npc_id, short x, short y, short attack_dir, int spawn_type, short spawn_time);
    ~Npc();
};


#endif
