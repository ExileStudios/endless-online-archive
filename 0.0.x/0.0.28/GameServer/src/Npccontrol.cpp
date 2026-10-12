#include <vcl.h>
#pragma hdrstop

#include "Npccontrol.h"
#include "Gamecontrol.h"
#include "MainForm.h"
#include "Map.h"
#include "Mapcontrol.h"
#include "Npc.h"
#include "Npcvalues.h"
#include "Packets.h"
#include "Player.h"
#include "Players.h"
#include "Settings.h"
#include "Shopvalues.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

NpcController::NpcController(MapContainer *map_control, Players *players, Packets *server, Settings *settings)
{
    this->encode_scratch = new char[8];
    this->map_control = map_control;
    this->settings = settings;
    this->players = players;
    this->server = server;
    this->act_counter = 0;
    this->talk_counter = 0;
    this->regen_counter = 0;
}

NpcController::~NpcController()
{
}

// One npc step ( every 200 ms ): respawns, regen, the moves and attacks, then each player gets what changed near them.
void NpcController::Tick()
{
    char flag = 1;
    act_counter++;
    regen_counter++;
    for (vector<MapItem>::iterator map = map_control->maps.begin(); map != map_control->maps.end(); map++)
    {
        map->npc_dirty = false;
        player_targets_valid = false;
        for (vector<Npc *>::iterator npc = map->npc_list.begin(); npc != map->npc_list.end(); npc++)
        {
            if (!(*npc)->alive)
            {
                // .. a dead npc: every 6th step, respawn once its spawn time has passed
                if (!(act_counter % 6))
                {
                    TDateTime now = Now();
                    TTimeStamp stamp = DateTimeToTimeStamp(now);
                    int date_delta = stamp.Date - (*npc)->death_stamp.Date;
                    int time_delta = stamp.Time - (*npc)->death_stamp.Time;
                    int elapsed = SECONDS_PER_DAY * date_delta + time_delta / MS_PER_SECOND;
                    if ((*npc)->spawn_time <= elapsed)
                    {
                        int sx = (*npc)->spawn_x;
                        int sy = (*npc)->spawn_y;
                        // up to 2 tiles from its spawn point
                        if (sx > 1 && sy > 1 && sx + 2 < map->width && sy + 2 < map->height)
                        {
                            sx = (*npc)->spawn_x + random(5) - 2;
                            sy = (*npc)->spawn_y + random(5) - 2;
                        }
                        if (!map_control->IsTileClear(map->map_id, sx, sy))
                            (*npc)->spawn_time++;
                        // a boss's child waits for its boss
                        else if ((*npc)->child > 0 && !map->boss_alive)
                            (*npc)->spawn_time++;
                        else
                        {
                            (*npc)->alive = true;
                            (*npc)->spawn_time = (*npc)->base_spawn_time + random(20);
                            (*npc)->act_ticks = (*npc)->base_act_ticks;
                            (*npc)->x = sx;
                            (*npc)->y = sy;
                            (*npc)->pos_pending = true;
                            map->npc_dirty = true;
                            (*npc)->direction = random(4);
                            (*npc)->move_cooldown = random(3) + 2;
                            if ((*npc)->boss > 0)
                                map->boss_alive = true;
                            // a slow npc stands at its spawn point
                            if ((*npc)->base_act_ticks >= 24)
                            {
                                (*npc)->x = (*npc)->spawn_x;
                                (*npc)->y = (*npc)->spawn_y;
                                (*npc)->direction = (*npc)->base_spawn_time % 4;
                            }
                            (*npc)->aggressive = false;
                            (*npc)->stuck_x = -1;
                            (*npc)->target_player_id = -1;
                            (*npc)->chase_target_id = -1;
                            (*npc)->leash_timer = 0;
                            (*npc)->hp = GUI->npc_values->GetMaxHp((*npc)->id);
                            (*npc)->max_hp = (*npc)->hp;
                            (*npc)->hp_regen = (*npc)->max_hp / 10;
                            (*npc)->hp_regen++;
                            MapCoord drop = GUI->npc_values->GetDrop((*npc)->id);
                            (*npc)->drop_item_id = drop.item_id;
                            (*npc)->drop_amount = drop.amount;
                            (*npc)->pos_buffer = EncodeNumber((*npc)->index, 1);
                            (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->x, 1), (*npc)->pos_buffer.Length() + 1);
                            (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->y, 1), (*npc)->pos_buffer.Length() + 1);
                            (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->direction, 1),
                                (*npc)->pos_buffer.Length() + 1);
                        }
                    }
                }
            }
            else
            {
                // .. every 451 steps an npc heals a tenth of its max hp
                if (regen_counter > 450)
                {
                    (*npc)->hp += (*npc)->hp_regen;
                    if ((*npc)->hp > (*npc)->max_hp)
                        (*npc)->hp = (*npc)->max_hp;
                }
                if ((*npc)->aggressive)
                {
                    (*npc)->leash_timer--;
                    // it calms down after a while, unless it is low on hp
                    if ((*npc)->leash_timer <= 0 && (*npc)->hp_percent > 30)
                    {
                        (*npc)->aggressive = false;
                        (*npc)->chase_target_id = -1;
                    }
                }
                // .. the npcs act only while the map has players, and for a while after
                if (map->npc_act_ticks >= 1)
                {
                    (*npc)->pos_pending = false;
                    (*npc)->talk_pending = false;
                    (*npc)->attack_pending = false;
                    if (map->player_count < 1)
                        map->npc_act_ticks--;
                    if (!(*npc)->act_ticks)
                        (*npc)->act_ticks = 1;
                    // .. every 24th step one npc in five rolls a talk line
                    if (act_counter > 23)
                    {
                        talk_counter++;
                        if (talk_counter > 4)
                        {
                            talk_counter = 0;
                            String line = GUI->npc_values->RollTalk((*npc)->id);
                            if (line.Length() > 0)
                            {
                                map->npc_dirty = true;
                                (*npc)->talk_pending = true;
                                (*npc)->talk_buffer = EncodeNumber((*npc)->index, 1);
                                (*npc)->talk_buffer.Insert(EncodeNumber(line.Length(), 1),
                                    (*npc)->talk_buffer.Length() + 1);
                                (*npc)->talk_buffer.Insert(line, (*npc)->talk_buffer.Length() + 1);
                            }
                        }
                    }
                    // .. an angry npc: chase its target, or attack it when next to it
                    if ((*npc)->aggressive || (*npc)->always_aggressive)
                    {
                        if (!(act_counter % (*npc)->act_ticks))
                        {
                            (*npc)->act_counter++;
                            if ((*npc)->act_counter >= 2)
                            {
                                (*npc)->act_counter = 0;
                                (*npc)->move_cooldown--;
                                Player *target = 0;
                                int distance = 10000;
                                if ((*npc)->chase_target_id > 0)
                                {
                                    if ((*npc)->target_player_id < 0)
                                        target = players->GetById((*npc)->chase_target_id);
                                    else
                                        target = players->GetById((*npc)->target_player_id);
                                    if (!target)
                                        (*npc)->chase_target_id = -1;
                                    else if (target->map_id != map->map_id)
                                    {
                                        (*npc)->chase_target_id = -1;
                                        target = 0;
                                    }
                                }
                                if (target)
                                {
                                    distance = Npc_GetDistance(*npc, target);
                                    // a member of the target's party who is closer is taken instead
                                    if (target->in_party && distance > 1)
                                    {
                                        for (int j = 0; j < PARTY_MAX_MEMBERS; j++)
                                        {
                                            Player *member = players->GetById(target->party_ids[j]);
                                            if (member)
                                            {
                                                if (member->map_id == target->map_id)
                                                {
                                                    int member_distance = Npc_GetDistance(*npc, member);
                                                    if (member_distance <= distance)
                                                    {
                                                        distance = member_distance;
                                                        target = member;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    // .. no target yet: the nearest player on the map
                                    if (!player_targets_valid)
                                    {
                                        player_targets.clear();
                                        for (vector<Player *>::iterator candidate_iter = players->players.begin();
                                            candidate_iter != players->players.end(); candidate_iter++)
                                        {
                                            if ((*candidate_iter)->logged_in &&
                                                (*candidate_iter)->map_id == map->map_id)
                                                player_targets.insert(player_targets.end(), *candidate_iter);
                                        }
                                        player_targets_valid = true;
                                    }
                                    for (vector<Player *>::iterator it = player_targets.begin();
                                        it != player_targets.end(); it++)
                                    {
                                        int candidate_distance = Npc_GetDistance(*npc, *it);
                                        if (candidate_distance <= distance)
                                        {
                                            distance = candidate_distance;
                                            target = *it;
                                        }
                                    }
                                }
                                if (!target)
                                {
                                    (*npc)->chase_target_id = -1;
                                    continue;
                                }
                                if (distance > 1)
                                {
                                    // a new target must be within 11 steps; a chase ends past 16
                                    if ((*npc)->chase_target_id < 0)
                                    {
                                        if (distance > 11)
                                            continue;
                                    }
                                    else if (distance > 16)
                                    {
                                        (*npc)->chase_target_id = -1;
                                        continue;
                                    }
                                    Npc_ChaseTarget(*npc, target, map->map_id, map->width, map->height);
                                    if ((*npc)->pos_pending)
                                    {
                                        map->npc_dirty = true;
                                        (*npc)->pos_buffer = EncodeNumber((*npc)->index, 1);
                                        (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->x, 1),
                                            (*npc)->pos_buffer.Length() + 1);
                                        (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->y, 1),
                                            (*npc)->pos_buffer.Length() + 1);
                                        (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->direction, 1),
                                            (*npc)->pos_buffer.Length() + 1);
                                    }
                                }
                                else
                                {
                                    if (Npc_AttackPlayer(*npc, target))  // the player died
                                    {
                                        (*npc)->chase_target_id = -1;
                                        flag = 0;
                                    }
                                    map->npc_dirty = true;
                                    (*npc)->attack_pending = true;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!(act_counter % (*npc)->act_ticks))
                        {
                            (*npc)->act_counter++;
                            if ((*npc)->act_counter >= 2)
                            {
                                (*npc)->act_counter = 0;
                                (*npc)->move_cooldown--;
                                // .. a calm npc wanders
                                Npc_Wander(*npc, map->map_id, map->width, map->height);
                                if ((*npc)->pos_pending)
                                {
                                    map->npc_dirty = true;
                                    (*npc)->pos_buffer = EncodeNumber((*npc)->index, 1);
                                    (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->x, 1),
                                        (*npc)->pos_buffer.Length() + 1);
                                    (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->y, 1),
                                        (*npc)->pos_buffer.Length() + 1);
                                    (*npc)->pos_buffer.Insert(EncodeNumber((*npc)->direction, 1),
                                        (*npc)->pos_buffer.Length() + 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (act_counter > 23)
        act_counter = 0;
    if (regen_counter > 450)
        regen_counter = 0;
    vector<Player *>::iterator it;
    // .. each player on a changed map gets the npcs in their view
    for (it = players->players.begin(); it != players->players.end(); it++)
    {
        if (!(*it)->logged_in)
            continue;
        if ((*it)->map_id <= 0)
            continue;
        if ((int)map_control->maps.size() < (*it)->map_id)
            continue;
        if (!map_control->maps[(*it)->map_id - 1].npc_dirty)
            continue;
        try
        {
            String pos = "";
            String talk = "";
            String attack = "";
            for (vector<Npc *>::iterator map_npc = map_control->maps[(*it)->map_id - 1].npc_list.begin();
                map_npc != map_control->maps[(*it)->map_id - 1].npc_list.end(); map_npc++)
            {
                if ((*map_npc)->pos_pending && Npc_IsWithinRange((*it)->x, (*it)->y, (*map_npc)->x, (*map_npc)->y))
                    pos.Insert((*map_npc)->pos_buffer, pos.Length() + 1);
                if ((*map_npc)->talk_pending && Npc_IsWithinRange((*it)->x, (*it)->y, (*map_npc)->x, (*map_npc)->y))
                    talk.Insert((*map_npc)->talk_buffer, talk.Length() + 1);
                if ((*map_npc)->attack_pending && Npc_IsWithinRange((*it)->x, (*it)->y, (*map_npc)->x, (*map_npc)->y))
                    attack.Insert((*map_npc)->attack_buffer, attack.Length() + 1);
            }
            if (pos.Length() >= 4 || talk.Length() >= 4 || attack.Length() >= 4)
            {
                String data = pos;
                // the moves, the attacks and the talk, each ended by a break byte
                data.Insert(String((char)EO_BREAK_BYTE), data.Length() + 1);
                data.Insert(attack, data.Length() + 1);
                data.Insert(String((char)EO_BREAK_BYTE), data.Length() + 1);
                data.Insert(talk, data.Length() + 1);
                data.Insert(String((char)EO_BREAK_BYTE), data.Length() + 1);
                // the player's hp and tp, and their hp percent to their party, when an npc hit them
                if ((*it)->stats_dirty)
                {
                    data.Insert(EncodeNumber((*it)->hp, 2), data.Length() + 1);
                    data.Insert(EncodeNumber((*it)->tp, 2), data.Length() + 1);
                    if ((*it)->in_party)
                    {
                        String party_data = EncodeNumber((*it)->player_id, 2);
                        party_data.Insert(EncodeNumber((*it)->HpPercent(), 1), party_data.Length() + 1);
                        server->Server_BroadcastToParty(*it, AC_UPDATE, OP_GROUP, party_data);
                    }
                    (*it)->stats_dirty = false;
                }
                server->Client_SendEncoded(*it, AC_SET, OP_NPC, data);
            }
        }
        catch (...)
        {
        }
    }
    // .. no one was killed: done; else the dead respawn
    if (flag)
        return;
    for (it = players->players.begin(); it != players->players.end(); it++)
    {
        if ((*it)->logged_in && (*it)->map_id > 0 && (int)map_control->maps.size() >= (*it)->map_id)
        {
            if ((*it)->dead)
                server->Player_Respawn(*it);
        }
    }
}

// The steps between the npc and the player ( x plus y ).
int NpcController::Npc_GetDistance(Npc *npc, Player *player)
{
    int distance = 0;
    if (npc->y > player->y)
        distance += npc->y - player->y;
    else
        distance += player->y - npc->y;
    if (npc->x > player->x)
        distance += npc->x - player->x;
    else
        distance += player->x - npc->x;
    return distance;
}

// The npc hits the player: the hit chance, the damage and element, then the attack as the clients get it.
// true when the player died
bool NpcController::Npc_AttackPlayer(Npc *npc, Player *player)
{
    int accuracy = GUI->game_control->Combat_CalcArmorPen(npc->accuracy, player->evasion, 0.9);
    int damage = 0;
    // a sitting player is always hit
    if (player->on_chair || player->sitting)
        accuracy = 100;
    if (random(100) < accuracy)
    {
        accuracy = GUI->game_control->Combat_CalcArmorPen((npc->min_damage + npc->max_damage) / 2, player->armor, 0.8);
        double scaled_damage = npc->min_damage;
        if (scaled_damage < 1.0)
            scaled_damage = 1.0;
        scaled_damage = 0.01L * scaled_damage;
        scaled_damage = accuracy * scaled_damage;
        scaled_damage = random(npc->max_damage - npc->min_damage + 2) + scaled_damage;
        damage = scaled_damage;
        if (damage < 1)
            damage = 1;
    }
    // .. the npc's element against the player's resistance to it
    if (npc->element_weakness > 0)
    {
        MapCoord element;
        element.x = npc->element_weakness;
        element.y = 0;
        if (element.x == 1)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[1],
                player->element_resistances[2]) * damage;
        if (element.x == 2)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[2],
                player->element_resistances[1]) * damage;
        if (element.x == 3)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[3],
                player->element_resistances[6]) * damage;
        if (element.x == 4)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[4],
                player->element_resistances[3]) * damage;
        if (element.x == 5)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[5],
                player->element_resistances[4]) * damage;
        if (element.x == 6)
            damage = GUI->game_control->Combat_CalcElementMult(element, npc->element_weakness_damage_table[6],
                player->element_resistances[5]) * damage;
    }
    // a hit in the back does half again
    if (npc->facing == player->direction)
        damage += damage / 2;
    // .. the npc turns to face the player
    if (npc->x == player->x)
    {
        if (npc->y > player->y)
            npc->facing = FacingDirection_Up;
        if (npc->y < player->y)
            npc->facing = FacingDirection_Down;
    }
    else
    {
        if (npc->x > player->x)
            npc->facing = FacingDirection_Left;
        if (npc->x < player->x)
            npc->facing = FacingDirection_Right;
    }
    player->hp -= damage;
    if (player->hp <= 0)
    {
        player->dead = true;
        player->hp = 0;
    }
    int hp_percent = 100 * player->hp / player->max_hp;
    player->stats_dirty = true;
    // .. the npc, 1 = the player lives or 2 = dies, the facing, the player, the damage, their hp %
    npc->attack_buffer = EncodeNumber(npc->index, 1);
    if (player->hp > 0)
        npc->attack_buffer.Insert(EncodeNumber(1, 1), npc->attack_buffer.Length() + 1);
    if (player->hp < 1)
        npc->attack_buffer.Insert(EncodeNumber(2, 1), npc->attack_buffer.Length() + 1);
    npc->attack_buffer.Insert(EncodeNumber(npc->facing, 1), npc->attack_buffer.Length() + 1);
    npc->attack_buffer.Insert(EncodeNumber(player->player_id, 2), npc->attack_buffer.Length() + 1);
    npc->attack_buffer.Insert(EncodeNumber(damage, 3), npc->attack_buffer.Length() + 1);
    npc->attack_buffer.Insert(EncodeNumber(hp_percent, 1), npc->attack_buffer.Length() + 1);
    if (player->hp == 0)
        return true;
    return false;
}

// A calm npc's step: now and then it turns or stops, and it steps on when the tile ahead is free.
void NpcController::Npc_Wander(Npc *npc, int map_id, int map_w, int map_h)
{
    if (npc->move_cooldown < 1 || npc->move_cooldown > 10)
    {
        npc->facing = random(5);  // 4 = it stands
        npc->move_cooldown = random(3) + 2;
    }
    if (npc->facing == FacingDirection_Down)
    {
        if (npc->y < map_h)
        {
            if (!map_control->GetTileBlock(map_id, npc->x, npc->y + 1, 0))
            {
                if (!map_control->IsOccupied(map_id, npc->x, npc->y + 1))
                {
                    if (!Npc_IsPlayerAt(map_id, npc->x, npc->y + 1))
                    {
                        npc->direction = npc->facing;
                        npc->pos_pending = true;
                        npc->y++;
                    }
                }
            }
        }
    }
    else if (npc->facing == FacingDirection_Left)
    {
        if (npc->x >= 1)
        {
            if (!map_control->GetTileBlock(map_id, npc->x - 1, npc->y, 0))
            {
                if (!map_control->IsOccupied(map_id, npc->x - 1, npc->y))
                {
                    if (!Npc_IsPlayerAt(map_id, npc->x - 1, npc->y))
                    {
                        npc->direction = npc->facing;
                        npc->pos_pending = true;
                        npc->x--;
                    }
                }
            }
        }
    }
    else if (npc->facing == FacingDirection_Up)
    {
        if (npc->y >= 1)
        {
            if (!map_control->GetTileBlock(map_id, npc->x, npc->y - 1, 0))
            {
                if (!map_control->IsOccupied(map_id, npc->x, npc->y - 1))
                {
                    if (!Npc_IsPlayerAt(map_id, npc->x, npc->y - 1))
                    {
                        npc->direction = npc->facing;
                        npc->pos_pending = true;
                        npc->y--;
                    }
                }
            }
        }
    }
    else if (npc->facing == FacingDirection_Right)
    {
        if (npc->x < map_w)
        {
            if (!map_control->GetTileBlock(map_id, npc->x + 1, npc->y, 0))
            {
                if (!map_control->IsOccupied(map_id, npc->x + 1, npc->y))
                {
                    if (!Npc_IsPlayerAt(map_id, npc->x + 1, npc->y))
                    {
                        npc->direction = npc->facing;
                        npc->pos_pending = true;
                        npc->x++;
                    }
                }
            }
        }
    }
}

// A step toward the player: along the longer way first, and up to four tries around what is in the way.
// a player who stands in the way becomes the npc's target
void NpcController::Npc_ChaseTarget(Npc *npc, Player *player, int map_id, int map_w, int map_h)
{
    int dir;
    if (player->x < npc->x)
    {
        if (player->y == npc->y)
            dir = FacingDirection_Left;
        if (player->y < npc->y)
        {
            if (npc->y - player->y < npc->x - player->x)
                dir = FacingDirection_Left;
            else
                dir = FacingDirection_Up;
        }
        if (player->y > npc->y)
        {
            if (player->y - npc->y < npc->x - player->x)
                dir = FacingDirection_Left;
            else
                dir = FacingDirection_Down;
        }
    }
    if (player->x > npc->x)
    {
        if (player->y == npc->y)
            dir = FacingDirection_Right;
        if (player->y < npc->y)
        {
            if (npc->y - player->y < player->x - npc->x)
                dir = FacingDirection_Right;
            else
                dir = FacingDirection_Up;
        }
        if (player->y > npc->y)
        {
            if (player->y - npc->y < player->x - npc->x)
                dir = FacingDirection_Right;
            else
                dir = FacingDirection_Down;
        }
    }
    if (player->x == npc->x)
    {
        dir = FacingDirection_Up;
        if (player->y > npc->y)
            dir = FacingDirection_Down;
    }
    int first_dir = dir;
    int attempt = 0;
    // .. try 1: straight on; 2: the other way to the player; 3: back, and keep away from here; 4: aside
    while (attempt < 4)
    {
        if (attempt == 3)
        {
            if (dir == FacingDirection_Down || dir == FacingDirection_Up)
            {
                if (first_dir == FacingDirection_Left)
                    dir = FacingDirection_Right;
                else
                    dir = FacingDirection_Left;
            }
            else
            {
                if (first_dir == FacingDirection_Up)
                    dir = FacingDirection_Down;
                else
                    dir = FacingDirection_Up;
            }
        }
        if (attempt == 2)
        {
            npc->stuck_x = npc->x;
            npc->stuck_y = npc->y;
            dir += 2;
            if (dir > FacingDirection_Right)
                dir = dir - 4;
        }
        if (attempt == 1)
        {
            if (dir == FacingDirection_Down || dir == FacingDirection_Up)
            {
                if (player->x < npc->x)
                    dir = FacingDirection_Left;
                if (player->x > npc->x)
                    dir = FacingDirection_Right;
                if (player->x == npc->x)
                {
                    if (npc->direction == FacingDirection_Left)
                        dir = FacingDirection_Left;
                    else
                        dir = FacingDirection_Right;
                }
            }
            else
            {
                if (player->y < npc->y)
                    dir = FacingDirection_Up;
                if (player->y > npc->y)
                    dir = FacingDirection_Down;
                if (player->y == npc->y)
                {
                    if (npc->direction == FacingDirection_Down)
                        dir = FacingDirection_Down;
                    else
                        dir = FacingDirection_Up;
                }
            }
        }
        attempt++;
        if (dir == FacingDirection_Down)
        {
            if (npc->y >= map_h)
                continue;
            if (npc->x == npc->stuck_x)
            {
                if (npc->y + 1 == npc->stuck_y)
                    continue;
            }
            if (map_control->GetTileBlock(map_id, npc->x, npc->y + 1, 0))
                continue;
            if (map_control->IsOccupied(map_id, npc->x, npc->y + 1))
                continue;
            int down_blocker_id = Npc_GetPlayerIdAt(map_id, npc->x, npc->y + 1);
            if (down_blocker_id > 0)
            {
                if (attempt <= 1)
                    continue;
                npc->target_player_id = down_blocker_id;
            }
            else
            {
                if (attempt == 1)
                    npc->stuck_x = -1;
                npc->target_player_id = -1;
                npc->direction = dir;
                npc->pos_pending = true;
                npc->y++;
            }
            return;
        }
        if (dir == FacingDirection_Left)
        {
            if (npc->x < 1)
                continue;
            if (npc->x - 1 == npc->stuck_x)
            {
                if (npc->y == npc->stuck_y)
                    continue;
            }
            if (map_control->GetTileBlock(map_id, npc->x - 1, npc->y, 0))
                continue;
            if (map_control->IsOccupied(map_id, npc->x - 1, npc->y))
                continue;
            int left_blocker_id = Npc_GetPlayerIdAt(map_id, npc->x - 1, npc->y);
            if (left_blocker_id > 0)
            {
                if (attempt <= 1)
                    continue;
                npc->target_player_id = left_blocker_id;
            }
            else
            {
                if (attempt == 1)
                    npc->stuck_x = -1;
                npc->target_player_id = -1;
                npc->direction = dir;
                npc->pos_pending = true;
                npc->x--;
            }
            return;
        }
        if (dir == FacingDirection_Up)
        {
            if (npc->y < 1)
                continue;
            if (npc->x == npc->stuck_x)
            {
                if (npc->y - 1 == npc->stuck_y)
                    continue;
            }
            if (map_control->GetTileBlock(map_id, npc->x, npc->y - 1, 0))
                continue;
            if (map_control->IsOccupied(map_id, npc->x, npc->y - 1))
                continue;
            int up_blocker_id = Npc_GetPlayerIdAt(map_id, npc->x, npc->y - 1);
            if (up_blocker_id > 0)
            {
                if (attempt <= 1)
                    continue;
                npc->target_player_id = up_blocker_id;
            }
            else
            {
                if (attempt == 1)
                    npc->stuck_x = -1;
                npc->target_player_id = -1;
                npc->direction = dir;
                npc->pos_pending = true;
                npc->y--;
            }
            return;
        }
        if (dir == FacingDirection_Right)
        {
            if (npc->x >= map_w)
                continue;
            if (npc->x + 1 == npc->stuck_x)
            {
                if (npc->y == npc->stuck_y)
                    continue;
            }
            if (map_control->GetTileBlock(map_id, npc->x + 1, npc->y, 0))
                continue;
            if (map_control->IsOccupied(map_id, npc->x + 1, npc->y))
                continue;
            int right_blocker_id = Npc_GetPlayerIdAt(map_id, npc->x + 1, npc->y);
            if (right_blocker_id > 0)
            {
                if (attempt <= 1)
                    continue;
                npc->target_player_id = right_blocker_id;
            }
            else
            {
                if (attempt == 1)
                    npc->stuck_x = -1;
                npc->target_player_id = -1;
                npc->direction = dir;
                npc->pos_pending = true;
                npc->x++;
            }
            return;
        }
    }
    return;
}

// 1 when a player on the map stands on the tile, else 0.
char NpcController::Npc_IsPlayerAt(int map_id, int x, int y)
{
    if (!player_targets_valid)
    {
        player_targets.clear();
        for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
        {
            if ((*it)->logged_in && (*it)->map_id == map_id)
                player_targets.insert(player_targets.end(), *it);
        }
        player_targets_valid = true;
    }
    for (vector<Player *>::iterator it = player_targets.begin(); it != player_targets.end(); it++)
    {
        if ((*it)->x == x && (*it)->y == y && (*it)->map_id == map_id)
            return 1;
    }
    return 0;
}

// The player who stands on the tile, or -1.
int NpcController::Npc_GetPlayerIdAt(int map_id, int x, int y)
{
    if (!player_targets_valid)
    {
        player_targets.clear();
        for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
        {
            if ((*it)->logged_in && (*it)->map_id == map_id)
                player_targets.insert(player_targets.end(), *it);
        }
        player_targets_valid = true;
    }
    for (vector<Player *>::iterator it = player_targets.begin(); it != player_targets.end(); it++)
    {
        if ((*it)->x == x && (*it)->y == y && (*it)->map_id == map_id)
            return (*it)->player_id;
    }
    return -1;
}

// 1 when the second spot is in view of the first: 15 steps to the lower right, 12 any other way.
char NpcController::Npc_IsWithinRange(int x1, int y1, int x2, int y2)
{
    char result = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (x2 > x1 && y2 > y1)
    {
        if (dx + dy <= 15)
            result = 1;
    }
    else if (dx + dy <= 12)
        result = 1;
    return result;
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(NpcController)
