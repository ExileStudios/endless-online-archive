#ifndef MapprotectionH
#define MapprotectionH

enum WarpTimeoutAction
{
    WarpTimeoutAction_AcceptWarp = 1,
    WarpTimeoutAction_ConfirmWarp = 2
};

#include <fstream>
#include <new>
#include <string>
#include <vector.h>

struct PathFinder;

#include "Mapdoor.h"
#include "Mapobject.h"
#include "Mapsign.h"
#include "Mapwarp.h"
#include "Mapglow.h"

#define PVP_MAP_TYPE 3
#define TILE_HALF_HEIGHT 16
#define TILE_HALF_WIDTH 32

enum MusicMode
{
    MusicMode_OnceOnChange = 0,
    MusicMode_OnceRestart = 1,
    MusicMode_OnceIfSilent = 2,
    MusicMode_LoopOnChange = 3,
    MusicMode_LoopRestart = 4,
    MusicMode_LoopIfSilent = 5,
    MusicMode_Stop = 6
};

// The current map: its tile layers, doors, chests, warps and signs, the view centre and scrolling, and warp handling.
struct MapController
{
    bool state_flag;
    bool item_use_restricted;
    unsigned char warp_countdown;
    bool warp_pending;
    bool loading_map;
    bool map_request_pending;
    int map_id;
    int map_file_size;
    int warp_session_id;
    String map_data;
    unsigned short checksum_high;
    unsigned short checksum_low;
    int file_size;
    unsigned short ground_tiles[250][250];
    unsigned short top_tiles[250][250];
    unsigned short walls[250][250];
    unsigned short walls_next_row[250][250];
    unsigned short objects[250][250];
    unsigned short masks[250][250];
    unsigned short overlay_tiles[250][250];
    unsigned short blend_tiles[250][250];
    unsigned short top_masks[250][250];
    char blocked[250][250];
    char has_tile_spec[250][250];
    unsigned char spec_grid[250][250];
    unsigned char minimap_mask[250][250];
    char edge_left[250][250];
    char edge_top[250][250];
    char edge_right[250][250];
    char edge_bottom[250][250];
    int trap_reveal_timer;
    int tile_frame;
    bool has_animated_tiles;
    vector<MapObject> tile_specs;
    vector<MapObject> chest_keys;
    vector<MapDoor> doors;
    vector<MapWarp> warps;
    vector<MapSign> signs;
    vector<MapGlow> glows;
    PathFinder *path_finder;
    String name;
    int map_type;
    int map_attribute;
    int music_id;
    int playing_music_id;
    int music_mode;
    int ambient_sound;
    int loaded_map_id;
    bool minimap_disabled;
    int height;
    int width;
    int center_x;
    int center_y;
    int cursor_x;
    int cursor_y;
    int scroll_shift_x;
    int scroll_shift_y;
    int scroll_offset_x;
    int scroll_offset_y;
    bool scroll_requested;
    bool scrolling;
    int scroll_direction;
    int scroll_step;
    bool frozen;
    bool action_blocked;
    int action_block_tick;
    bool has_boss;
    bool cursor_moved;
    bool cursor_update_pending;
    int last_cursor_x;
    int last_cursor_y;
    int warp_timeout;
    int warp_timeout_action;
    int warp_delay;
    bool path_marker_shown;
    int path_marker_x;
    int path_marker_y;
    int path_marker_timer;

    MapController();
    ~MapController();
    void AdvanceTileFrame();
    void ReadMapChecksum(int map_number);
    void SaveMapFile(int map_number);
    void LoadMap(int map_number);
    void MarkEdgeTiles();
    bool IsWallOrSpecTile(int x, int y);
    char IsBlocked(int x, int y);
    void SetChestKey(int x, int y, int key);
    void OpenDoor(int x, int y);
    void AddDoor(int x, int y, int key);
    void SetTileSpec(int x, int y, int spec);
    void AddWarp(int x, int y, int dest_map, int level_required, int dest_x, int dest_y);
    void AddSign(int x, int y, String text);
    void SetWarpTimeoutAction(int action);
    void StartWarpTimeout();
    void StopWarpTimeout();
    void ClearTileObjects();
    void ClearSigns();
    String FindSignTextAt(int x, int y);
    TPoint FindLockerAt(int x, int y);
    TPoint FindChestAt(int x, int y);
    TPoint FindBoardAt(int x, int y);
    TPoint FindJukeboxAt(int x, int y);
    TPoint FindDoorAt(int x, int y);
    int GetWarpLevel(int x, int y);
    int GetChairSpec(int x, int y);
    int GetChestKey(int x, int y);
    int GetDoorKey(int x, int y);
    int IsNearTrap();
    int GetTileSpec(int x, int y);
    int GetUsableTileSpec(int x, int y);
    void ClearWarps();
    char ConsumeCursorMoved();
    void UpdateCursorTile(int screen_x, int screen_y);
    int GetCursorX();
    int GetCursorY();
    void Animate();
    int GetScrollShiftX();
    int GetScrollShiftY();
    void AcceptPendingWarp();
    int GetScrollOffsetX();
    int GetScrollOffsetY();
    void ClearMap();
    String GetName();
    int GetMapType();
    int GetMapAttribute();
    int GetMusicId();
    int GetMusicMode();
    void ClearMusic();
    int GetMapId();
    void SetCenterX(int x);
    void SetCenterY(int y);
    int GetHeight();
    int GetWidth();
    int GetCenterX();
    int GetCenterY();
    int GetScrollStep();
    int GetScrollDirection();
    bool IsMinimapDisabled();
    bool IsWarpDelayed();
    void DelayWarp();
    void StartScroll(int direction);
    bool IsScrolling();
    bool IsCursorUpdatePending();
    void BlockActionsBriefly();
    void FreezePlayer();
    void UnfreezePlayer();
    bool IsActionBlocked();
    bool HasBoss();
    bool HasPathMarker();
    void SetPathMarker(int x, int y);
    int GetPathMarkerX();
    int GetPathMarkerY();
    int GetPathMarkerTimer();
};

#endif
