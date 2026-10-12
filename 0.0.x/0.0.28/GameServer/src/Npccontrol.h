#ifndef NpccontrolH
#define NpccontrolH

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

struct MapContainer;
struct Npc;
struct Packets;
struct Player;
struct Players;
struct Settings;

#include "Player.h"

struct NpcController;

// Moves the npcs on every map: their respawns, wandering, chases, attacks and talk.
struct NpcController
{
    Settings *settings;
    MapContainer *map_control;
    Players *players;
    Packets *server;
    char *encode_scratch;
    bool player_targets_valid;
    vector<Player *> player_targets;
    int act_counter;
    int talk_counter;
    int regen_counter;

    NpcController(MapContainer *map_control, Players *players, Packets *server, Settings *settings);
    ~NpcController();
    void Tick();
    int Npc_GetDistance(Npc *npc, Player *player);
    bool Npc_AttackPlayer(Npc *npc, Player *player);
    void Npc_Wander(Npc *npc, int map_id, int map_w, int map_h);
    void Npc_ChaseTarget(Npc *npc, Player *player, int map_id, int map_w, int map_h);
    char Npc_IsPlayerAt(int map_id, int x, int y);
    int Npc_GetPlayerIdAt(int map_id, int x, int y);
    char Npc_IsWithinRange(int x1, int y1, int x2, int y2);
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
