#include <vcl.h>
#pragma hdrstop

#include "Mapprotection.h"
#include "Tmainform.h"
#include "Pathfinder.h"
#include "Gfx_mapwalls.h"

#pragma package(smart_init)

// An empty map with no view centre, no scrolling and no warp in progress, and its path finder.
MapController::MapController()
{
    ClearMap();
    path_finder = new PathFinder();
    warp_pending = false;
    loading_map = false;
    trap_reveal_timer = 0;
    tile_frame = 0;
    has_animated_tiles = false;
    height = 0;
    width = 0;
    center_x = 0;
    center_y = 0;
    cursor_x = 0;
    cursor_y = 0;
    scroll_requested = false;
    scrolling = false;
    scroll_direction = FacingDirection_Down;
    scroll_step = 0;
    state_flag = false;
    item_use_restricted = false;
    cursor_moved = true;
    cursor_update_pending = false;
    action_blocked = false;
    frozen = false;
    has_boss = false;
    map_request_pending = false;
    loading_map = false;
    warp_timeout = 0;
    warp_delay = 0;
    map_data = "";
}

// Steps the four-frame tile animation and asks for a redraw when the trap reveal ends or a tile animated.
void MapController::AdvanceTileFrame()
{
    tile_frame++;
    if (tile_frame > 3)
        tile_frame = 0;
    if (trap_reveal_timer > 0)
    {
        trap_reveal_timer--;
        if (!trap_reveal_timer)
            MainForm->redraw_needed = true;
    }
    if (has_animated_tiles)
    {
        has_animated_tiles = false;
        MainForm->redraw_needed = true;
    }
}

// The map's name from its file, cut at the first 0xFF filler; empty when the file is not a map.
String MapController::ReadMapName(int map_number)
{
    int file;
    int size;
    char *buffer;
    String map_name = "";
    String data;
    try
    {
        data = IntToStr(map_number);
        for (int length = data.Length(); length <= 4; length++)
            data.Insert("0", 0);
        data.Insert("./maps/", 0);
        data.Insert(".emf", data.Length() + 1);
        file = FileOpen(data.c_str(), fmOpenRead);
        size = FileSeek(file, 0, 2);
        FileSeek(file, 0, 0);
        buffer = new char[size + 1];
        FileRead(file, buffer, size);
        FileClose(file);
        data = buffer;
        file_size = size;
        data.SetLength(size);
        delete[] buffer;
        if (data[1] != 'E' || data[2] != 'M' || data[3] != 'F')
            return "";
        String header = MainForm->net_client->DeobfuscateString(data.SubString(8, 24));
        map_name = header.SubString(1, 16);
        for (int i = 1; i <= map_name.Length(); i++)
        {
            if ((unsigned char)map_name[i] == 255)
            {
                map_name = map_name.SubString(1, i - 1);
                break;
            }
        }
    }
    catch (...)
    {
    }
    return map_name;
}

// Reads the checksum and size of the local map file, for comparison with the ones the server sends.
void MapController::ReadMapChecksum(int map_number)
{
    int file;
    int size;
    char *file_handle;
    String data;
    try
    {
        data = IntToStr(map_number);
        for (int length = data.Length(); length <= 4; length++)
            data.Insert("0", 0);
        data.Insert("./maps/", 0);
        data.Insert(".emf", data.Length() + 1);
        file = FileOpen(data.c_str(), fmOpenRead);
        size = FileSeek(file, 0, 2);
        FileSeek(file, 0, 0);
        file_handle = new char[size + 1];
        FileRead(file, file_handle, size);
        FileClose(file);
        data = file_handle;
        file_size = size;
        data.SetLength(size);
        delete[] file_handle;
        if (data[1] != 'E' || data[2] != 'M' || data[3] != 'F')
        {
            checksum_high = 0;
            checksum_low = 0;
            return;
        }
        checksum_high = MainForm->net_client->DecodeNumber(data.SubString(4, 2));
        checksum_low = MainForm->net_client->DecodeNumber(data.SubString(6, 2));
    }
    catch (...)
    {
    }
}

// Writes the downloaded map data to the map's file.
void MapController::SaveMapFile(int map_number)
{
    String lp_file_name = IntToStr(map_number);
    int i = lp_file_name.Length();
    for (; i <= 4; i++)
        lp_file_name.Insert("0", 0);
    lp_file_name.Insert("./maps/", 0);
    lp_file_name.Insert(".emf", lp_file_name.Length() + 1);
    basic_ofstream<char, char_traits<char> > handle;
    handle.open(lp_file_name.c_str(), std::ios::binary);
    handle << map_data.c_str();
    handle.close();
}

