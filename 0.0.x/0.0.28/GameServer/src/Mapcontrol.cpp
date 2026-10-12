#include <vcl.h>
#pragma hdrstop

#include "Mapcontrol.h"
#include "Itemchest.h"
#include "Itemground.h"
#include "Jukeboxcontrol.h"
#include "MainForm.h"
#include "Map.h"
#include "Mapchest.h"
#include "Mapobject.h"
#include "Mapwarp.h"
#include "Npc.h"
#include "Npcdrop.h"
#include "Npcvalue.h"
#include "Npcvalues.h"
#include "Settings.h"
#include "Shopvalues.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

#define MAP_EFFECT_TP_DRAIN 2

// Takes the start, rescue and map settings, then loads the maps.
MapContainer::MapContainer(Settings *settings)
{
    this->encode_scratch = new char[8];
    this->settings = settings;
    this->start_map = this->settings->GetStartMap();
    this->start_x = this->settings->GetStartX();
    this->start_y = this->settings->GetStartY();
    this->rescue_map = this->settings->GetRescueMap();
    this->rescue_x = this->settings->GetRescueX();
    this->rescue_y = this->settings->GetRescueY();
    this->max_maps = this->settings->GetMaxMaps();
    this->memory_map = this->settings->GetMemoryMap();
    this->LoadMaps();
}

MapContainer::~MapContainer()
{
}

// Sets the arena's block on the map.
void MapContainer::SetArenaBlock(int map_id, int block)
{
    if (map_id > 0 && (int)maps.size() >= map_id)
        maps[map_id - 1].arena_block = block;
}

// Makes the map an arena and adds a spot its players are sent to.
void MapContainer::AddArenaSpawn(int map_id, int from_x, int from_y, int to_x, int to_y)
{
    if (map_id > 0)
    {
        if ((int)maps.size() >= map_id)
        {
            maps[map_id - 1].arena_enabled = true; maps[map_id - 1].arena_block = 4;
            maps[map_id - 1].arena_ticks = random(60);
            MapWarp value(from_x, from_y, map_id, 0, to_x, to_y);
            maps[map_id - 1].arena_spawn_list.insert(maps[map_id - 1].arena_spawn_list.end(), value);
        }
    }
}

// A player came onto the map; its npcs start to act.
void MapContainer::IncPlayerCount(int map_id)
{
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        maps[map_id - 1].player_count++;
        maps[map_id - 1].npc_act_ticks = 202;
    }
}

// A player left the map.
void MapContainer::DecPlayerCount(int map_id)
{
    if (map_id > 0 && (int)maps.size() >= map_id && maps[map_id - 1].player_count > 0)
        maps[map_id - 1].player_count--;
}

// Sets the tile's two bits in the map:
// 0 = open
// 1 = wall
// 2 = a special tile ( see tile_specs )
// 3 = a warp
void MapContainer::SetTileBits(MapItem *map, int x, int y, int code)
{
    int tile_offset = 2 * x + y * (2 * map->width);
    if (!code)
    {
        map->tile_bits[tile_offset] = 0;
        map->tile_bits[tile_offset + 1] = 0;
    }
    if (code == 1)
    {
        map->tile_bits[tile_offset] = 0;
        map->tile_bits[tile_offset + 1] = 1;
    }
    if (code == 2)
    {
        map->tile_bits[tile_offset] = 1;
        map->tile_bits[tile_offset + 1] = 0;
    }
    if (code == 3)
    {
        map->tile_bits[tile_offset] = 1;
        map->tile_bits[tile_offset + 1] = 1;
    }
}

// The chest's items as the client gets them ( id and amount ), or "N" when there is no chest there.
String MapContainer::BuildChestItemsString(int map_id, MapCoord coords)
{
    vector<MapChest>::iterator chest_iter;
    String result = "N";
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (chest_iter = maps[map_id - 1].chest_list.begin(); chest_iter != maps[map_id - 1].chest_list.end();
            chest_iter++)
        {
            if (chest_iter->x == coords.x && chest_iter->y == coords.y)
            {
                result = "";
                vector<ChestItem>::iterator it;
                for (it = chest_iter->slots.begin(); it != chest_iter->slots.end(); it++)
                {
                    if (it->item_present)
                    {
                        result.Insert(EncodeNumber(it->item_id, 2), result.Length() + 1);
                        result.Insert(EncodeNumber(it->amount, 3), result.Length() + 1);
                    }
                }
                break;
            }
        }
    }
    return result;
}

// The map a warp tile leads to, or 0.
int MapContainer::GetWarpMap(int map_id, int x, int y)
{
    int result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (maps[map_id - 1].tile_bits[tile_offset])
        {
            for (vector<MapWarp>::iterator it = maps[map_id - 1].warp_list.begin();
                it != maps[map_id - 1].warp_list.end(); it++)
            {
                if (it->from_x == x && it->from_y == y)
                {
                    result = it->dest_map;
                    break;
                }
            }
        }
    }
    return result;
}

// The level the warp needs, or 1000 when there is no warp.
int MapContainer::GetWarpLevelReq(int map_id, int x, int y)
{
    int result = 1000;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapWarp>::iterator it = maps[map_id - 1].warp_list.begin(); it != maps[map_id - 1].warp_list.end();
            it++)
        {
            if (it->from_x == x && it->from_y == y)
            {
                result = it->level;
                break;
            }
        }
    }
    return result;
}

// Where the warp puts the player ( x ), or 1000 when there is no warp.
int MapContainer::GetWarpX(int map_id, int x, int y)
{
    int result = 1000;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapWarp>::iterator it = maps[map_id - 1].warp_list.begin(); it != maps[map_id - 1].warp_list.end();
            it++)
        {
            if (it->from_x == x && it->from_y == y)
            {
                result = it->to_x;
                break;
            }
        }
    }
    return result;
}

