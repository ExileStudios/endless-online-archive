#ifndef MapH
#define MapH

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

struct ItemObj;
struct MapChest;
struct MapObject;
struct MapWarp;
struct Npc;

#include "Itemground.h"
#include "Mapchest.h"
#include "Mapobject.h"
#include "Mapwarp.h"
#include "Npc.h"

struct MapItem;

// One map: its size and type, its tiles, warps, chests, npcs and the items on its ground.
struct MapItem
{
    unsigned short map_id;
    unsigned short rid1;
    unsigned short rid2;
    unsigned short filesize;
    unsigned char width;
    unsigned char height;
    unsigned char map_type;
    unsigned char timed_effect;
    bool has_quakes;
    bool has_hp_drain;
    bool has_tp_drain;
    bool has_spikes;
    bool arena_enabled;
    int arena_block;
    int arena_ticks;
    int quest_cooldown;
    int evac_countdown;
    vector<MapWarp> arena_spawn_list;
    int relog_x;
    int relog_y;
    short field_0x4c;
    bool buf_copied;
    bool no_scroll;
    String buf;
    String hp_drain_others;
    bool hp_drain_others_sent;
    vector<MapObject> tile_specs;
    vector<MapObject> legacy_door_key_list;
    vector<MapChest> chest_list;
    vector<MapWarp> warp_list;
    vector<Npc *> npc_list;
    vector<ItemObj *> ground_items;
    int next_ground_item_id;
    unsigned short child_npc_id;
    bool boss_alive;
    bool npc_dirty;
    bool chests_dirty;
    bool has_open_doors;
    int npc_act_ticks;
    int player_count;
    vector<bool> tile_bits;

    MapItem(int map_id, int width, int height);
    ~MapItem();
};


#endif