// Loads a map from its .emf file: the name and settings, then the npc spawns, chests, tile kinds, warps, doors and
// signs, and every graphic layer; it starts the map's music and marks the edge tiles.
void MapController::LoadMap(int map_number)
{
    int spec_x;
    int spec_y;
    int row_spec_count;
    int h_file;
    int n_number_of_bytes_to_read;
    char *file_handle;
    String data;
    try
    {
        data = IntToStr(map_number);
        loaded_map_id = map_number;
        for (int i = data.Length(); i <= 4; i++)
            data.Insert("0", 0);
        data.Insert("./maps/", 0);
        data.Insert(".emf", data.Length() + 1);
        h_file = FileOpen(data.c_str(), fmOpenRead);
        n_number_of_bytes_to_read = FileSeek(h_file, 0, 2);
        FileSeek(h_file, 0, 0);
        file_handle = new char[n_number_of_bytes_to_read + 1];
        FileRead(h_file, file_handle, n_number_of_bytes_to_read);
        FileClose(h_file);
        data = file_handle;
        data.SetLength(n_number_of_bytes_to_read);
        delete[] file_handle;
        if (data[1] != 'E' || data[2] != 'M' || data[3] != 'F')
            return;
        String header = MainForm->net_client->DeobfuscateString(data.SubString(8, 24));
        name = header.SubString(1, 23);
        map_type = MainForm->net_client->DecodeNumber(data.SubString(32, 1));
        map_attribute = MainForm->net_client->DecodeNumber(data.SubString(33, 1));
        music_id = MainForm->net_client->DecodeNumber(data.SubString(34, 1));
        music_mode = MainForm->net_client->DecodeNumber(data.SubString(35, 1));
        ambient_sound = MainForm->net_client->DecodeNumber(data.SubString(36, 2));
        width = MainForm->net_client->DecodeNumber(data.SubString(38, 1));
        height = MainForm->net_client->DecodeNumber(data.SubString(39, 1));
        int count;
        int fill_tile = MainForm->net_client->DecodeNumber(data.SubString(40, 2));
        int minimap_setting = MainForm->net_client->DecodeNumber(data.SubString(42, 1));
        int item_setting = MainForm->net_client->DecodeNumber(data.SubString(43, 1));
        int trailing_header_value = MainForm->net_client->DecodeNumber(data.SubString(46, 1));
        for (spec_x = 1; spec_x <= name.Length(); spec_x++)
        {
            if ((unsigned char)name[spec_x] == 255)
            {
                name = name.SubString(1, spec_x - 1);
                break;
            }
        }
        ClearMap();
        for (int k = 0; k <= 249; k++)
        {
            for (int m = 0; m <= 249; m++)
                ground_tiles[k][m] = fill_tile;
        }
        if (!minimap_setting)
            minimap_disabled = true;
        else
            minimap_disabled = false;
        if (!item_setting)
            item_use_restricted = true;
        else
            item_use_restricted = false;
        data.Delete(1, 46);
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int npc_index = 0; npc_index < count; npc_index++)
        {
            int npc_id = MainForm->net_client->DecodeNumber(data.SubString(3, 2));
            NpcValue npc_value = MainForm->npc_values->GetNpcByIndex(npc_id - 1);
            if (npc_value.boss > 0)
                has_boss = true;
            data.Delete(1, 8);
        }
        // Doors: x, y and a two-byte key.
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int n = 0; n < count; n++)
        {
            int door_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            int door_y = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            int door_key = MainForm->net_client->DecodeNumber(data.SubString(3, 2));
            AddDoor(door_x, door_y, door_key);
            data.Delete(1, 4);
        }
        // Chests: x, y and a two-byte key in a twelve-byte record.
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int ii = 0; ii < count; ii++)
        {
            int chest_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            int chest_y = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            int chest_key = MainForm->net_client->DecodeNumber(data.SubString(3, 2));
            SetChestKey(chest_x, chest_y, chest_key);
            data.Delete(1, 12);
        }
        // Spec rows: y and a count, then x and spec pairs.
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int jj = 0; jj < count; jj++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int kk = 0; row_spec_count > kk; kk++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                unsigned char spec = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
                spec_grid[spec_x][spec_y] = spec;
                if (!spec)
                {
                    blocked[spec_x][spec_y] = true; has_tile_spec[spec_x][spec_y] = false;
                }
                // A spec is stored one lower than in the file.
                if (spec > MapTileSpec_Wall && spec <= MapTileSpec_BankVault)
                {
                    objects[spec_x][spec_y] = 0; has_tile_spec[spec_x][spec_y] = true;
                    SetTileSpec(spec_x, spec_y, spec - 1);
                    if (spec == MapTileSpec_Door)
                        AddDoor(spec_x, spec_y, 0);
                }
                if (spec == MapTileSpec_Edge)
                {
                    blocked[spec_x][spec_y] = true;
                    has_tile_spec[spec_x][spec_y] = false;
                }
                if (spec == MapTileSpec_FakeWall)
                {
                }
                if (spec > MapTileSpec_FakeWall && spec <= MapTileSpec_Jukebox)
                {
                    objects[spec_x][spec_y] = 0; has_tile_spec[spec_x][spec_y] = true;
                    SetTileSpec(spec_x, spec_y, spec - 1);
                }
                if (spec >= MapTileSpec_Jump && spec <= MapTileSpec_HiddenSpikes)
                    SetTileSpec(spec_x, spec_y, spec - 1);
                data.Delete(1, 2);
            }
        }
        // Warp rows: y and a count, then eight-byte records.
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int mm = 0; mm < count; mm++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int nn = 0; row_spec_count > nn; nn++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                int warp_map = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                int warp_level = MainForm->net_client->DecodeNumber(data.SubString(6, 1));
                // A warp record: x, map, destination x and y, level, door.
                int warp_door = MainForm->net_client->DecodeNumber(data.SubString(7, 2));
                blocked[spec_x][spec_y] = false;
                has_tile_spec[spec_x][spec_y] = false;
                if (!warp_level)
                    minimap_mask[spec_x][spec_y] = 1;
                else
                    minimap_mask[spec_x][spec_y] = 2;
                AddWarp(spec_x, spec_y, warp_map, warp_level, MainForm->net_client->DecodeNumber(data.SubString(4, 1)),
                    MainForm->net_client->DecodeNumber(data.SubString(5, 1)));
                if (warp_door > 0)
                {
                    has_tile_spec[spec_x][spec_y] = true;
                    if (warp_door == 1)
                        AddDoor(spec_x, spec_y, 0);
                    else
                        AddDoor(spec_x, spec_y, warp_door);
                    SetTileSpec(spec_x, spec_y, TileSpec_Door);
                }
                data.Delete(1, 8);
            }
        }
        // Each tile layer: y and a count, then x and a two-byte graphic.
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int ground_row = 0; ground_row < count; ground_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int ground_column = 0; row_spec_count > ground_column; ground_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                ground_tiles[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int object_row = 0; object_row < count; object_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int object_column = 0; row_spec_count > object_column; object_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                objects[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int mask_row = 0; mask_row < count; mask_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int mask_column = 0; row_spec_count > mask_column; mask_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                masks[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int wall_row = 0; wall_row < count; wall_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int wall_column = 0; row_spec_count > wall_column; wall_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                walls[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int next_row_wall_row = 0; next_row_wall_row < count; next_row_wall_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int next_row_wall_column = 0; row_spec_count > next_row_wall_column; next_row_wall_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                walls_next_row[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int top_row = 0; top_row < count; top_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int top_column = 0; row_spec_count > top_column; top_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                top_tiles[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int overlay_row = 0; overlay_row < count; overlay_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int overlay_column = 0; row_spec_count > overlay_column; overlay_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                overlay_tiles[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int blend_row = 0; blend_row < count; blend_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int blend_column = 0; row_spec_count > blend_column; blend_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                blend_tiles[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
        data.Delete(1, 1);
        for (int top_mask_row = 0; top_mask_row < count; top_mask_row++)
        {
            spec_y = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            row_spec_count = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
            data.Delete(1, 2);
            for (int top_mask_column = 0; row_spec_count > top_mask_column; top_mask_column++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                top_masks[spec_x][spec_y] = MainForm->net_client->DecodeNumber(data.SubString(2, 2));
                data.Delete(1, 3);
            }
        }
        // Signs: x, y, a two-byte length and the encoded text.
        if (data.Length() > 0)
        {
            count = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
            data.Delete(1, 1);
            for (int sign_index = 0; sign_index < count; sign_index++)
            {
                spec_x = MainForm->net_client->DecodeNumber(data.SubString(1, 1));
                spec_y = MainForm->net_client->DecodeNumber(data.SubString(2, 1));
                int sign_length = MainForm->net_client->DecodeNumber(data.SubString(3, 2));
                data.Delete(1, 4);
                String sign_text = MainForm->net_client->DeobfuscateString(data.SubString(1, sign_length));
                data.Delete(1, sign_length);
                minimap_mask[spec_x][spec_y] = 3;
                AddSign(spec_x, spec_y, sign_text);
            }
        }
        if (music_id > 0)
        {
            // Modes 0 and 3 change the music only when it differs or has stopped.
            if ((music_mode == MusicMode_OnceOnChange || music_mode == MusicMode_LoopOnChange) &&
                (music_id != playing_music_id || !MainForm->sfx->IsMusicPlaying()))
            {
                MainForm->sfx->QueueMusic(music_id);
                playing_music_id = music_id;
            }
            // Modes 1 and 4 always restart the music.
            if (music_mode == MusicMode_OnceRestart || music_mode == MusicMode_LoopRestart)
            {
                MainForm->sfx->QueueMusic(music_id);
                playing_music_id = music_id;
            }
            // Modes 2 and 5 play only into silence.
            if ((music_mode == MusicMode_OnceIfSilent || music_mode == MusicMode_LoopIfSilent) &&
                !MainForm->sfx->IsMusicPlaying())
            {
                MainForm->sfx->QueueMusic(music_id);
                playing_music_id = music_id;
            }
        }
        else
        {
            if (music_mode >= MusicMode_Stop)
                MainForm->sfx->StopMusic();
            music_mode = MusicMode_OnceOnChange;
        }
        if (music_mode >= MusicMode_Stop)
        {
            MainForm->sfx->StopMusic();
            music_mode = MusicMode_OnceOnChange;
        }
    }
    catch (...)
    {
    }
    MarkEdgeTiles();
}

// Marks each tile that borders the map's edge or a tile of kind 18 on any side, for the minimap's edge lines.
void MapController::MarkEdgeTiles()
{
    int x;
    unsigned char last_x = GetWidth();
    unsigned char last_y = GetHeight();
    for (int y = 0; y <= GetHeight(); y++)
    {
        for (x = 0; x <= GetWidth(); x++)
        {
            edge_bottom[x][y] = false;
            edge_top[x][y] = false;
            edge_left[x][y] = false;
            edge_right[x][y] = false;
            if (spec_grid[x][y] != 18)
            {
                if (!x)
                    edge_left[x][y] = true;
                if (!y)
                    edge_top[x][y] = true;
                if (x == last_x)
                    edge_right[x][y] = true;
                if (y == last_y)
                    edge_bottom[x][y] = true;
                if (x > 0 && spec_grid[x - 1][y] == 18)
                    edge_left[x][y] = true;
                if (x < last_x && spec_grid[x + 1][y] == 18)
                    edge_right[x][y] = true;
                if (y > 0 && spec_grid[x][y - 1] == 18)
                    edge_top[x][y] = true;
                if (y < last_y && spec_grid[x][y + 1] == 18)
                    edge_bottom[x][y] = true;
            }
        }
    }
}

// Whether the tile is inside the map and is blocked or holds a tile spec.
bool MapController::IsWallOrSpecTile(int x, int y)
{
    bool solid = false;
    if (x >= 0 && y >= 0 && x <= width && y <= height && (blocked[x][y] || has_tile_spec[x][y]))
        solid = true;
    return solid;
}

// Whether the tile can not be walked on; a spec tile is open only when its spec is an open door or 11.
char MapController::IsBlocked(int x, int y)
{
    char tile_blocked = false;
    if (x >= 0 && y >= 0 && x <= width && y <= height)
    {
        if (blocked[x][y])
            tile_blocked = true;
        if (has_tile_spec[x][y])
        {
            tile_blocked = true;
            for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
            {
                if (x == it->x && y == it->y)
                {
                    if ((short)it->spec == TileSpec_OpenDoor || (short)it->spec == TileSpec_Passable)
                        tile_blocked = false;
                    break;
                }
            }
        }
    }
    return tile_blocked;
}

// Sets the key of the chest on the tile, adding the chest when there is none.
void MapController::SetChestKey(int x, int y, int key)
{
    char found = false;
    for (vector<MapObject>::iterator it = chest_keys.begin(); it != chest_keys.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            it->spec = key;
            found = true;
            break;
        }
    }
    if (!found)
    {
        MapObject handle(x, y, key);
        chest_keys.insert(chest_keys.end(), handle);
    }
}

// Shows the closed door on the tile as open, starts its close timer and marks its tile spec as an open door.
// The open graphic is the one after the closed graphic, on the wall layer the door is drawn on.
void MapController::OpenDoor(int x, int y)
{
    char door_found = false;
    vector<MapDoor>::iterator it;
    for (it = doors.begin(); it != doors.end(); it++)
    {
        if (it->x == x && it->y == y && !it->open)
        {
            int closed_graphic = 0;
            door_found = true;
            if (walls[x][y] > 0)
            {
                closed_graphic = walls[x][y];
                walls[x][y]++;
                MainForm->gfx->walls->LoadGraphicNow(walls[x][y]);
                MainForm->redraw_needed = true;
            }
            else if (walls_next_row[x][y] > 0)
            {
                closed_graphic = walls_next_row[x][y];
                walls_next_row[x][y]++;
                MainForm->gfx->walls->LoadGraphicNow(walls_next_row[x][y]);
                MainForm->redraw_needed = true;
            }
            it->close_timer = 24;
            it->open = true;
            if (it->closed_graphic <= 0)
                it->closed_graphic = closed_graphic;
            break;
        }
    }
    if (door_found)
    {
        for (vector<MapObject>::iterator walk = tile_specs.begin(); walk != tile_specs.end(); walk++)
        {
            if (walk->x == x && walk->y == y && (short)walk->spec == TileSpec_Door)
            {
                walk->spec = TileSpec_OpenDoor;
                break;
            }
        }
    }
}

// Adds a door on the tile, or sets the key of the door already there.
void MapController::AddDoor(int x, int y, int key)
{
    char found = false;
    for (vector<MapDoor>::iterator it = doors.begin(); it != doors.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            if (key > 0)
                it->key = key;
            found = true;
            break;
        }
    }
    if (!found)
    {
        MapDoor handle(x, y, key);
        doors.insert(doors.end(), handle);
    }
}

void MapController::SetTileSpec(int x, int y, int spec)
{
    MapObject handle(x, y, spec);
    tile_specs.insert(tile_specs.end(), handle);
}

void MapController::AddWarp(int x, int y, int dest_map, int level_required, int dest_x, int dest_y)
{
    MapWarp handle(x, y, dest_map, level_required, dest_x, dest_y);
    warps.insert(warps.end(), handle);
}

void MapController::AddSign(int x, int y, String text)
{
    MapSign handle(x, y, text);
    signs.insert(signs.end(), handle);
}

void MapController::SetWarpTimeoutAction(int action)
{
    warp_timeout_action = action;
}

void MapController::StartWarpTimeout()
{
    warp_timeout = 20;
}

void MapController::StopWarpTimeout()
{
    warp_timeout = 0;
}

void MapController::ClearTileObjects()
{
    tile_specs.clear();
    chest_keys.clear();
    doors.clear();
}

void MapController::ClearSigns()
{
    signs.clear();
}

// The text of the sign under the screen point, or empty.
// Tiles lie on a diamond grid: a step in x moves right and down, a step in y moves left and down.
String MapController::FindSignTextAt(int x, int y)
{
    int screen_y;
    String text = "";
    for (vector<MapSign>::iterator it = signs.begin(); it != signs.end(); it++)
    {
        int screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
        screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
        if (screen_x - 12 <= x && screen_x + 23 >= x && screen_y <= y && screen_y + 62 >= y)
        {
            text = it->text;
            break;
        }
    }
    return text;
}

// The tile of the locker under the screen point, or -1, -1.
TPoint MapController::FindLockerAt(int x, int y)
{
    int screen_x;
    int screen_y;
    TPoint tile;
    tile.x = -1;
    tile.y = -1;
    for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if ((short)it->spec == TileSpec_Locker)
        {
            screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
            screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
            if (screen_x - 15 <= x && screen_x + 28 >= x && screen_y - 16 <= y && screen_y + 67 >= y)
            {
                tile.x = it->x;
                tile.y = it->y;
                break;
            }
        }
    }
    return tile;
}

// The tile of the chest under the screen point, or -1, -1.
TPoint MapController::FindChestAt(int x, int y)
{
    int screen_x;
    int screen_y;
    TPoint tile;
    tile.x = -1;
    tile.y = -1;
    for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if ((short)it->spec == TileSpec_Chest)
        {
            screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
            screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
            if (screen_x - 12 <= x && screen_x + 25 >= x && screen_y + 16 <= y && screen_y + 67 >= y)
            {
                tile.x = it->x;
                tile.y = it->y;
                break;
            }
        }
    }
    return tile;
}

// The tile of the board under the screen point, or -1, -1.
TPoint MapController::FindBoardAt(int x, int y)
{
    int screen_x;
    int screen_y;
    TPoint tile;
    tile.x = -1;
    tile.y = -1;
    for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if (it->spec >= TileSpec_Board1 && it->spec <= TileSpec_Board8)
        {
            screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
            screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
            if (screen_x - 18 <= x && screen_x + 33 >= x && screen_y - 10 <= y && screen_y + 66 >= y)
            {
                tile.x = it->x;
                tile.y = it->y;
                break;
            }
        }
    }
    return tile;
}

// The tile of the jukebox under the screen point, or -1, -1.
TPoint MapController::FindJukeboxAt(int x, int y)
{
    int screen_x;
    int screen_y;
    TPoint tile;
    tile.x = -1;
    tile.y = -1;
    for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if ((short)it->spec == TileSpec_Jukebox)
        {
            screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
            screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
            if (screen_x - 18 <= x && screen_x + 33 >= x && screen_y - 10 <= y && screen_y + 66 >= y)
            {
                tile.x = it->x;
                tile.y = it->y;
                break;
            }
        }
    }
    return tile;
}