// Where the warp puts the player ( y ), or 1000 when there is no warp.
int MapContainer::GetWarpY(int map_id, int x, int y)
{
    int result = 1000;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapWarp>::iterator it = maps[map_id - 1].warp_list.begin(); it != maps[map_id - 1].warp_list.end();
            it++)
        {
            if (it->from_x == x && it->from_y == y)
            {
                result = it->to_y;
                break;
            }
        }
    }
    return result;
}

// Opens the door on the tile for 2 ticks, or starts an open door's 2 ticks again; 1 when it did.
char MapContainer::OpenDoor(int map_id, int x, int y)
{
    char result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin();
            it != maps[map_id - 1].tile_specs.end(); it++)
        {
            if (it->x == x && it->y == y)
            {
                if ((it->value == TileSpec_DoorOpen || it->value == TileSpec_PassableDoorOpen) && it->ticks != 2)
                {
                    it->ticks = 2;
                    result = 1;
                }
                if (it->value == TileSpec_Door)
                {
                    result = 1;
                    maps[map_id - 1].has_open_doors = true;
                    it->value = TileSpec_DoorOpen;
                    it->ticks = 2;
                }
                if (it->value == TileSpec_PassableDoor)
                {
                    result = 1;
                    maps[map_id - 1].has_open_doors = true;
                    it->value = TileSpec_PassableDoorOpen;
                    it->ticks = 2;
                }
                break;
            }
        }
    }
    return result;
}

// The special tile at x, y.
MapObject MapContainer::GetTileSpecObject(int map_id, int x, int y)
{
    vector<MapObject>::iterator spec_iter;
    if (map_id <= 0)
        map_id == 1;
    for (spec_iter = maps[map_id - 1].tile_specs.begin(); spec_iter != maps[map_id - 1].tile_specs.end(); spec_iter++)
    {
        if (spec_iter->x == x && spec_iter->y == y)
            break;
    }
    return *spec_iter;
}

// The special tile's value, or -1.
int MapContainer::GetTileSpec(int map_id, int x, int y)
{
    int result = -1;
    for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin(); it != maps[map_id - 1].tile_specs.end();
        it++)
    {
        if (it->x == x && it->y == y)
        {
            result = it->value;
            break;
        }
    }
    return result;
}

// The value of the tile when it is marked special, or -1.
int MapContainer::GetTileSpecValueAt(int map_id, int x, int y)
{
    int result = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (maps[map_id - 1].tile_bits[tile_offset])
        {
            if (!maps[map_id - 1].tile_bits[tile_offset + 1])
            {
                for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin();
                    it != maps[map_id - 1].tile_specs.end(); it++)
                {
                    if (it->x == x && it->y == y)
                    {
                        result = it->value;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// 1 when the map's quest cooldown ran out; it starts again at 10.
char MapContainer::TryTakeQuestCooldown(int map_id)
{
    if (map_id > 0 && (int)maps.size() >= map_id && maps[map_id - 1].quest_cooldown < 1)
    {
        maps[map_id - 1].quest_cooldown = 10;
        return 1;
    }
    return 0;
}

char MapContainer::GetNoScroll(int map_id)
{
    char result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
        result = maps[map_id - 1].no_scroll;
    return result;
}

// Where a player who logs in on the map is put, or 0, 0 when the map gives no spot inside it.
MapCoord MapContainer::GetRelogCoords(int map_id)
{
    MapCoord coords;
    coords.x = 0;
    coords.y = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        coords.x = maps[map_id - 1].relog_x;
        coords.y = maps[map_id - 1].relog_y;
        if (maps[map_id - 1].width <= coords.x || maps[map_id - 1].height <= coords.y)
        {
            coords.x = 0;
            coords.y = 0;
        }
    }
    return coords;
}

// The boss's children on the map turn angry; 1 when there are any.
char MapContainer::AggroChildNpcs(int map_id)
{
    char found_child = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        TDateTime now = Now();
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->child > 0)
            {
                (*it)->aggressive = true;
                found_child = 1;
            }
        }
    }
    return found_child;
}

// The boss is dead: its children on the map die too; 1 when there were any.
char MapContainer::KillChildNpcs(int map_id)
{
    char found_child = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        maps[map_id - 1].boss_alive = false;
        TDateTime now = Now();
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->child <= 0)
                continue;
            (*it)->death_stamp = DateTimeToTimeStamp(now);
            (*it)->alive = false;
            (*it)->aggressive = false;
            found_child = 1;
        }
    }
    return found_child;
}

// The npc's id by its index on the map, or -1.
int MapContainer::GetNpcIdByIndex(int map_id, int npc_index)
{
    int result = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->index == npc_index)
            {
                result = (*it)->id;
                break;
            }
        }
    }
    return result;
}

// Where the npc stands, by its index on the map, or -1, -1.
MapCoord MapContainer::GetNpcCoordsByIndex(int map_id, int npc_index)
{
    MapCoord coords;
    coords.x = -1;
    coords.y = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->index == npc_index)
            {
                coords.x = (*it)->x;
                coords.y = (*it)->y;
                break;
            }
        }
    }
    return coords;
}

// 1 when a living npc stands on the tile.
char MapContainer::IsOccupied(int map_id, int x, int y)
{
    char result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->x == x && (*it)->y == y && (*it)->alive)
            {
                result = 1;
                break;
            }
        }
    }
    return result;
}

// The npcs on the map that chase the player.
int MapContainer::CountNpcsChasingPlayer(int map_id, int player_id)
{
    int chase_count = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<Npc *>::iterator it = maps[map_id - 1].npc_list.begin(); it != maps[map_id - 1].npc_list.end();
            it++)
        {
            if ((*it)->chase_target_id == player_id)
                chase_count++;
        }
    }
    return chase_count;
}

