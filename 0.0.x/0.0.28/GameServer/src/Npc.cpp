#include <vcl.h>
#pragma hdrstop

#include "Npc.h"

#pragma package(smart_init)

#include <stdlib.h>

Npc::Npc(int npc_index, short npc_id, short x, short y, short attack_dir, int spawn_type, short spawn_time)
{
    // .. the spawn type sets act_ticks: 1 for types 0 and 1, up to 48 for 7 and up
    if (spawn_type >= 7)
        this->act_ticks = 48;
    if (spawn_type == 6)
        this->act_ticks = 24;
    if (spawn_type == 5)
        this->act_ticks = 12;
    if (spawn_type == 4)
        this->act_ticks = 6;
    if (spawn_type == 3)
        this->act_ticks = 3;
    if (spawn_type == 2)
        this->act_ticks = 2;
    if (spawn_type == 1)
        this->act_ticks = 1;
    if (!spawn_type)
        this->act_ticks = 1;
    this->index = npc_index;
    this->id = npc_id;
    this->x = x;
    this->y = y;
    this->direction = random(4);
    this->pos_pending = false;
    this->talk_pending = false;
    this->base_act_ticks = this->act_ticks;
    this->spawn_time = spawn_time;
    this->base_spawn_time = spawn_time;
    TDateTime now1 = Now();
    this->death_stamp = DateTimeToTimeStamp(now1);
    this->spawn_x = x;
    this->spawn_y = y;
    this->act_counter = 4;
    this->move_cooldown = 1;
    this->facing = attack_dir;
    this->alive = false;
    this->aggressive = false;
    this->always_aggressive = false;
    this->chase_target_id = -1;
}

Npc::~Npc()
{
}