// The tile of the closed door under the screen point, or -1, -1.
TPoint MapController::FindDoorAt(int x, int y)
{
    int screen_x;
    int screen_y;
    TPoint tile;
    tile.x = -1;
    tile.y = -1;
    for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if ((short)it->spec == TileSpec_Door)
        {
            screen_x = TILE_HALF_WIDTH * it->x - TILE_HALF_WIDTH * it->y + MainForm->map->GetScrollOffsetX() + 310;
            screen_y = TILE_HALF_HEIGHT * it->x + TILE_HALF_HEIGHT * it->y + MainForm->map->GetScrollOffsetY() + 102;
            if (screen_x - 22 <= x && screen_x + 35 >= x && screen_y - 56 <= y && screen_y + 67 >= y)
            {
                tile.x = it->x;
                tile.y = it->y;
                break;
            }
        }
    }
    return tile;
}

// The level a warp on the tile needs, or -1 when there is no warp.
int MapController::GetWarpLevel(int x, int y)
{
    int level = -1;
    vector<MapWarp>::iterator it;
    for (it = warps.begin(); it != warps.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            level = it->level_required;
            break;
        }
    }
    return level;
}

// The chair spec of the tile, or -1 when it holds no chair.
int MapController::GetChairSpec(int x, int y)
{
    int spec = -1;
    if (has_tile_spec[x][y])
    {
        for (vector<MapObject>::iterator it = tile_specs.begin(); it != tile_specs.end(); it++)
        {
            if (x == it->x && y == it->y)
            {
                if (it->spec <= TileSpec_ChairAnySide)
                    spec = it->spec;
            }
        }
    }
    return spec;
}