// 1 when an item can lie on the tile: an open tile, a warp or an npc boundary, inside the map.
char MapContainer::IsDropTileClear(int map_id, int x, int y)
{
    char result = 1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (x >= 0 && y >= 0 && maps[map_id - 1].width > x && maps[map_id - 1].height > y)
        {
            if (maps[map_id - 1].tile_bits[tile_offset])
            {
                if (!maps[map_id - 1].tile_bits[tile_offset + 1])
                {
                    result = 0;
                    for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin();
                        it != maps[map_id - 1].tile_specs.end(); it++)
                    {
                        if (it->x == x && it->y == y)
                        {
                            if (it->value == TileSpec_NpcBoundary)
                            {
                                result = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                if (maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 0;
            }
        }
        else
            result = 0;
    }
    return result;
}

// 1 when the tile is open or a warp, inside the map.
char MapContainer::IsTileClear(int map_id, int x, int y)
{
    char result = 1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (x >= 0 && y >= 0 && maps[map_id - 1].width > x && maps[map_id - 1].height > y)
        {
            if (maps[map_id - 1].tile_bits[tile_offset])
            {
                if (!maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 0;
            }
            else
            {
                if (maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 0;
            }
        }
        else
            result = 0;
    }
    return result;
}

// 1 when the tile is a warp, or a door a player walks through.
char MapContainer::IsWarpTile(int map_id, int x, int y)
{
    char result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (x >= 0 && y >= 0 && maps[map_id - 1].width > x && maps[map_id - 1].height > y)
        {
            if (maps[map_id - 1].tile_bits[tile_offset])
            {
                if (maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 1;
                else
                {
                    for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin();
                        it != maps[map_id - 1].tile_specs.end(); it++)
                    {
                        if (it->x == x && it->y == y)
                        {
                            if (it->value == TileSpec_PassableDoor || it->value == TileSpec_PassableDoorOpen)
                            {
                                result = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return result;
}

// How many of the four tiles around are open to an npc ( npc boundaries count as open ).
int MapContainer::CountOpenNeighbors(int map_id, int x, int y)
{
    int result = 0;
    if (GetTileBlock(map_id, x - 1, y, 1) == 0)
        result++;
    if (!GetTileBlock(map_id, x, y - 1, 1))
        result++;
    if (!GetTileBlock(map_id, x + 1, y, 1))
        result++;
    if (!GetTileBlock(map_id, x, y + 1, 1))
        result++;
    return result;
}

// Whether an npc can step on the tile; with ignore_spec_block an npc boundary counts as open:
// 0 = open
// 1 = blocked ( a wall, a special tile, off the map )
// 2 = a warp
int MapContainer::GetTileBlock(int map_id, int x, int y, char ignore_spec_block)
{
    int result = 1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int tile_offset = 2 * x + y * (2 * maps[map_id - 1].width);
        if (x >= 0 && y >= 0 && maps[map_id - 1].width > x && maps[map_id - 1].height > y)
        {
            if (maps[map_id - 1].tile_bits[tile_offset])
            {
                if (maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 2;
                else
                {
                    result = 1;
                    for (vector<MapObject>::iterator it = maps[map_id - 1].tile_specs.begin();
                        it != maps[map_id - 1].tile_specs.end(); it++)
                    {
                        if (it->x == x && it->y == y)
                        {
                            if (it->value == TileSpec_DoorOpen)
                                result = 0;
                            if (it->value == TileSpec_PassableDoorOpen)
                                result = 2;
                            if (it->value == TileSpec_NpcBoundary && ignore_spec_block)
                            {
                                result = 0;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                if (maps[map_id - 1].tile_bits[tile_offset + 1])
                    result = 1;
                else
                    result = 0;
            }
        }
    }
    return result;
}

// Adds a special tile to the map.
void MapContainer::AddTileSpec(MapItem *map, int x, int y, int spec)
{
    MapObject value(x, y, spec);
    map->tile_specs.insert(map->tile_specs.end(), value);
}

// Adds a warp to the map.
void MapContainer::AddWarp(MapItem *map, int x, int y, int dest_map, int level, int dest_x, int dest_y)
{
    MapWarp value(x, y, dest_map, level, dest_x, dest_y);
    map->warp_list.insert(map->warp_list.end(), value);
}

// Sets the key a door on the map needs.
void MapContainer::AddLockKey(MapItem *map, int x, int y, int key_id)
{
    char found = 0;
    for (vector<MapObject>::iterator it = map->legacy_door_key_list.begin(); it != map->legacy_door_key_list.end();
        it++)
    {
        if (it->x == x && it->y == y)
        {
            it->value = key_id;
            found = 1;
            break;
        }
    }
    if (!found)
    {
        MapObject value(x, y, key_id);
        map->legacy_door_key_list.insert(map->legacy_door_key_list.end(), value);
    }
}

// Adds a chest on the tile, once.
void MapContainer::AddChest(MapItem *map, int x, int y)
{
    char found = 0;
    for (vector<MapChest>::iterator it = map->chest_list.begin(); it != map->chest_list.end(); it++)
    {
        if (it->x == x && it->y == y)
        {
            found = 1;
            break;
        }
    }
    if (!found)
    {
        MapChest value(x, y, 0);
        map->chest_list.insert(map->chest_list.end(), value);
    }
}

// Adds a chest item that comes back on its time; the first for a slot makes the slot, up to 3 more are its other items.
void MapContainer::AddChestSpawn(MapItem *map, int x, int y, int key_id, int slot, int item_id, int spawn_time,
    int amount)
{
    char found = 0;
    for (vector<MapChest>::iterator chest_iter = map->chest_list.begin(); chest_iter != map->chest_list.end();
        chest_iter++)
    {
        if (chest_iter->x == x && chest_iter->y == y)
        {
            found = 1;
            chest_iter->key_id = key_id;
            if ((int)chest_iter->slots.size() <= slot)
            {
                ChestItem extra_slot(item_id);
                extra_slot.item_present = false;
                extra_slot.respawn_enabled = true;
                extra_slot.respawn_countdown = spawn_time;
                extra_slot.respawn_delay = spawn_time;
                extra_slot.amount = amount;
                extra_slot.alt_item_id0 = item_id;
                extra_slot.alt_amount0 = amount;
                extra_slot.alt_item_id1 = 0;
                extra_slot.alt_item_id2 = 0;
                extra_slot.alt_item_id3 = 0;
                chest_iter->slots.insert(chest_iter->slots.end(), extra_slot);
            }
            else if (chest_iter->slots[slot].alt_item_id1 > 0)
            {
                if (chest_iter->slots[slot].alt_item_id2 > 0)
                {
                    chest_iter->slots[slot].alt_item_id3 = item_id; chest_iter->slots[slot].alt_amount3 = amount;
                }
                else
                {
                    chest_iter->slots[slot].alt_item_id2 = item_id; chest_iter->slots[slot].alt_amount2 = amount;
                }
            }
            else
            {
                chest_iter->slots[slot].alt_item_id1 = item_id; chest_iter->slots[slot].alt_amount1 = amount;
            }
            break;
        }
    }
    if (!found)
    {
        MapChest new_chest(x, y, key_id);
        ChestItem new_item(item_id);
        new_item.item_present = false;
        new_item.respawn_enabled = true;
        new_item.respawn_countdown = spawn_time;
        new_item.respawn_delay = spawn_time;
        new_item.amount = amount;
        new_item.alt_item_id0 = item_id;
        new_item.alt_amount0 = amount;
        new_item.alt_item_id1 = 0;
        new_item.alt_item_id2 = 0;
        new_item.alt_item_id3 = 0;
        new_chest.slots.insert(new_chest.slots.end(), new_item);
        map->chest_list.insert(map->chest_list.end(), new_chest);
    }
}

// Loads maps 1 to max_maps; a missing file gives an empty map.
void MapContainer::LoadMaps()
{
    for (int map_id = 1; map_id <= 64000 && max_maps >= map_id; map_id++)
    {
        if (!LoadMap(map_id))
        {
            MapItem value(map_id, 0, 0);
            maps.insert(maps.end(), value);
        }
    }
}

// Empties a map before it is read again.
void MapContainer::ResetMap(int map_id)
{
    for (int i = 0; i < (int)maps[map_id - 1].chest_list.size(); i++)
        maps[map_id - 1].chest_list[i].slots.clear();
    for (int tile_index = 0; tile_index < (int)maps[map_id - 1].tile_bits.size(); tile_index++)
        maps[map_id - 1].tile_bits[tile_index] = 0;
    maps[map_id - 1].tile_specs.clear();
    maps[map_id - 1].chest_list.clear();
    maps[map_id - 1].warp_list.clear();
    maps[map_id - 1].ground_items.clear();
    maps[map_id - 1].npc_list.clear();
    maps[map_id - 1].legacy_door_key_list.clear();
    maps[map_id - 1].boss_alive = false; maps[map_id - 1].width = 0; maps[map_id - 1].height = 0;
    maps[map_id - 1].buf.Delete(0, maps[map_id].buf.Length());
}

// The map's file, from memory when it was kept, else from maps/<id>.emf.
String MapContainer::ReadRawFile(int map_id)
{
    String result;
    int file_handle;
    if (maps[map_id - 1].buf == "")
    {
        String file_name;
        try
        {
            int size;
            char *buf;
            file_name = IntToStr(map_id);
            for (int i = file_name.Length(); i <= 4; i++)
                file_name.Insert("0", 0);
            file_name.Insert("./maps/", 0);
            file_name.Insert(".emf", file_name.Length() + 1);
            file_handle = FileOpen(file_name.c_str(), 0);
            if (file_handle < 0)
                return "";
            size = FileSeek(file_handle, 0, 2);
            FileSeek(file_handle, 0, 0);
            buf = new char[size + 1];
            FileRead(file_handle, buf, size);
            FileClose(file_handle);
            result = buf;
            result.SetLength(size);
            delete[] buf;
        }
        catch (...)
        {
            FileClose(file_handle);
        }
    }
    else
        result = maps[map_id - 1].buf;
    return result;
}

// Reads a map's file again, its jukebox too.
char MapContainer::ReloadMap(int map_id)
{
    char result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        ResetMap(map_id);
        GUI->jukebox_control->RemoveMap(map_id);
        result = ParseMapFile(&maps[map_id - 1], map_id);
    }
    return result;
}

// Reads maps/<id>.emf ( it starts EMF ) into a map that is already in the list.
bool MapContainer::ParseMapFile(MapItem *map, int map_id)
{
    String map_buf;
    String local_c;
    int file_handle;
    try
    {
        int size;
        char *buf;
        map_buf = IntToStr(map_id);
        for (int i = map_buf.Length(); i <= 4; i++)
            map_buf.Insert("0", 0);
        int y;
        int count;
        int x;
        int tile_count;
        map_buf.Insert("./maps/", 0);
        map_buf.Insert(".emf", map_buf.Length() + 1);
        file_handle = FileOpen(map_buf.c_str(), 0);
        if (file_handle < 0)
            return false;
        size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buf = new char[size + 1];
        FileRead(file_handle, buf, size);
        FileClose(file_handle);
        map_buf = buf;
        map_buf.SetLength(size);
        delete[] buf;
        if (map_buf[1] != 'E' || map_buf[2] != 'M' || map_buf[3] != 'F')
            return false;
        map->map_id = map_id;
        map->width = DecodeNumber(map_buf.SubString(38, 1)) + 1;
        map->height = DecodeNumber(map_buf.SubString(39, 1)) + 1;
        map->rid1 = DecodeNumber(map_buf.SubString(4, 2));
        map->rid2 = DecodeNumber(map_buf.SubString(6, 2));
        map->map_type = DecodeNumber(map_buf.SubString(32, 1));
        map->timed_effect = DecodeNumber(map_buf.SubString(33, 1));
        map->relog_x = DecodeNumber(map_buf.SubString(44, 1));
        map->relog_y = DecodeNumber(map_buf.SubString(45, 1));
        map->has_hp_drain = false;
        map->has_tp_drain = false;
        map->has_quakes = false;
        map->has_spikes = false;
        if (map->timed_effect == 1)
            map->has_hp_drain = true;
        if (map->timed_effect == MAP_EFFECT_TP_DRAIN)
            map->has_tp_drain = true;
        if (map->timed_effect > 2 && map->timed_effect < 7)
            map->has_quakes = true;
        if ((int)map->tile_bits.size() != map->width * map->height * 2)
            map->tile_bits.resize(map->width * map->height * 2, false);
        if (DecodeNumber(map_buf.SubString(43, 1)) == 0)
            map->no_scroll = true;
        map->filesize = size;
        // the start map's file, or every one with memorymap, is kept in memory
        if (start_map == map_id || memory_map)
        {
            map->buf_copied = true;
            map->buf = map_buf;
        }
        else
            map->buf_copied = false;
        // .. after the header: the npc spawns, 8 bytes each, each for a count of one npc
        map_buf.Delete(1, 46);
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            int npc_count = DecodeNumber(map_buf.SubString(8, 1));
            for (int j = 0; j < npc_count; j++)
            {
                int index = (int)map->npc_list.size() + 1;
                Npc *
                    npc = new Npc(index, DecodeNumber(map_buf.SubString(3, 2)), DecodeNumber(map_buf.SubString(1, 1)),
                    DecodeNumber(map_buf.SubString(2, 1)), 0, DecodeNumber(map_buf.SubString(5, 1)),
                    DecodeNumber(map_buf.SubString(6, 2)));
                NpcValue npc_value = GUI->npc_values->GetNpc(DecodeNumber(map_buf.SubString(3, 2)));
                if (npc_value.npc_type == 2)
                {
                    npc->aggressive = true;
                    npc->always_aggressive = true;
                }
                if (npc_value.child > 0)
                    map->child_npc_id = npc->id;
                npc->boss = npc_value.boss;
                npc->child = npc_value.child;
                npc->min_damage = npc_value.min_damage;
                npc->max_damage = npc_value.max_damage;
                npc->accuracy = npc_value.accuracy;
                npc->evade = npc_value.evade;
                npc->armor = npc_value.armor;
                npc->element_weakness = npc_value.element_weakness;
                for (int k = 0; k < ELEMENT_COUNT; k++)
                    npc->element_weakness_damage_table[k] = 0;
                if (npc_value.element_weakness > 0 && npc_value.element_weakness < ELEMENT_COUNT)
                    npc->element_weakness_damage_table[npc_value.element_weakness] = npc_value.element_weakness_damage;
                map->npc_list.insert(map->npc_list.end(), npc);
            }
            map_buf.Delete(1, 8);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            // .. the door keys, 4 bytes each
            int key_x = DecodeNumber(map_buf.SubString(1, 1));
            int key_y = DecodeNumber(map_buf.SubString(2, 1));
            int key_id = DecodeNumber(map_buf.SubString(3, 2));
            AddLockKey(map, key_x, key_y, key_id);
            map_buf.Delete(1, 4);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            // .. the chest items, 12 bytes each
            int chest_x = DecodeNumber(map_buf.SubString(1, 1));
            int chest_y = DecodeNumber(map_buf.SubString(2, 1));
            int chest_key = DecodeNumber(map_buf.SubString(3, 2));
            int chest_slot = DecodeNumber(map_buf.SubString(5, 1));
            int chest_item = DecodeNumber(map_buf.SubString(6, 2));
            int chest_time = DecodeNumber(map_buf.SubString(8, 2));
            int chest_amount = DecodeNumber(map_buf.SubString(10, 3));
            AddChestSpawn(map, chest_x, chest_y, chest_key, chest_slot, chest_item, chest_time, chest_amount);
            map_buf.Delete(1, 12);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            y = DecodeNumber(map_buf.SubString(1, 1));
            tile_count = DecodeNumber(map_buf.SubString(2, 1));
            map_buf.Delete(1, 2);
            // .. the special tiles, a row at a time
            for (int k = 0; k < tile_count; k++)
            {
                x = DecodeNumber(map_buf.SubString(1, 1));
                int code = DecodeNumber(map_buf.SubString(2, 1));
                if (code == MapTileSpec_Wall || code == MapTileSpec_Edge)
                    SetTileBits(map, x, y, 1);
                if (code > MapTileSpec_Wall && code <= MapTileSpec_NpcBoundary)
                {
                    SetTileBits(map, x, y, 2);
                    AddTileSpec(map, x, y, code - 1);
                }
                if (code == MapTileSpec_FakeWall || code == MapTileSpec_Jump)
                {
                    SetTileBits(map, x, y, 2);
                    AddTileSpec(map, x, y, TileSpec_NpcBoundary);
                }
                if (code > MapTileSpec_FakeWall && code <= MapTileSpec_Board8)
                {
                    SetTileBits(map, x, y, 2);
                    AddTileSpec(map, x, y, code - 1);
                }
                if (code == MapTileSpec_Jukebox)
                {
                    SetTileBits(map, x, y, 2);
                    AddTileSpec(map, x, y, code - 1);
                    GUI->jukebox_control->Add(map_id);
                }
                if (code == MapTileSpec_Chest)
                    AddChest(map, x, y);
                if (code == MapTileSpec_Arena)
                    AddTileSpec(map, x, y, code - 1);
                if (code > MapTileSpec_AmbientSource && code < 37)
                {
                    AddTileSpec(map, x, y, code - 1);
                    map->has_spikes = true;
                }
                map_buf.Delete(1, 2);
            }
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            y = DecodeNumber(map_buf.SubString(1, 1));
            tile_count = DecodeNumber(map_buf.SubString(2, 1));
            map_buf.Delete(1, 2);
            // .. the warps, a row at a time, 8 bytes each; a warp with a key is a door
            for (int k = 0; k < tile_count; k++)
            {
                x = DecodeNumber(map_buf.SubString(1, 1));
                int lock_key = DecodeNumber(map_buf.SubString(7, 2));
                SetTileBits(map, x, y, 3);
                AddWarp(map, x, y, DecodeNumber(map_buf.SubString(2, 2)), DecodeNumber(map_buf.SubString(6, 1)),
                    DecodeNumber(map_buf.SubString(4, 1)), DecodeNumber(map_buf.SubString(5, 1)));
                if (lock_key > 0)
                {
                    AddTileSpec(map, x, y, TileSpec_PassableDoor);
                    SetTileBits(map, x, y, 2);
                    if (lock_key > 1)
                        AddLockKey(map, x, y, lock_key);
                }
                map_buf.Delete(1, 8);
            }
        }
    }
    catch (...)
    {
        FileClose(file_handle);
        return false;
    }
    return true;
}

// Reads maps/<id>.emf ( it starts EMF ) into a new map at the end of the list.
bool MapContainer::LoadMap(int map_id)
{
    String map_buf;
    String local_c;
    int file_handle;
    try
    {
        int size;
        char *buf;
        map_buf = IntToStr(map_id);
        for (int i = map_buf.Length(); i <= 4; i++)
            map_buf.Insert("0", 0);
        int y;
        int count;
        int x;
        int tile_count;
        map_buf.Insert("./maps/", 0);
        map_buf.Insert(".emf", map_buf.Length() + 1);
        file_handle = FileOpen(map_buf.c_str(), 0);
        if (file_handle < 0)
            return false;
        size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buf = new char[size + 1];
        FileRead(file_handle, buf, size);
        FileClose(file_handle);
        map_buf = buf;
        map_buf.SetLength(size);
        delete[] buf;
        if (map_buf[1] != 'E' || map_buf[2] != 'M' || map_buf[3] != 'F')
            return false;
        MapItem map(map_id, DecodeNumber(map_buf.SubString(38, 1)) + 1, DecodeNumber(map_buf.SubString(39, 1)) + 1);
        map.rid1 = DecodeNumber(map_buf.SubString(4, 2));
        map.rid2 = DecodeNumber(map_buf.SubString(6, 2));
        map.map_type = DecodeNumber(map_buf.SubString(32, 1));
        map.timed_effect = DecodeNumber(map_buf.SubString(33, 1));
        map.relog_x = DecodeNumber(map_buf.SubString(44, 1));
        map.relog_y = DecodeNumber(map_buf.SubString(45, 1));
        map.has_hp_drain = false;
        map.has_tp_drain = false;
        map.has_quakes = false;
        map.has_spikes = false;
        if (map.timed_effect == 1)
            map.has_hp_drain = true;
        if (map.timed_effect == MAP_EFFECT_TP_DRAIN)
            map.has_tp_drain = true;
        if (map.timed_effect > 2 && map.timed_effect < 7)
            map.has_quakes = true;
        if (DecodeNumber(map_buf.SubString(43, 1)) == 0)
            map.no_scroll = true;
        map.filesize = size;
        // the start map's file, or every one with memorymap, is kept in memory
        if (start_map == map_id || memory_map)
        {
            map.buf_copied = true;
            map.buf = map_buf;
        }
        else
            map.buf_copied = false;
        // .. after the header: the npc spawns, 8 bytes each, each for a count of one npc
        map_buf.Delete(1, 46);
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            int npc_count = DecodeNumber(map_buf.SubString(8, 1));
            for (int j = 0; j < npc_count; j++)
            {
                int index = (int)map.npc_list.size() + 1;
                Npc *
                    npc = new Npc(index, DecodeNumber(map_buf.SubString(3, 2)), DecodeNumber(map_buf.SubString(1, 1)),
                    DecodeNumber(map_buf.SubString(2, 1)), 0, DecodeNumber(map_buf.SubString(5, 1)),
                    DecodeNumber(map_buf.SubString(6, 2)));
                NpcValue npc_value = GUI->npc_values->GetNpc(DecodeNumber(map_buf.SubString(3, 2)));
                if (npc_value.npc_type == 2)
                {
                    npc->aggressive = true;
                    npc->always_aggressive = true;
                }
                if (npc_value.child > 0)
                    map.child_npc_id = npc->id;
                npc->boss = npc_value.boss;
                npc->child = npc_value.child;
                npc->min_damage = npc_value.min_damage;
                npc->max_damage = npc_value.max_damage;
                npc->accuracy = npc_value.accuracy;
                npc->evade = npc_value.evade;
                npc->armor = npc_value.armor;
                npc->element_weakness = npc_value.element_weakness;
                for (int k = 0; k < ELEMENT_COUNT; k++)
                    npc->element_weakness_damage_table[k] = 0;
                if (npc_value.element_weakness > 0 && npc_value.element_weakness < ELEMENT_COUNT)
                    npc->element_weakness_damage_table[npc_value.element_weakness] = npc_value.element_weakness_damage;
                map.npc_list.insert(map.npc_list.end(), npc);
            }
            map_buf.Delete(1, 8);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            // .. the door keys, 4 bytes each
            int key_x = DecodeNumber(map_buf.SubString(1, 1));
            int key_y = DecodeNumber(map_buf.SubString(2, 1));
            int key_id = DecodeNumber(map_buf.SubString(3, 2));
            AddLockKey(&map, key_x, key_y, key_id);
            map_buf.Delete(1, 4);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            // .. the chest items, 12 bytes each
            int chest_x = DecodeNumber(map_buf.SubString(1, 1));
            int chest_y = DecodeNumber(map_buf.SubString(2, 1));
            int chest_key = DecodeNumber(map_buf.SubString(3, 2));
            int chest_slot = DecodeNumber(map_buf.SubString(5, 1));
            int chest_item = DecodeNumber(map_buf.SubString(6, 2));
            int chest_time = DecodeNumber(map_buf.SubString(8, 2));
            int chest_amount = DecodeNumber(map_buf.SubString(10, 3));
            AddChestSpawn(&map, chest_x, chest_y, chest_key, chest_slot, chest_item, chest_time, chest_amount);
            map_buf.Delete(1, 12);
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            y = DecodeNumber(map_buf.SubString(1, 1));
            tile_count = DecodeNumber(map_buf.SubString(2, 1));
            map_buf.Delete(1, 2);
            // .. the special tiles, a row at a time
            for (int k = 0; k < tile_count; k++)
            {
                x = DecodeNumber(map_buf.SubString(1, 1));
                int code = DecodeNumber(map_buf.SubString(2, 1));
                if (code == MapTileSpec_Wall || code == MapTileSpec_Edge)
                    SetTileBits(&map, x, y, 1);
                if (code > MapTileSpec_Wall && code <= MapTileSpec_NpcBoundary)
                {
                    SetTileBits(&map, x, y, 2);
                    AddTileSpec(&map, x, y, code - 1);
                }
                if (code == MapTileSpec_FakeWall || code == MapTileSpec_Jump)
                {
                    SetTileBits(&map, x, y, 2);
                    AddTileSpec(&map, x, y, TileSpec_NpcBoundary);
                }
                if (code > MapTileSpec_FakeWall && code <= MapTileSpec_Board8)
                {
                    SetTileBits(&map, x, y, 2);
                    AddTileSpec(&map, x, y, code - 1);
                }
                if (code == MapTileSpec_Jukebox)
                {
                    SetTileBits(&map, x, y, 2);
                    AddTileSpec(&map, x, y, code - 1);
                    GUI->jukebox_control->Add(map_id);
                }
                if (code == MapTileSpec_Chest)
                    AddChest(&map, x, y);
                if (code == MapTileSpec_Arena)
                    AddTileSpec(&map, x, y, code - 1);
                if (code > MapTileSpec_AmbientSource && code < 37)
                {
                    AddTileSpec(&map, x, y, code - 1);
                    map.has_spikes = true;
                }
                map_buf.Delete(1, 2);
            }
        }
        count = DecodeNumber(map_buf.SubString(1, 1));
        map_buf.Delete(1, 1);
        for (int i = 0; i < count; i++)
        {
            y = DecodeNumber(map_buf.SubString(1, 1));
            tile_count = DecodeNumber(map_buf.SubString(2, 1));
            map_buf.Delete(1, 2);
            // .. the warps, a row at a time, 8 bytes each; a warp with a key is a door
            for (int k = 0; k < tile_count; k++)
            {
                x = DecodeNumber(map_buf.SubString(1, 1));
                int lock_key = DecodeNumber(map_buf.SubString(7, 2));
                SetTileBits(&map, x, y, 3);
                AddWarp(&map, x, y, DecodeNumber(map_buf.SubString(2, 2)), DecodeNumber(map_buf.SubString(6, 1)),
                    DecodeNumber(map_buf.SubString(4, 1)), DecodeNumber(map_buf.SubString(5, 1)));
                if (lock_key > 0)
                {
                    AddTileSpec(&map, x, y, TileSpec_PassableDoor);
                    SetTileBits(&map, x, y, 2);
                    if (lock_key > 1)
                        AddLockKey(&map, x, y, lock_key);
                }
                map_buf.Delete(1, 8);
            }
        }
        maps.insert(maps.end(), map);
    }
    catch (...)
    {
        FileClose(file_handle);
        return false;
    }
    return true;
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(MapContainer)

DEFINE_MAP_DECODE_NUMBER(MapContainer)

// The chest's slots, or -1 when there is no chest there.
int MapContainer::GetChestSlotCount(int map_id, MapCoord coords)
{
    int result = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapChest>::iterator it = maps[map_id - 1].chest_list.begin();
            it != maps[map_id - 1].chest_list.end(); it++)
        {
            if (it->x == coords.x && it->y == coords.y)
            {
                result = (int)it->slots.size();
                break;
            }
        }
    }
    return result;
}

// The key the door needs, or 0.
int MapContainer::GetDoorKeyAt(int map_id, MapCoord coords)
{
    int result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapObject>::iterator it = maps[map_id - 1].legacy_door_key_list.begin();
            it != maps[map_id - 1].legacy_door_key_list.end(); it++)
        {
            if (it->x == coords.x && it->y == coords.y)
            {
                result = it->value;
                break;
            }
        }
    }
    return result;
}

// The key the chest needs, or 0.
int MapContainer::GetChestKeyAt(int map_id, MapCoord coords)
{
    int result = 0;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapChest>::iterator it = maps[map_id - 1].chest_list.begin();
            it != maps[map_id - 1].chest_list.end(); it++)
        {
            if (it->x == coords.x && it->y == coords.y)
            {
                result = it->key_id;
                break;
            }
        }
    }
    return result;
}

// A player puts an item in the chest: onto a stack of it, or into a new slot that does not come back.
void MapContainer::AddChestItem(int map_id, MapCoord coords, int item_id, int amount)
{
    if (map_id > 0)
    {
        if ((int)maps.size() >= map_id)
        {
            for (vector<MapChest>::iterator chest_iter = maps[map_id - 1].chest_list.begin();
                chest_iter != maps[map_id - 1].chest_list.end(); chest_iter++)
            {
                if (chest_iter->x == coords.x && chest_iter->y == coords.y)
                {
                    char found = 0;
                    for (vector<ChestItem>::iterator it = chest_iter->slots.begin();
                        it != chest_iter->slots.end() && !found; it++)
                    {
                        if (it->item_id != item_id)
                            continue;
                        if (found)
                            continue;
                        if (it->respawn_enabled)
                        {
                            if (it->item_present)
                            {
                                found = 1;
                                it->amount += amount;
                            }
                        }
                        else
                        {
                            found = 1;
                            it->amount += amount;
                        }
                    }
                    if (!found)
                    {
                        ChestItem new_item(item_id);
                        new_item.amount = amount;
                        new_item.item_present = true;
                        new_item.respawn_enabled = false;
                        chest_iter->slots.insert(chest_iter->slots.end(), new_item);
                    }
                    break;
                }
            }
        }
    }
}

// Drops an item on the ground; its index on the map, or -1 off the map.
int MapContainer::AddGroundItem(int map_id, int item_id, int x, int y, int amount, int owner_account_ident,
    short protect_ticks)
{
    int item_index = -1;
    if (map_id > 0 && (int)maps.size() >= map_id && x >= 0 && y >= 0 && maps[map_id - 1].width > x &&
        maps[map_id - 1].height > y)
    {
        ItemObj *item = new ItemObj();
        item->index = maps[map_id - 1].next_ground_item_id;
        item->item_id = item_id;
        item->x = x;
        item->y = y;
        item->amount = amount;
        item->drop_time = DateTimeToTimeStamp(Now());
        item->owner_account_ident = owner_account_ident;
        item->protect_ticks = protect_ticks;
        // .. the indexes go round 0 to 59999: a new quarter first clears the old items in it
        if (maps[map_id - 1].next_ground_item_id == 15000)
            PurgeGroundItemsInRange(map_id, 15000, 30000);
        if (maps[map_id - 1].next_ground_item_id == 30000)
            PurgeGroundItemsInRange(map_id, 30000, 45000);
        if (maps[map_id - 1].next_ground_item_id == 45000)
            PurgeGroundItemsInRange(map_id, 45000, 60000);
        if (maps[map_id - 1].next_ground_item_id == 60000)
            PurgeGroundItemsInRange(map_id, 0, 15000);
        if (maps[map_id - 1].next_ground_item_id >= 60000)
            maps[map_id - 1].next_ground_item_id = 0;
        maps[map_id - 1].ground_items.insert(maps[map_id - 1].ground_items.end(), item);
        item_index = maps[map_id - 1].next_ground_item_id;
        maps[map_id - 1].next_ground_item_id++;
    }
    return item_index;
}

// Takes the item off the ground.
void MapContainer::RemoveGroundItem(int map_id, int index)
{
    if (map_id > 0)
    {
        if ((int)maps.size() >= map_id)
        {
            for (vector<ItemObj *>::iterator cursor = maps[map_id - 1].ground_items.begin();
                cursor != maps[map_id - 1].ground_items.end(); cursor++)
            {
                if ((*cursor)->index == index)
                {
                    ItemObj *item = *cursor;
                    maps[map_id - 1].ground_items.erase(cursor);
                    delete item;
                    break;
                }
            }
        }
    }
}

// Takes an item from the chest: its id and amount, or -1, -1; a slot that comes back starts its time.
MapCoord MapContainer::TakeChestItem(int map_id, MapCoord coords, int item_id)
{
    MapCoord result;
    result.x = -1;
    result.y = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (vector<MapChest>::iterator chest_iter = maps[map_id - 1].chest_list.begin();
            chest_iter != maps[map_id - 1].chest_list.end(); chest_iter++)
        {
            if (chest_iter->x == coords.x && chest_iter->y == coords.y)
            {
                char found = 0;
                for (vector<ChestItem>::iterator it = chest_iter->slots.begin();
                    it != chest_iter->slots.end() && !found; it++)
                {
                    if (it->item_id != item_id)
                        continue;
                    if (found)
                        continue;
                    if (!it->item_present)
                        continue;
                    found = 1;
                    result.x = it->item_id;
                    result.y = it->amount;
                    if (it->respawn_enabled)
                    {
                        it->item_present = false;
                        it->respawn_countdown = it->respawn_delay;
                        it->amount = 0;
                    }
                    else
                        chest_iter->slots.erase(it);
                }
                break;
            }
        }
    }
    return result;
}

// 0 when the player has 10 items on the tile, or another player's item there is still theirs alone.
char MapContainer::CanDropItemAt(int map_id, int x, int y, int account_ident)
{
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        int count = 0;
        for (vector<ItemObj *>::iterator cursor = maps[map_id - 1].ground_items.begin();
            cursor != maps[map_id - 1].ground_items.end(); cursor++)
        {
            if ((*cursor)->x != x)
                continue;
            if ((*cursor)->y != y)
                continue;
            if ((*cursor)->owner_account_ident == (unsigned int)account_ident)
            {
                count++;
                if (count > 9)
                    return 0;
            }
            else
            {
                TTimeStamp now = DateTimeToTimeStamp(Now());
                int elapsed = now.Date - (*cursor)->drop_time.Date;
                int ms = now.Time - (*cursor)->drop_time.Time;
                elapsed = ms / MS_PER_SECOND + elapsed * SECONDS_PER_DAY;
                if ((*cursor)->protect_ticks > elapsed)
                    return 0;
            }
        }
    }
    return 1;
}

// The item on the ground by its index, for the player to pick up:
// x = -1    not there
// x = -2    another player's, still theirs alone
GroundItemInfo MapContainer::GetGroundItemInfo(int map_id, int index, int account_ident)
{
    vector<ItemObj *>::iterator cursor;
    TTimeStamp now;
    int elapsed;
    int ms;
    GroundItemInfo result;
    result.x = -1;
    result.y = -1;
    if (map_id > 0 && (int)maps.size() >= map_id)
    {
        for (cursor = maps[map_id - 1].ground_items.begin(); cursor != maps[map_id - 1].ground_items.end(); cursor++)
        {
            if ((*cursor)->index == index)
            {
                now = DateTimeToTimeStamp(Now());
                elapsed = now.Date - (*cursor)->drop_time.Date;
                ms = now.Time - (*cursor)->drop_time.Time;
                elapsed = SECONDS_PER_DAY * elapsed + ms / MS_PER_SECOND;
                if ((*cursor)->protect_ticks > elapsed && (*cursor)->owner_account_ident != (unsigned int)account_ident &&
                    (*cursor)->owner_account_ident > 0)
                    result.x = -2;
                else
                {
                    result.x = (*cursor)->x;
                    result.y = (*cursor)->y;
                    result.item_id = (*cursor)->item_id;
                    result.amount = (*cursor)->amount;
                }
                break;
            }
        }
    }
    return result;
}

// Clears the ground items with an index from range_low to range_high.
void MapContainer::PurgeGroundItemsInRange(int map_id, int range_low, int range_high)
{
    for (vector<ItemObj *>::iterator cursor = maps[map_id - 1].ground_items.begin();
        cursor != maps[map_id - 1].ground_items.end();)
    {
        if ((*cursor)->index < range_low)
            cursor++;
        else if ((*cursor)->index > range_high)
            cursor++;
        else
        {
            ItemObj *item = *cursor;
            cursor = maps[map_id - 1].ground_items.erase(cursor);
            delete item;
        }
    }
}
