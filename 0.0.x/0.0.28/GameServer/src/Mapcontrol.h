#ifndef MapcontrolH
#define MapcontrolH

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

enum MapId
{
    MapId_PvpMainArena = 46,
    MapId_CastleMaze = 47,
    MapId_PvpRoom1 = 137,
    MapId_PvpRoom2 = 138,
    MapId_SmallArena2 = 183,
    MapId_SmallArena1 = 184
};

enum MapTileSpec
{
    MapTileSpec_Wall = 0,
    MapTileSpec_Chest = 9,
    MapTileSpec_NpcBoundary = 17,
    MapTileSpec_Edge = 18,
    MapTileSpec_FakeWall = 19,
    MapTileSpec_Board8 = 27,
    MapTileSpec_Jukebox = 28,
    MapTileSpec_Jump = 29,
    MapTileSpec_Arena = 32,
    MapTileSpec_AmbientSource = 33
};

struct MapItem;
struct MapObject;
struct MapCoord;
struct Settings;

#include "Map.h"

struct MapContainer;
struct GroundItemInfo;

// The maps, read from maps/<id>.emf: their tiles, warps, doors, chests, npcs and the items on the ground.
struct MapContainer
{
    vector<MapItem> maps;
    unsigned short start_map;
    int start_x;
    int start_y;
    short rescue_map;
    int rescue_x;
    int rescue_y;
    unsigned short max_maps;
    char memory_map;
    Settings *settings;
    char *encode_scratch;

    MapContainer(Settings *settings);
    ~MapContainer();
    void SetArenaBlock(int map_id, int block);
    void AddArenaSpawn(int map_id, int from_x, int from_y, int to_x, int to_y);
    void IncPlayerCount(int map_id);
    void DecPlayerCount(int map_id);
    void SetTileBits(MapItem *map, int x, int y, int code);
    String BuildChestItemsString(int map_id, MapCoord coords);
    int GetWarpMap(int map_id, int x, int y);
    int GetWarpLevelReq(int map_id, int x, int y);
    int GetWarpX(int map_id, int x, int y);
    int GetWarpY(int map_id, int x, int y);
    char OpenDoor(int map_id, int x, int y);
    MapObject GetTileSpecObject(int map_id, int x, int y);
    int GetTileSpec(int map_id, int x, int y);
    int GetTileSpecValueAt(int map_id, int x, int y);
    char TryTakeQuestCooldown(int map_id);
    char GetNoScroll(int map_id);
    MapCoord GetRelogCoords(int map_id);
    char AggroChildNpcs(int map_id);
    char KillChildNpcs(int map_id);
    int GetNpcIdByIndex(int map_id, int npc_index);
    MapCoord GetNpcCoordsByIndex(int map_id, int npc_index);
    char IsOccupied(int map_id, int x, int y);
    int CountNpcsChasingPlayer(int map_id, int player_id);
    char IsDropTileClear(int map_id, int x, int y);
    char IsTileClear(int map_id, int x, int y);
    char IsWarpTile(int map_id, int x, int y);
    int CountOpenNeighbors(int map_id, int x, int y);
    int GetTileBlock(int map_id, int x, int y, char ignore_spec_block);
    void AddTileSpec(MapItem *map, int x, int y, int spec);
    void AddWarp(MapItem *map, int x, int y, int dest_map, int level, int dest_x, int dest_y);
    void AddLockKey(MapItem *map, int x, int y, int key_id);
    void AddChest(MapItem *map, int x, int y);
    void AddChestSpawn(MapItem *map, int x, int y, int key_id, int slot, int item_id, int spawn_time, int amount);
    void LoadMaps();
    void ResetMap(int map_id);
    String ReadRawFile(int map_id);
    char ReloadMap(int map_id);
    bool ParseMapFile(MapItem *map, int map_id);
    bool LoadMap(int map_id);
    String EncodeNumber(unsigned int value, signed int width);
    int DecodeNumber(String value);
    int GetChestSlotCount(int map_id, MapCoord coords);
    int GetDoorKeyAt(int map_id, MapCoord coords);
    int GetChestKeyAt(int map_id, MapCoord coords);
    void AddChestItem(int map_id, MapCoord coords, int item_id, int amount);
    int AddGroundItem(int map_id, int item_id, int x, int y, int amount, int owner_account_ident, short protect_ticks);
    void RemoveGroundItem(int map_id, int index);
    MapCoord TakeChestItem(int map_id, MapCoord coords, int item_id);
    char CanDropItemAt(int map_id, int x, int y, int account_ident);
    GroundItemInfo GetGroundItemInfo(int map_id, int index, int account_ident);
    void PurgeGroundItemsInRange(int map_id, int range_low, int range_high);
};

// An item picked up from the ground: where it lay, what and how many.
struct GroundItemInfo
{
    struct
    {
        int x;
        int y;
        int item_id;
        int amount;
    };

    GroundItemInfo()
    {
    }
};


#endif