// The key of the chest on the tile, or -1.
int MapController::GetChestKey(int x, int y)
{
    int key = -1;
    vector<MapObject>::iterator it;
    for (it = chest_keys.begin(); it != chest_keys.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            key = it->spec;
            break;
        }
    }
    return key;
}

// The key of the door on the tile, or -1.
int MapController::GetDoorKey(int x, int y)
{
    int key = -1;
    vector<MapDoor>::iterator it;
    for (it = doors.begin(); it != doors.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            key = it->key;
            break;
        }
    }
    return key;
}

// A level for the nearest trap tile, -1000 less 160 for each step from the view centre; -3000 when there is none.
int MapController::IsNearTrap()
{
    vector<MapObject>::iterator it;
    int dy;
    int is_near = -3000;
    for (it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if ((short)it->spec == TileSpec_Trap)
        {
            int dx = abs(it->x - GetCenterX());
            dy = abs(it->y - GetCenterY());
            dy = -1000 - 160 * (dx + dy);
            if (dy > is_near)
                is_near = dy;
        }
    }
    return is_near;
}

// The tile spec on the tile, or -1.
int MapController::GetTileSpec(int x, int y)
{
    int spec = -1;
    vector<MapObject>::iterator it;
    for (it = tile_specs.begin(); it != tile_specs.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            spec = it->spec;
            break;
        }
    }
    return spec;
}

