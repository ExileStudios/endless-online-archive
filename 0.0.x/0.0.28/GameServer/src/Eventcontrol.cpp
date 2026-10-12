#include <vcl.h>
#pragma hdrstop

#include "Eventcontrol.h"
#include "Map.h"
#include "Mapcontrol.h"
#include "Mapwarp.h"
#include "Packets.h"
#include "Player.h"
#include "Players.h"
#include "Settings.h"
#include "Shopvalues.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

EventController::EventController(MapContainer *map_control, Players *players, Packets *server, Settings *settings)
{
    this->encode_scratch = new char[8];
    this->map_control = map_control;
    this->settings = settings;
    this->players = players;
    this->server = server;
}

EventController::~EventController()
{
}

// Once a second: the maps' quest cooldowns, the evacuations and the arena rounds.
void EventController::Tick()
{
    for (vector<MapItem>::iterator map_iter = map_control->maps.begin(); map_iter != map_control->maps.end();
        map_iter++)
    {
        if (map_iter->quest_cooldown > 0)
            map_iter->quest_cooldown--;
        // .. an evacuation: warnings at 30, 20 and 10 seconds, then the players left go to jail
        if (map_iter->evac_countdown > 0)
        {
            map_iter->evac_countdown--;
            if (map_iter->evac_countdown == 30 || map_iter->evac_countdown == 20 || map_iter->evac_countdown == 10)
            {
                if (map_iter->evac_countdown == 10)
                    server->Server_BroadcastToMap(map_iter->map_id, AC_SYSTEM, OP_CHAT,
                        "Last warning! - leave this map in (" + IntToStr(map_iter->evac_countdown) +
                        ") seconds or be send to jail.");
                else
                    server->Server_BroadcastToMap(map_iter->map_id, AC_SYSTEM, OP_CHAT,
                        "Warning! - please leave this map in (" + IntToStr(map_iter->evac_countdown) +
                        ") seconds or be send to jail.");
            }
            else if (map_iter->evac_countdown > 3)
                server->Server_BroadcastToMap(map_iter->map_id, AC_SET, OP_SOUND,
                    EncodeNumber(51, 1));
            if (map_iter->evac_countdown < 1)
            {
                for (vector<Player *>::iterator player_iter = players->players.begin();
                    player_iter != players->players.end(); player_iter++)
                {
                    if (!(*player_iter)->connected)
                        continue;
                    if ((*player_iter)->admin_level >= AdminLevel_LightGuide)
                        continue;
                    if ((*player_iter)->map_id != map_iter->map_id)
                        continue;
                    MapCoord evac_pos;
                    evac_pos.x = random(3) + 14;
                    evac_pos.y = random(3) + 37;
                    // map 47, the jail of the warning
                    server->Player_Warp(*player_iter, MapId_CastleMaze, evac_pos, WarpEffect_None, 0);
                }
            }
        }
        // .. the arena: once a minute the players on its spawn tiles are warped in
        if (map_iter->arena_enabled)
        {
            map_iter->arena_ticks++;
            if (!(map_iter->arena_ticks % 60))
            {
                if (map_iter->arena_ticks > 119)
                    map_iter->arena_ticks = 0;
                int fighter_count = players->CountArenaPlayers(map_iter->map_id);
                int warped_count = 0;
                if (fighter_count >= map_iter->arena_block)
                {
                    if (!map_iter->arena_ticks)
                        server->Server_BroadcastToMap(map_iter->map_id, AC_DROP, OP_ARENA, "N");
                }
                else
                {
                    for (vector<MapWarp>::iterator it = map_iter->arena_spawn_list.begin();
                        it != map_iter->arena_spawn_list.end(); it++)
                    {
                        Player *target_player = players->GetByMapTile(map_iter->map_id, it->from_x, it->from_y);
                        if (target_player)
                        {
                            warped_count++;
                            MapCoord arena_pos;
                            arena_pos.x = it->to_x;
                            arena_pos.y = it->to_y;
                            target_player->arena_entering = true;
                            target_player->in_arena = false;
                            target_player->arena_kills = 0;
                            server->Player_Warp(target_player, map_iter->map_id, arena_pos, WarpEffect_None, 0);
                            if (map_iter->arena_block == 2 && fighter_count == 1)
                                break;
                        }
                    }
                    if (warped_count > 0)
                        server->Server_BroadcastToMap(map_iter->map_id, AC_DO, OP_ARENA,
                            EncodeNumber(warped_count, 1));
                }
            }
        }
    }
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(EventController)