// The spec of the tile when the player can use it from the current tile, or -1.
int MapController::GetUsableTileSpec(int x, int y)
{
    vector<MapObject>::iterator it;
    int spec = -1;
    if (has_tile_spec[x][y])
    {
        for (it = tile_specs.begin(); it != tile_specs.end(); it++)
        {
            if (x == it->x && it->y == y)
            {
                if (it->spec <= TileSpec_ChairAnySide)
                {
                    int player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
                    int player_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
                    short spec_above = it->spec;
                    if ((spec_above == TileSpec_ChairYHigh || spec_above == TileSpec_ChairYHighXHigh ||
                        spec_above == TileSpec_ChairAnySide) && player_x == it->x && it->y == player_y - 1)
                        spec = it->spec;
                    short spec_right = it->spec;
                    if ((spec_right == TileSpec_ChairXLow || spec_right == TileSpec_ChairXLowYLow ||
                        spec_right == TileSpec_ChairAnySide) && it->x == player_x + 1 && player_y == it->y)
                        spec = it->spec;
                    short spec_left = it->spec;
                    if ((spec_left == TileSpec_ChairXHigh || spec_left == TileSpec_ChairYHighXHigh ||
                        spec_left == TileSpec_ChairAnySide) && it->x == player_x - 1 && player_y == it->y)
                        spec = it->spec;
                    short spec_below = it->spec;
                    if ((spec_below == TileSpec_ChairYLow || spec_below == TileSpec_ChairXLowYLow ||
                        spec_below == TileSpec_ChairAnySide) && player_x == it->x && it->y == player_y + 1)
                        spec = it->spec;
                }
                else
                    spec = it->spec;
                break;
            }
        }
    }
    return spec;
}

void MapController::ClearWarps()
{
    warps.clear();
}

MapController::~MapController()
{
    ClearMap();
}

char MapController::ConsumeCursorMoved()
{
    char moved = cursor_moved;
    cursor_moved = false;
    return moved;
}

// Works out the tile under the screen point from the view centre and notes when it changed.
void MapController::UpdateCursorTile(int screen_x, int screen_y)
{
    int shifted_x;
    int shifted_y = screen_y + 640;
    shifted_x = screen_x + 620;
    int tile_offset_y = (shifted_y + 4 - shifted_x / 2) / 32;
    int adjusted_y = (shifted_y - -shifted_x / 2) / 32;
    cursor_x = adjusted_y + center_x - 39;
    cursor_y = tile_offset_y + center_y - 10;
    if (cursor_x != last_cursor_x || cursor_y != last_cursor_y)
        cursor_moved = true;
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;
}

int MapController::GetCursorX()
{
    return cursor_x;
}

int MapController::GetCursorY()
{
    return cursor_y;
}

// One step of the map's timers: the path marker, the brief action block, the ambient sound of the nearest source,
// door closing, the warp delay and timeout, and the scrolling of the view.
void MapController::Animate()
{
    MapDoor *it;
    int dx;
    MapObject *spec_it;
    int dy;
    int door_graphic;
    MapObject *tile_spec;
    int player_x;
    int player_y;
    if (path_marker_shown)
    {
        MainForm->redraw_needed = true;
        path_marker_timer--;
        if (path_marker_timer <= 0)
            path_marker_shown = false;
    }
    if (action_block_tick)
        action_block_tick = false;
    else
        action_blocked = false;
    // The ambient sound plays by the nearest tile of spec 32 within 25 steps, and fades out beyond.
    // The ambient sound plays by the nearest ambient source tile within 25 steps, and fades out beyond.
    if (ambient_sound > 0)
    {
        player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
        player_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
        int nearest = 26;
        spec_it = tile_specs.begin();
        while (spec_it != tile_specs.end())
        {
            if ((short)spec_it->spec == TileSpec_AmbientSource)
            {
                dx = abs(player_x - spec_it->x);
                dy = abs(player_y - spec_it->y);
                if (nearest > dx + dy)
                    nearest = dx + dy;
            }
            spec_it++;
        }
        if (nearest < 25)
            MainForm->sfx->PlayAmbientSound(ambient_sound, nearest);
        else
            MainForm->sfx->FadeOutAmbientSound();
    }
    else
        MainForm->sfx->FadeOutAmbientSound();
    it = doors.begin();
    while (it != doors.end())
    {
        if (it->open)
        {
            it->close_timer--;
            if (it->close_timer <= 0)
            {
                int door_x = it->x;
                int door_y = it->y;
                door_graphic = it->closed_graphic;
                if (door_graphic > 0)
                {
                    if (walls[door_x][door_y] > 0)
                        walls[door_x][door_y] = door_graphic;
                    else if (walls_next_row[door_x][door_y] > 0)
                        walls_next_row[door_x][door_y] = door_graphic;
                }
                tile_spec = tile_specs.begin();
                while (tile_spec != tile_specs.end())
                {
                    if (door_x == tile_spec->x && tile_spec->y == door_y && (short)tile_spec->spec == TileSpec_OpenDoor)
                        tile_spec->spec = TileSpec_Door;
                    tile_spec++;
                }
                if (MainForm->net_client->IsInSoundRange(door_x, door_y, MainForm->map->GetCenterX(),
                    MainForm->map->GetCenterY()))
                    MainForm->sfx->PlayEffectSoundAt(SfxSound_DoorClose, MainForm->sfx->GetDistanceVolume(door_x,
                        door_y));
                it->closed_graphic = 0;
                it->open = false;
                MainForm->redraw_needed = true;
                break;
            }
        }
        it++;
    }
    if (warp_delay > 0)
    {
        warp_delay--;
        if (!warp_delay)
        {
            warp_delay--;
            AcceptPendingWarp();
        }
    }
    if (warp_timeout > 0 && warp_delay <= 0)
    {
        warp_timeout--;
        if (!warp_timeout)
        {
            warp_timeout--;
            if (warp_timeout_action == WarpTimeoutAction_AcceptWarp)
                AcceptPendingWarp();
            if (warp_timeout_action == WarpTimeoutAction_ConfirmWarp)
            {
                String request = MainForm->net_client->EncodeNumber(map_id, 2);
                request = request + MainForm->net_client->EncodeNumber(warp_session_id, 2);
                MainForm->net_client->SendPacket(AC_CONFIRM, OP_MAP, request);
            }
        }
    }
    if (scroll_requested)
    {
        scroll_requested = false;
        scrolling = true;
        scroll_step = 1;
        if (scroll_direction == FacingDirection_Down)
            center_y++;
        if (scroll_direction == FacingDirection_Right)
            center_x++;
        if (scroll_direction == FacingDirection_Up)
            center_y--;
        if (scroll_direction == FacingDirection_Left)
            center_x--;
    }
    if (IsScrolling())
    {
        if (GetScrollDirection() == FacingDirection_Left)
        {
            scroll_offset_x = 0 - (32 * center_x + 32 - 8 * GetScrollStep()) + 32 * center_y;
            scroll_offset_y = 0 - (16 * center_x + 16 - 4 * GetScrollStep()) - 16 * center_y;
            scroll_shift_x = 0 - (8 - 2 * GetScrollStep());
            scroll_shift_y = 0 - (4 - GetScrollStep());
        }
        if (GetScrollDirection() == FacingDirection_Up)
        {
            scroll_offset_x = 32 * center_y + 32 - 8 * GetScrollStep() + (0 - 32 * center_x);
            scroll_offset_y = 0 - 16 * center_x - (16 * center_y + 16 - 4 * GetScrollStep());
            scroll_shift_x = 8 - 2 * GetScrollStep();
            scroll_shift_y = 0 - (4 - GetScrollStep());
        }
        if (GetScrollDirection() == FacingDirection_Right)
        {
            scroll_offset_x = 0 - (8 * GetScrollStep() + 32 * center_x - 32) + 32 * center_y;
            scroll_offset_y = 0 - (4 * GetScrollStep() + 16 * center_x - 16) - 16 * center_y;
            scroll_shift_x = 8 - 2 * GetScrollStep();
            scroll_shift_y = 4 - GetScrollStep();
        }
        if (GetScrollDirection() == FacingDirection_Down)
        {
            scroll_offset_x = 8 * GetScrollStep() + 32 * center_y + (0 - 32 * center_x) - 32;
            scroll_offset_y = 0 - 16 * center_x - (4 * GetScrollStep() + 16 * center_y - 16);
            scroll_shift_x = 0 - (8 - 2 * GetScrollStep());
            scroll_shift_y = 4 - GetScrollStep();
        }
        if (scroll_step <= 3)
        {
            scroll_step++;
            cursor_moved = true;
        }
        else
        {
            scroll_step = 0;
            scrolling = false;
            AcceptPendingWarp();
        }
    }
    else
    {
        if (cursor_update_pending)
        {
            cursor_update_pending = false;
            cursor_moved = true;
            UpdateCursorTile(Mouse->CursorPos.x, Mouse->CursorPos.y);
        }
        scroll_offset_x = 0 - 32 * center_x + 32 * center_y;
        scroll_offset_y = 0 - 16 * center_x - 16 * center_y;
        scroll_shift_x = 0;
        scroll_shift_y = 0;
    }
}

int MapController::GetScrollShiftX()
{
    return scroll_shift_x;
}

int MapController::GetScrollShiftY()
{
    return scroll_shift_y;
}

// Acts on the warp the server sent: accepts it, or asks for the map first when the local file is not the same.
// The warp data starts with its type. Type 1 carries the map and the session.
// Type 2 carries the map, the checksum, the file size and the session.
void MapController::AcceptPendingWarp()
{
    if (map_data.Length() > 1)
    {
        int warp_type = MainForm->net_client->DecodeNumber(map_data.SubString(1, 1));
        if (warp_type == 1)
        {
            SetWarpTimeoutAction(WarpTimeoutAction_AcceptWarp);
            int warp_map_id = MainForm->net_client->DecodeNumber(map_data.SubString(2, 2));
            int session_id = MainForm->net_client->DecodeNumber(map_data.SubString(4, 2));
            map_id = warp_map_id;
            warp_session_id = session_id;
            map_data = "";
            warp_pending = true;
            FreezePlayer();
            String accept_data = MainForm->net_client->EncodeNumber(warp_map_id, 2);
            accept_data = accept_data + MainForm->net_client->EncodeNumber(session_id, 2);
            if (warp_map_id > 0)
                MainForm->net_client->SendPacket(AC_CONFIRM, OP_MAP, accept_data);
        }
        if (warp_type == 2)
        {
            SetWarpTimeoutAction(WarpTimeoutAction_AcceptWarp);
            int new_map_id = MainForm->net_client->DecodeNumber(map_data.SubString(2, 2));
            int server_checksum_high = MainForm->net_client->DecodeNumber(map_data.SubString(4, 2));
            int server_checksum_low = MainForm->net_client->DecodeNumber(map_data.SubString(6, 2));
            int server_file_size = MainForm->net_client->DecodeNumber(map_data.SubString(8, 3));
            int new_session_id = MainForm->net_client->DecodeNumber(map_data.SubString(11, 2));
            map_id = new_map_id;
            warp_session_id = new_session_id;
            map_file_size = server_file_size;
            map_data = "";
            ReadMapChecksum(new_map_id);
            if (checksum_high == server_checksum_high && checksum_low == server_checksum_low &&
                file_size == server_file_size)
            {
                warp_pending = true;
                FreezePlayer();
                String map_accept_data = MainForm->net_client->EncodeNumber(new_map_id, 2);
                map_accept_data = map_accept_data + MainForm->net_client->EncodeNumber(new_session_id, 2);
                if (new_map_id > 0)
                    MainForm->net_client->SendPacket(AC_CONFIRM, OP_MAP, map_accept_data);
            }
            else
            {
                StopWarpTimeout();
                MainForm->edits->caret_enabled = false;
                warp_pending = true;
                loading_map = true;
                map_request_pending = true;
                FreezePlayer();
            }
        }
    }
}

int MapController::GetScrollOffsetX()
{
    return scroll_offset_x;
}

int MapController::GetScrollOffsetY()
{
    return scroll_offset_y;
}

// Empties every tile layer and the doors, chests, tile specs, warps and signs.
void MapController::ClearMap()
{
    for (int i = 0; i <= 249; i++)
    {
        for (int j = 0; j <= 249; j++)
        {
            ground_tiles[i][j] = 0;
            walls[i][j] = 0;
            walls_next_row[i][j] = 0;
            objects[i][j] = 0;
            masks[i][j] = 0;
            top_tiles[i][j] = 0;
            overlay_tiles[i][j] = 0;
            blend_tiles[i][j] = 0;
            top_masks[i][j] = 0;
            blocked[i][j] = false;
            has_tile_spec[i][j] = false;
            minimap_mask[i][j] = 0;
            spec_grid[i][j] = 0;
        }
    }
    has_boss = false;
    ClearTileObjects();
    ClearWarps();
    ClearSigns();
}

String MapController::GetName()
{
    return name;
}

int MapController::GetMapType()
{
    return map_type;
}

int MapController::GetMapAttribute()
{
    return map_attribute;
}

int MapController::GetMusicId()
{
    return music_id;
}

int MapController::GetMusicMode()
{
    return music_mode;
}

void MapController::ClearMusic()
{
    music_id = 0;
    music_mode = MusicMode_OnceOnChange;
}

int MapController::GetMapId()
{
    return loaded_map_id;
}

void MapController::SetCenterX(int x)
{
    center_x = x;
}

void MapController::SetCenterY(int y)
{
    center_y = y;
}

int MapController::GetHeight()
{
    return height;
}

int MapController::GetWidth()
{
    return width;
}

int MapController::GetCenterX()
{
    return center_x;
}

int MapController::GetCenterY()
{
    return center_y;
}

int MapController::GetScrollStep()
{
    return scroll_step;
}

int MapController::GetScrollDirection()
{
    return scroll_direction;
}

bool MapController::IsMinimapDisabled()
{
    return minimap_disabled;
}

bool MapController::IsWarpDelayed()
{
    bool delayed = false;
    if (warp_delay > 0)
        delayed = true;
    return delayed;
}

void MapController::DelayWarp()
{
    warp_delay = 10;
}

// Asks for the view to scroll one tile in the direction on the next step.
void MapController::StartScroll(int direction)
{
    scroll_requested = true;
    scroll_direction = direction;
    cursor_update_pending = true;
}

bool MapController::IsScrolling()
{
    return scrolling || scroll_requested;
}

bool MapController::IsCursorUpdatePending()
{
    return cursor_update_pending;
}

void MapController::BlockActionsBriefly()
{
    action_blocked = true;
    action_block_tick = true;
}

void MapController::FreezePlayer()
{
    frozen = true;
}

void MapController::UnfreezePlayer()
{
    frozen = false;
}

// Whether the player is held: briefly blocked, frozen for a warp or waiting out a warp delay.
bool MapController::IsActionBlocked()
{
    return action_blocked || frozen || warp_delay > 0;
}

bool MapController::HasBoss()
{
    return has_boss;
}

bool MapController::HasPathMarker()
{
    return path_marker_shown;
}

// Shows the path marker at the tile for 7 ticks.
void MapController::SetPathMarker(int x, int y)
{
    path_marker_x = x;
    path_marker_y = y;
    path_marker_timer = 7;
    path_marker_shown = true;
}

int MapController::GetPathMarkerX()
{
    return path_marker_x;
}

int MapController::GetPathMarkerY()
{
    return path_marker_y;
}

int MapController::GetPathMarkerTimer()
{
    return path_marker_timer;
}
