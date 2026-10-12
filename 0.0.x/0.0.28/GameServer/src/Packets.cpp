#include <vcl.h>
#pragma hdrstop

#include "Packets.h"
#include "Banned.h"
#include "Classvalue.h"
#include "Classvalues.h"
#include "Filecache.h"
#include "Gamecontrol.h"
#include "Innvalues.h"
#include "Itemground.h"
#include "Itemvalue.h"
#include "Itemvalues.h"
#include "Jukeboxcontrol.h"
#include "Killcounters.h"
#include "Learnitem.h"
#include "Learnvalues.h"
#include "Logins.h"
#include "MainForm.h"
#include "Mapcontrol.h"
#include "Mapobject.h"
#include "Msgboardcontrol.h"
#include "Mysqlcontrols.h"
#include "Mysqltask.h"
#include "Newscontrol.h"
#include "Npc.h"
#include "Npcvalues.h"
#include "Player.h"
#include "Playercommand.h"
#include "Playerinventory.h"
#include "Playerquest.h"
#include "Players.h"
#include "Playerskill.h"
#include "Questcounters.h"
#include "Questengine.h"
#include "Queststate.h"
#include "Settings.h"
#include "Shopvalues.h"
#include "Skillvalues.h"
#include "Weaponmap.h"
#include "Weddings.h"

#pragma package(smart_init)

#include <stdio.h>
#include <stdlib.h>
#include "Constants.h"

#define ACTION_QUEUE_LIMIT 10
#define CLIENT_TICK_WRAP_MIN 7500000
#define MAP_TYPE_PK 3
#define MIN_ACTION_DELAY_MS 350
#define PROTECTED_MIN_LEVEL 5
#define SIT_STATE_FLOOR 2
#define WALK_DELAY_SANITY_MS 7500000

// Loads the word filter from endl_wordfilter, hands out the first pings and keeps the client version.
Packets::Packets(MapContainer *map_control, QuestContainer *quest_engine, Players *players, Settings *settings,
    mySQLdb *mysql_controls, Logins *logins, int version_major, int version_minor, int version_patch)
{
    this->encode_buffer = new char[8];
    this->packet_buffer = new char[65000];
    DateSeparator = 47;
    ShortDateFormat = "yyyy/mm/dd";
    this->start_time = Now();
    this->online_names_ttl = 0;
    this->online_list_ttl = 0;
    this->weapon_map = new WeaponMapper();
    this->banned = new Banned(mysql_controls);
    this->kill_counters = new KillCounters();
    this->quest_counters = new QuestCounters();
    this->map_control = map_control;
    this->quest_engine = quest_engine;
    this->players = players;
    this->logins = logins;
    this->mysql_controls = mysql_controls;
    this->settings = settings;
    this->mysql_controls->Query("SELECT * FROM endl_wordfilter");
    this->wordfilter = new TStringList();
    while (!this->mysql_controls->ResultAtEnd())
    {
        this->wordfilter->Add(this->mysql_controls->Db_GetString("word"));
        this->mysql_controls->NextResultRecord();
    }
    this->Connection_Ping();
    this->version_major = version_major;
    this->version_minor = version_minor;
    this->version_patch = version_patch;
    this->received_bytes = 0;
    this->received_kilobytes = 0;
    this->received_megabytes = 0;
    this->sent_bytes = 0;
    this->sent_kilobytes = 0;
    this->sent_megabytes = 0;
    this->ticks = 0;
    this->shutting_down = false;
    this->kill_counters_cleared = false;
    this->chat_log_lines = 1;
    this->chat_log_number = 1;
    this->cheat_offset_x = 0;
    this->cheat_offset_y = 0;
}

Packets::~Packets()
{
}

// The server's tick ( every 100 ms ): the pings, and once a second each player's timers.
void Packets::Game_Tick()
{
    ticks++;
    save_gate = false;
    // .. every 10 seconds the cached online lists age
    if (!(ticks % 100))
    {
        if (online_names_ttl > 0)
            online_names_ttl--;
        if (online_list_ttl > 0)
            online_list_ttl--;
    }
    // every 20 seconds new pings
    if (!(ticks % 200))
        Connection_Ping();
    if (ticks > 200)
        ticks = 0;
    // .. once a second, for each player in the game
    if (!(ticks % 10))
    {
        for (vector<Player *>::iterator player = players->players.begin(); player != players->players.end(); player++)
        {
            if ((*player)->logged_in)
            {
                (*player)->save_ticks++;
                (*player)->recover_ticks++;
                (*player)->ghost_token_ticks++;
                (*player)->attack_token_ticks++;
                (*player)->idle_ticks++;
                // standing still 12 seconds: a warp next to them or under them takes them
                if ((*player)->idle_ticks == 12)
                {
                    char found = 0;
                    if (!found)
                        found = Player_CheckIdleWarp(*player, (*player)->x, (*player)->y + 1);
                    if (!found)
                        found = Player_CheckIdleWarp(*player, (*player)->x + 1, (*player)->y);
                    if (!found)
                        found = Player_CheckIdleWarp(*player, (*player)->x, (*player)->y - 1);
                    if (!found)
                        found = Player_CheckIdleWarp(*player, (*player)->x - 1, (*player)->y);
                    if (!found)
                        found = Player_CheckIdleWarp(*player, (*player)->x, (*player)->y);
                }
                // the ghost and attack budgets fill up again
                if ((*player)->ghost_token_ticks > 12)
                {
                    (*player)->ghost_walk_tokens = 2;
                    (*player)->ghost_token_ticks = 0;
                    (*player)->attack_tokens = 35;
                }
                if ((*player)->attack_token_ticks > 3)
                {
                    (*player)->attack_token_ticks = 0;
                    (*player)->attack_tokens = 9;
                }
                // .. every minute they heal and get their hp and tp
                if ((*player)->recover_ticks > 60)
                {
                    cheat_offset_x = random(3);
                    cheat_offset_y = random(3);
                    players->Player_RegenHpTp(*player);
                    String hp_str = EO_EncodeNumber((*player)->hp, 2);
                    hp_str.Insert(EO_EncodeNumber((*player)->tp, 2), hp_str.Length() + 1);
                    hp_str.Insert(EO_EncodeNumber(0, 2), hp_str.Length() + 1);
                    Client_SendEncoded(*player, AC_SET, OP_GAMEBAR, hp_str);
                    if ((*player)->in_party)
                    {
                        String pid_str = EO_EncodeNumber((*player)->player_id, 2);
                        pid_str.Insert(EO_EncodeNumber((*player)->HpPercent(), 1), pid_str.Length() + 1);
                        Server_BroadcastToParty(*player, AC_UPDATE, OP_GROUP, pid_str);
                    }
                    (*player)->recover_ticks = 0;
                }
                if (save_gate && players->GetOnlineCount() + 600 < (*player)->save_ticks)
                {
                    if (!(*player)->removing)
                        mysql_controls->Mysql_SubmitExec((*player)->account_ident,
                            players->Character_BuildSaveQuery(*player, 1));
                    (*player)->save_ticks = 0;
                    save_gate = false;
                }
            }
            if ((*player)->remove_timer > 0)
            {
                (*player)->remove_timer--;
                if ((*player)->remove_timer < 1)
                {
                    (*player)->removing = true;
                    players->MarkDirty();
                }
            }
            if ((*player)->account_create_cooldown > 0)
                (*player)->account_create_cooldown--;
        }
        // .. shutting down: once everyone is gone and the queries are done, save and quit
        if (shutting_down && players->players.size() < 1 && mysql_controls->Database_TryShutdown())
        {
            mysql_controls->file_cache->FlushCache();
            kill_counters->Save();
            quest_counters->Save();
            Application->Terminate();
        }
    }
    // .. with maxkills on, the day's kill and quest counts are cleared just before midnight
    if (settings->GetMaxKills() > 0)
    {
        TTimeStamp ts = DateTimeToTimeStamp(Now());
        int stamp = ts.Time / 10 + 100;
        if (!kill_counters_cleared)
        {
            if (stamp > 8620000)
            {
                kill_counters->Clear();
                quest_counters->Clear();
                kill_counters_cleared = true;
            }
        }
        else if (stamp < 100000)
            kill_counters_cleared = false;
    }
}

// Tells everyone the server closes, and removes them all.
void Packets::Server_Shutdown()
{
    Server_BroadcastToAll(AC_CLOSE, OP_SYSTEM, "r");
    players->MarkDirty();
    for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
        (*it)->removing = true;
    shutting_down = true;
}

// A player's connection closed: their party, their arena and the players near them hear of it; their spot is made safe.
void Packets::Server_RemovePlayer(Scktcomp::TCustomWinSocket *socket)
{
    if (players->by_id[socket->SocketHandle])
    {
        Player *player = players->by_id[socket->SocketHandle];
        if (!player->logged_in)
            return;
        if (player->in_party)
        {
            Server_BroadcastToPartyExceptSelf(player, AC_DELETE, OP_GROUP,
                EO_EncodeNumber(socket->SocketHandle, 2));
            players->Player_LeaveParty(player);
        }
        if (player->in_arena && map_control->maps[player->map_id - 1].arena_enabled &&
            players->CountArenaPlayers(player->map_id) == 2)
        {
            String arena_msg = "The event was aborted, last opponent disconnected -server";
            Server_BroadcastToMap(player->map_id, AC_SYSTEM, OP_CHAT, arena_msg);
        }
        Server_BroadcastNearby(player, AC_DELETE, OP_PLAYERS,
            EO_EncodeNumber(socket->SocketHandle, 2));
        // .. a map with a relog spot puts them there
        if (player->map_id > 0)
        {
            MapCoord coords = map_control->GetRelogCoords(player->map_id);
            if (coords.x > 0 && coords.y > 0)
            {
                player->x = coords.x;
                player->y = coords.y;
                player->on_chair = 0;
                player->sitting = 0;
            }
            map_control->DecPlayerCount(player->map_id);
        }
        if (player->map_switch_pending)
        {
            player->map_id = player->restore_map;
            player->x = player->restore_x;
            player->y = player->restore_y;
        }
        // .. an impossible spot: back to their inn, or to the rescue spot
        if (player->x > 250 || player->y > 250 || player->map_id < 1 || map_control->maps.size() < (unsigned int)player->map_id)
        {
            player->map_id = GUI->inn_values->GetSpawnMap(player->home_id, player->level);
            if (player->map_id < 0)
            {
                player->map_id = settings->GetRescueMap();
                player->x = settings->GetRescueX();
                player->y = settings->GetRescueY();
            }
            else
            {
                player->x = GUI->inn_values->GetSpawnX(player->home_id, player->level);
                player->y = GUI->inn_values->GetSpawnY(player->home_id, player->level);
            }
        }
    }
    return;
}

// Takes what came in on a socket and handles each whole packet in it: the first one is the init.
void Packets::Server_ClientRead(Scktcomp::TCustomWinSocket *socket, String data)
{
    if (!players->by_id[socket->SocketHandle])
    {
        socket->Close();
        return;
    }
    Player *player = players->by_id[socket->SocketHandle];
    if (player->removing)
        return;
    player->receive_buffer.Insert(data, player->receive_buffer.Length() + 1);
    // more than 500 bytes waiting closes the socket
    if (player->receive_buffer.Length() > 500)
    {
        socket->Close();
        return;
    }
    while (player->receive_buffer.Length() >= 3)
    {
        int packet_len = 2;
        packet_len += Server_DecodePacketLength(player->receive_buffer.SubString(1, 2));
        // a bad length counts as a failed login for the ip
        if (packet_len < 3 || packet_len > 300)
        {
            logins->AddLogin(player->remote_ip);
            socket->Close();
            break;
        }
        if (player->receive_buffer.Length() < packet_len)
            break;
        if (!player->initialized)
        {
            if (shutting_down)
            {
                socket->Close();
                break;
            }
            else
            {
                if (!Server_HandleInit(player, player->receive_buffer.SubString(3, packet_len - 2)))
                {
                    socket->Close();
                    break;
                }
            }
        }
        else
        {
            if (!Player_HandlePacket(player, player->receive_buffer.SubString(3, packet_len - 2)))
            {
                socket->Close();
                break;
            }
        }
        player->receive_buffer.Delete(1, packet_len);
    }
}

// Handles one packet from a player: decrypts it, checks its sequence, then runs it by its family and action.
bool Packets::Player_HandlePacket(Player *player, String data)
{
    int i;
    this->Server_AddReceivedBytes(data.Length());
    if (data.Length() < 4)
        return false;
    player->packet_count++;
    player->sequence++;
    if (player->sequence > 9)
        player->sequence = 0;
    // .. decrypt: flip the top bits, then undo the interleave
    for (i = 1; i <= data.Length(); i++)
    {
        int byte_value = (unsigned char)data[i];
        if (byte_value < 128)
            data[i] += (char)128;
        if (byte_value > 128)
            data[i] += (char)128;
    }
    std::string range(data.c_str());
    data = EO_Decode_Deinterleave(player->client_encryption_multiple, range.begin(), range.end());
    int action = this->EO_DecodeByte(data[1]);
    int family = this->EO_DecodeByte(data[2]);
    int packet_seq = this->EO_DecodeNumber(String(data[3]));
    packet_seq -= player->sequence;
    data.Delete(1, 3);
    char found = 0;
    // the sequence must be one of the last three the pings handed out
    for (int ping_index = 0; ping_index < PING_HISTORY_SIZE; ping_index++)
    {
        if (this->ping_history[ping_index] == packet_seq)
            found = 1;
    }
    if (!found)
        return true;
    GUI->last_family = family;
    GUI->last_action = action;
    // .. Walk: a step that comes sooner than MIN_ACTION_DELAY_MS after the last waits in the action queue
    if (family == OP_MOVE)
    {
        if (!player->logged_in)
            return false;
        TTimeStamp walk_stamp = DateTimeToTimeStamp(Now());
        int walk_delay = walk_stamp.Time - player->last_action_tick.Time;
        // a gap past the sanity limit counts as just in time
        if (walk_delay > WALK_DELAY_SANITY_MS)
            walk_delay = MIN_ACTION_DELAY_MS;
        if (walk_delay < MIN_ACTION_DELAY_MS)
        {
            if (player->action_queue.size() > ACTION_QUEUE_LIMIT)
                return true;
            PlayerCommand walk_early_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), walk_early_command);
            return true;
        }
        else
        {
            if (player->action_queue.size() > 0)
            {
                PlayerCommand walk_queued_command(family, action, data);
                player->action_queue.insert(player->action_queue.end(), walk_queued_command);
                return true;
            }
            else
                return this->Walk_Execute(player, action, &data);
        }
    }
    // .. Talk: chat, the player commands and the admin commands
    if (family == OP_CHAT)
    {
        if (!player->logged_in)
            return false;
        if (action == AC_DO || action == AC_SET || action == AC_PUBLIC)
        {
            this->mysql_controls->NormalizePlayerText(&data);
            String chat_packet = EO_EncodeNumber(player->player_id, 2);
            chat_packet.Insert(data, chat_packet.Length() + 1);
            // .. player commands start with '#'
            if (data[1] == '#')
            {
                if (data == "#kills" || data == "#kill")
                {
                    int kills = this->kill_counters->Get(player->name);
                    int max_kills = this->settings->GetMaxKills();
                    if (kills > max_kills)
                        kills = max_kills;
                    int kills_left = max_kills - kills;
                    String player_command_message = "kill status : " + IntToStr(kills) + "/" + IntToStr(max_kills) +
                        " -> " + IntToStr(kills_left) + " exp-kills left for today.";
                    this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, player_command_message);
                    return true;
                }
            }
            // .. admin commands: '$', a letter, a space and the player's name
            if (player->admin_level > AdminLevel_Player && data.Length() > 1 && data[1] == '$')
            {
                // .. $j jails a player, $f frees one from the jail map ( guardian and up )
                if (data[2] == 'j' || data[2] == 'f')
                {
                    if (player->admin_level < AdminLevel_Guardian)
                        return true;
                    this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                    Player *jail_target = this->players->FindByName(PacketReader_GetBreakString());
                    if (!jail_target)
                        return true;
                    if (jail_target->player_id == player->player_id)
                        return true;
                    // players under level 5 are protected from admin actions
                    if (jail_target->level < PROTECTED_MIN_LEVEL)
                    {
                        String message = "Action denied: " + jail_target->name +
                            " is not level 5, and is protected against admin actions.";
                        this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, message);
                        return true;
                    }
                    if (jail_target->admin_level > AdminLevel_LightGuide)
                        return true;
                    if (data[2] == 'j')
                    {
                        MapCoord jail_coords;
                        jail_coords.x = this->settings->GetJailX();
                        jail_coords.y = this->settings->GetJailY();
                        jail_target->flush_queue = true;
                        this->Player_Warp(jail_target, this->settings->GetJailMap(), jail_coords, WarpEffect_None, 0);
                        String jail_message = "Attention!! " + jail_target->name + " has been jailed -" + player->name;
                        this->Server_BroadcastToAll(AC_SYSTEM, OP_CHAT, jail_message);
                    }
                    if (data[2] == 'f')
                    {
                        if (this->settings->GetJailMap() == jail_target->map_id)
                        {
                            MapCoord unjail_coords;
                            unjail_coords.x = 9;
                            unjail_coords.y = 11;
                            this->Player_Warp(jail_target, 76, unjail_coords, WarpEffect_None, 0);
                        }
                    }
                }
                // .. $t brings a player to the tile in front of you ( game master and up )
                if (data[2] == 't')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_GameMaster)
                            return true;
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                        Player *wall_check_target = this->players->FindByName(PacketReader_GetBreakString());
                        if (!wall_check_target)
                            return true;
                        if (wall_check_target->player_id == player->player_id)
                            return true;
                        if (wall_check_target->level < PROTECTED_MIN_LEVEL)
                        {
                            String message = "Action denied: " + wall_check_target->name +
                                " is not level 5, and is protected against admin actions.";
                            this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, message);
                            return true;
                        }
                        MapCoord wall_check_coords;
                        wall_check_coords.x = player->x;
                        wall_check_coords.y = player->y;
                        if (player->direction == FacingDirection_Left)
                            wall_check_coords.x = wall_check_coords.x - 1;
                        if (player->direction == FacingDirection_Up)
                            wall_check_coords.y--;
                        if (player->direction == FacingDirection_Right)
                            wall_check_coords.x = wall_check_coords.x + 1;
                        if (player->direction == FacingDirection_Down)
                            wall_check_coords.y++;
                        if (!this->map_control->IsTileClear(player->map_id, wall_check_coords.x, wall_check_coords.y))
                        {
                            wall_check_coords.x = player->x;
                            wall_check_coords.y = player->y;
                        }
                        if (!this->map_control->CountOpenNeighbors(player->map_id, wall_check_coords.x,
                            wall_check_coords.y))
                        {
                            this->banned->AddBan(player->remote_ip, player->hdid, (char)0, 7200);
                            String wall_ban_message = "Attention!! " + player->name +
                                " has been banned for a wall-attempt on a player -" + player->name + " [2hr. ban]";
                            this->Server_BroadcastToMapAndAdmins(player->map_id, AC_SYSTEM, OP_CHAT,
                                wall_ban_message);
                            return false;
                        }
                        wall_check_target->flush_queue = true;
                        this->Player_Warp(wall_check_target, player->map_id, wall_check_coords, WarpEffect_Admin, 0);
                    }
                    catch (...)
                    {
                    }
                }
                // .. $w warps you to a player: in front of them, or for a light guide somewhere else on their map
                if (data[2] == 'w')
                {
                    try
                    {
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                        Player *goto_target = this->players->FindByName(PacketReader_GetBreakString());
                        if (!goto_target)
                            return true;
                        if (goto_target->player_id == player->player_id)
                            return true;
                        player->flush_queue = true;
                        MapCoord goto_coords;
                        goto_coords.x = goto_target->x;
                        goto_coords.y = goto_target->y;
                        if (player->admin_level > AdminLevel_LightGuide)
                        {
                            if (goto_target->direction == FacingDirection_Left)
                                goto_coords.x = goto_coords.x - 1;
                            if (goto_target->direction == FacingDirection_Up)
                                goto_coords.y--;
                            if (goto_target->direction == FacingDirection_Right)
                                goto_coords.x = goto_coords.x + 1;
                            if (goto_target->direction == FacingDirection_Down)
                                goto_coords.y++;
                            if (!this->map_control->IsTileClear(goto_target->map_id, goto_coords.x, goto_coords.y))
                            {
                                goto_coords.x = goto_target->x;
                                goto_coords.y = goto_target->y;
                            }
                            this->Player_Warp(player, goto_target->map_id, goto_coords, WarpEffect_Admin, 0);
                        }
                        else
                        {
                            goto_coords.x = this->map_control->maps[goto_target->map_id - 1].width / 2;
                            goto_coords.y = this->map_control->maps[goto_target->map_id - 1].height / 2;
                            if (goto_coords.x < 7 && goto_coords.y < 7)
                                return true;
                            if (goto_target->x < goto_coords.x)
                                goto_coords.x = goto_coords.x / 2 + goto_coords.x;
                            if (goto_target->x >= goto_coords.x)
                                goto_coords.x = goto_coords.x / 2;
                            if (goto_target->y < goto_coords.y)
                                goto_coords.y += goto_coords.y / 2;
                            if (goto_target->y >= goto_coords.y)
                                goto_coords.y = goto_coords.y / 2;
                            if (!this->map_control->IsTileClear(goto_target->map_id, goto_coords.x, goto_coords.y))
                            {
                                goto_coords.x = 2;
                                goto_coords.y = 2;
                                if (!this->map_control->IsTileClear(goto_target->map_id, goto_coords.x, goto_coords.y))
                                    return true;
                            }
                            this->Player_Warp(player, goto_target->map_id, goto_coords, WarpEffect_Scroll, 0);
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $m mutes a player ( guardian and up )
                if (data[2] == 'm')
                {
                    if (player->admin_level < AdminLevel_Guardian)
                        return true;
                    try
                    {
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                        Player *mute_target = this->players->FindByName(PacketReader_GetBreakString());
                        if (!mute_target)
                            return true;
                        if (mute_target->player_id == player->player_id)
                            return true;
                        if (mute_target->admin_level > AdminLevel_LightGuide)
                            return true;
                        this->Client_SendEncoded(mute_target, AC_HIT, OP_CHAT, player->name);
                    }
                    catch (...)
                    {
                    }
                }
                // .. $q shakes the map with a quake of strength 1 to 7 ( game master and up )
                if (data[2] == 'q')
                {
                    if (player->admin_level < AdminLevel_GameMaster)
                        return true;
                    try
                    {
                        String quake_packet = EO_EncodeNumber(1, 1);
                        quake_packet.Insert(EO_EncodeNumber(random(7) + 1, 1), quake_packet.Length() + 1);
                        this->Server_BroadcastToMap(player->map_id, AC_DO, OP_EFFECT,
                            quake_packet);
                    }
                    catch (...)
                    {
                    }
                }
                // .. $g turns the world ( global ) chat on or off ( game master and up )
                if (data[2] == 'g')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_GameMaster)
                            return true;
                        String comm = "off";
                        if (this->settings->GetWorldCommunication())
                            this->settings->SetWorldCommunication(0);
                        else
                        {
                            this->settings->SetWorldCommunication(1);
                            comm = "on";
                        }
                        String global_toggle_message = "World communication changed to: " + comm + " -" + player->name;
                        this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, global_toggle_message);
                    }
                    catch (...)
                    {
                    }
                }
                // .. $d speaks as another player, only for vult-r and arglon
                if (data[2] == 'd')
                {
                    try
                    {
                        if (LowerCase(player->name) == "vult-r" || LowerCase(player->name) == "arglon")
                        {
                            this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                            Player *dev_tell_target = this->players->FindByName(PacketReader_GetBreakString());
                            String dev_tell_message = PacketReader_GetBreakString();
                            if (!dev_tell_target)
                                return true;
                            if (dev_tell_target->player_id == player->player_id)
                                return true;
                            String dev_tell_packet = EO_EncodeNumber(dev_tell_target->player_id, 2);
                            dev_tell_packet.Insert(dev_tell_message, dev_tell_packet.Length() + 1);
                            this->Server_BroadcastNearby(dev_tell_target, AC_SET, OP_CHAT,
                                dev_tell_packet);
                            this->Client_SendEncoded(dev_tell_target, AC_SET, OP_CHAT,
                                dev_tell_packet);
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $c stats or $c config prints the server's numbers on the map ( guardian and up )
                if (data[2] == 'c')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_Guardian)
                            return true;
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 32);
                        String server_info_command = PacketReader_GetBreakString();
                        if (server_info_command == "stats")
                        {
                            String stats_packet = "Server stats requested by " + player->name + " [";
                            stats_packet.Insert(IntToStr(GUI->server->Socket->ActiveConnections) + " conn] [",
                                stats_packet.Length() + 1);
                            stats_packet.Insert(IntToStr(this->players->GetOnlineCount()) + " players] [",
                                stats_packet.Length() + 1);
                            stats_packet.Insert(Server_FormatSentTraffic() + " b/w out] [", stats_packet.Length() + 1);
                            stats_packet.Insert(Server_FormatReceivedTraffic() + " b/w in]", stats_packet.Length() + 1);
                            this->Server_BroadcastToMap(player->map_id, AC_SYSTEM, OP_CHAT,
                                stats_packet);
                        }
                        if (server_info_command == "config")
                        {
                            String config_packet = "Server config requested by " + player->name + " [";
                            config_packet.Insert(IntToStr(this->settings->GetMaxConnections()) + " max. connections] [",
                                config_packet.Length() + 1);
                            config_packet.Insert(IntToStr(this->settings->GetMaxPlayers()) + " max. players]",
                                config_packet.Length() + 1);
                            this->Server_BroadcastToMap(player->map_id, AC_SYSTEM, OP_CHAT,
                                config_packet);
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $x hides you, or shows you again ( guardian and up )
                if (data[2] == 'x')
                {
                    if (player->admin_level < AdminLevel_Guardian)
                        return true;
                    if (player->hide_online)
                    {
                        player->hide_online = false;
                        player->hidden = false;
                        this->Server_BroadcastNearby(player, AC_UPDATE, OP_ADMIN,
                            EO_EncodeNumber(player->player_id, 2));
                        this->Client_SendEncoded(player, AC_UPDATE, OP_ADMIN,
                            EO_EncodeNumber(player->player_id, 2));
                    }
                    else
                    {
                        player->hide_online = true;
                        player->hidden = true;
                        this->Server_BroadcastNearby(player, AC_DELETE, OP_ADMIN,
                            EO_EncodeNumber(player->player_id, 2));
                        this->Client_SendEncoded(player, AC_DELETE, OP_ADMIN,
                            EO_EncodeNumber(player->player_id, 2));
                    }
                }
                // .. $k kicks, $s kicks with a 20 minute ban, $b bans for 2 hours ( game master ), up to 11 names
                if (data[2] == 'k' || data[2] == 's' || data[2] == 'b')
                {
                    if (player->admin_level < AdminLevel_Guardian)
                        return true;
                    try
                    {
                        this->PacketReader_Init(data.SubString(4, data.Length() - 3), 46);
                        for (int i = 0; i <= 10; i++)
                        {
                            Player *removal_target;
                            String removal_name = PacketReader_GetBreakString();
                            if (removal_name.Length() < 1)
                                break;
                            removal_target = this->players->FindByName(removal_name);
                            if (!removal_target)
                                return true;
                            if (removal_target->admin_level > AdminLevel_LightGuide)
                                return true;
                            removal_target->removing = true;
                            this->players->MarkDirty();
                            if (data[2] == 'k')
                            {
                                String kick_message = "Attention!! " + removal_target->name +
                                    " has been removed from game -" + player->name + " [kick]";
                                this->Server_BroadcastToMapAndAdmins(removal_target->map_id, AC_SYSTEM,
                                    OP_CHAT, kick_message);
                            }
                            if (data[2] == 's')
                            {
                                // the ban times are in seconds
                                this->banned->AddBan(removal_target->remote_ip, removal_target->hdid, (char)0, 1200);
                                String temp_ban_message = "Attention!! " + removal_target->name +
                                    " has been removed from game -" + player->name + " [20min. ban]";
                                this->Server_BroadcastToMapAndAdmins(removal_target->map_id, AC_SYSTEM,
                                    OP_CHAT, temp_ban_message);
                            }
                            if (data[2] == 'b')
                            {
                                if (player->admin_level < AdminLevel_GameMaster)
                                    return true;
                                this->banned->AddBan(removal_target->remote_ip, removal_target->hdid, (char)0, 7200);
                                String ban_message = "Attention!! " + removal_target->name +
                                    " has been banned from game -" + player->name + " [2hr. ban]";
                                this->Server_BroadcastToMapAndAdmins(removal_target->map_id, AC_SYSTEM,
                                    OP_CHAT, ban_message);
                            }
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $< bans the account for good ( high game master )
                if (data[2] == '<')
                {
                    if (player->admin_level < AdminLevel_HighGameMaster)
                        return true;
                    try
                    {
                        this->PacketReader_Init(data.SubString(4, data.Length() - 3), 46);
                        for (int i = 0; i <= 10; i++)
                        {
                            Player *account_ban_target;
                            String account_ban_name = PacketReader_GetBreakString();
                            if (account_ban_name.Length() < 1)
                                break;
                            account_ban_target = this->players->FindByName(account_ban_name);
                            if (!account_ban_target)
                                return true;
                            account_ban_target->removing = true;
                            this->players->MarkDirty();
                            String account_ban_message = "Attention!! " + account_ban_target->name +
                                " has been banned -" + player->name + " [ perm-account ban]";
                            this->Server_BroadcastToAll(AC_SYSTEM, OP_CHAT, account_ban_message);
                            this->banned->AddBan(account_ban_target->remote_ip, account_ban_target->hdid, (char)0, 200);
                            this->mysql_controls->Mysql_SubmitExec(account_ban_target->account_ident,
                                "UPDATE endl_accounts SET banned = 1 WHERE ident = " +
                                IntToStr((DWORD)account_ban_target->account_ident));
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $> bans the account, the ip and the hdid for good ( high game master )
                if (data[2] == '>')
                {
                    if (player->admin_level < AdminLevel_HighGameMaster)
                        return true;
                    try
                    {
                        this->PacketReader_Init(data.SubString(4, data.Length() - 3), 46);
                        for (int i = 0; i <= 10; i++)
                        {
                            Player *ip_ban_target;
                            String ip_ban_name = PacketReader_GetBreakString();
                            if (ip_ban_name.Length() < 1)
                                break;
                            ip_ban_target = this->players->FindByName(ip_ban_name);
                            if (!ip_ban_target)
                                return true;
                            ip_ban_target->removing = true;
                            this->players->MarkDirty();
                            String ip_ban_message = "Attention!! " + ip_ban_target->name + " has been banned -" +
                                player->name + " [ perm-IP ban]";
                            this->Server_BroadcastToAll(AC_SYSTEM, OP_CHAT, ip_ban_message);
                            this->banned->AddBan(ip_ban_target->remote_ip, ip_ban_target->hdid, true, 750);
                            this->banned->AddBan(ip_ban_target->remote_ip, ip_ban_target->hdid, (char)0, 200);
                            this->mysql_controls->Mysql_SubmitExec(ip_ban_target->account_ident,
                                "UPDATE endl_accounts SET banned = 1 WHERE ident = " +
                                IntToStr((DWORD)ip_ban_target->account_ident));
                            this->mysql_controls->Mysql_SubmitExec(ip_ban_target->account_ident,
                                "INSERT INTO endl_banlist (bandate,permanent,executor,ipaddress,serial_h,reason) "
                                "VALUES (NOW(),1,'" +
                                player->name + "','" + ip_ban_target->remote_ip + "','" + ip_ban_target->hdid +
                                "','perm-ban by HGM'");
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $p shows a player's stats, $i the inventory and bank, $h flags a bot, $a asks the flag
                if (data[2] == 'i' || data[2] == 'p' || data[2] == 'a' || data[2] == 'h')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_LightGuide)
                            return true;
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 32);
                        Player *lookup_target = this->players->FindByName(PacketReader_GetBreakString());
                        if (!lookup_target)
                            return true;
                        if (lookup_target->admin_level > AdminLevel_LightGuide)
                        {
                            if (lookup_target != player)
                                return true;
                        }
                        String lookup_reply = lookup_target->name;
                        lookup_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), lookup_reply.Length() + 1);
                        lookup_reply.Insert(EO_EncodeNumber(lookup_target->usage, 4), lookup_reply.Length() + 1);
                        lookup_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), lookup_reply.Length() + 1);
                        lookup_reply.Insert(EO_EncodeNumber(lookup_target->money_bank, 4), lookup_reply.Length() + 1);
                        lookup_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), lookup_reply.Length() + 1);
                        if (data[2] == 'p')
                        {
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->experience, 4),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->level, 1), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->map_id, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->x, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->y, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->hp, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->max_hp, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->tp, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->max_tp, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_strength, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_intelligence, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_wisdom, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_agility, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_constitution, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->adj_charisma, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->max_damage, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->min_damage, 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->accuracy, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->evasion, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->armor, 2), lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[1], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[2], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[3], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[4], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[5], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->element_resistances[6], 2),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->weight_current, 1),
                                lookup_reply.Length() + 1);
                            lookup_reply.Insert(EO_EncodeNumber(lookup_target->weight_max, 1),
                                lookup_reply.Length() + 1);
                            this->Client_SendEncoded(player, AC_PRIVATE, OP_ADMIN,
                                lookup_reply);
                        }
                        if (data[2] == 'i')
                        {
                            vector<PlayerInventory>::iterator lookup_inventory_item;
                            for (lookup_inventory_item = lookup_target->inventory.begin();
                                lookup_inventory_item != lookup_target->inventory.end(); lookup_inventory_item++)
                            {
                                lookup_reply.Insert(EO_EncodeNumber(lookup_inventory_item->item_id, 2),
                                    lookup_reply.Length() + 1);
                                lookup_reply.Insert(EO_EncodeNumber(lookup_inventory_item->amount, 4),
                                    lookup_reply.Length() + 1);
                            }
                            lookup_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), lookup_reply.Length() + 1);
                            vector<PlayerInventory>::iterator lookup_bank_item;
                            for (lookup_bank_item = lookup_target->bank.begin();
                                lookup_bank_item != lookup_target->bank.end(); lookup_bank_item++)
                            {
                                lookup_reply.Insert(EO_EncodeNumber(lookup_bank_item->item_id, 2),
                                    lookup_reply.Length() + 1);
                                lookup_reply.Insert(EO_EncodeNumber(lookup_bank_item->amount, 3),
                                    lookup_reply.Length() + 1);
                            }
                            this->Client_SendEncoded(player, AC_LIST, OP_ADMIN,
                                lookup_reply);
                            if (player->admin_level >= AdminLevel_HighGameMaster)
                            {
                                String inventory_lookup_message = lookup_target->name + " connection: " +
                                    lookup_target->remote_ip + ", hds: " + lookup_target->hdid + ", acc: " +
                                    lookup_target->account_name + ", kills: " +
                                    IntToStr(this->kill_counters->Get(lookup_target->name));
                                this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT,
                                    inventory_lookup_message);
                            }
                        }
                        if (data[2] == 'h')
                            lookup_target->cheater_flag = true;
                        if (data[2] == 'a')
                        {
                            if (lookup_target->cheater_flag)
                            {
                                String flagged_message = lookup_target->name + " is detected as a BOT.";
                                this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT,
                                    flagged_message);
                            }
                            else
                            {
                                String clean_message = lookup_target->name + " is not detected as a bot.";
                                this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, clean_message);
                            }
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $l freezes a player's movement, $u lets them walk again ( guardian and up )
                if (data[2] == 'l' || data[2] == 'u')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_Guardian)
                            return true;
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 32);
                        Player *freeze_target = this->players->FindByName(PacketReader_GetBreakString());
                        if (!freeze_target)
                            return true;
                        if (freeze_target->admin_level > AdminLevel_LightGuide)
                            return true;
                        if (data[2] == 'l')
                        {
                            this->Client_SendEncoded(freeze_target, AC_CLOSE, OP_MOVE, "S");
                            String freeze_message = "Attention!! " + freeze_target->name +
                                " movement has been frozen -" + player->name;
                            this->Server_BroadcastToMapAndAdmins(freeze_target->map_id, AC_SYSTEM,
                                OP_CHAT, freeze_message);
                        }
                        if (data[2] == 'u')
                        {
                            this->Client_SendEncoded(freeze_target, AC_OPEN, OP_MOVE, "S");
                            String unfreeze_message = "Attention!! " + freeze_target->name +
                                " movement has been released -" + player->name;
                            this->Server_BroadcastToMapAndAdmins(freeze_target->map_id, AC_SYSTEM,
                                OP_CHAT, unfreeze_message);
                        }
                    }
                    catch (...)
                    {
                    }
                }
                // .. $r map reads your map again, $r guilds the top guilds ( high game master )
                if (data[2] == 'r' && player->admin_level >= AdminLevel_HighGameMaster)
                {
                    this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 32);
                    String reload_command = PacketReader_GetBreakString();
                    if (reload_command == "map")
                    {
                        if (this->map_control->ReloadMap(player->map_id))
                        {
                            this->Server_SyncMapHazardFlags(player->map_id);
                            this->Server_SendRawToMap(player->map_id, this->map_control->ReadRawFile(player->map_id),
                                10);
                        }
                    }
                    if (reload_command == "guilds")
                        this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildTopList, player->player_id,
                            player->query_id, data,
                            "SELECT ident_guild, guild, SUM(level) as exptotal, MAX(level) as exphigh, "
                            "COUNT(ident_guild) as members FROM endl_characters WHERE LENGTH(ident_guild) > 1 AND "
                            "privilege = 0 GROUP BY ident_guild desc ORDER BY exptotal desc LIMIT 100");
                }
                // .. $v plays a map's music to you and the players near you ( game master and up )
                if (data[2] == 'v')
                {
                    try
                    {
                        if (player->admin_level < AdminLevel_GameMaster)
                            return true;
                        this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                        int music_map_id = StrToInt(PacketReader_GetBreakString());
                        this->Server_BroadcastNearby(player, AC_SET, OP_MUSIC,
                            EO_EncodeNumber(music_map_id, 1));
                        this->Client_SendEncoded(player, AC_SET, OP_MUSIC,
                            EO_EncodeNumber(music_map_id, 1));
                    }
                    catch (...)
                    {
                    }
                }
                // .. $y warps you to the middle of a map by its number ( guardian and up )
                if (data[2] == 'y')
                {
                    if (data.Length() > 3)
                    {
                        try
                        {
                            if (player->admin_level < AdminLevel_Guardian)
                                return true;
                            this->PacketReader_Init(data.SubString(4, chat_packet.Length() - 4), 46);
                            unsigned int teleport_map_id = StrToInt(PacketReader_GetBreakString());
                            if ((int)teleport_map_id > 0)
                            {
                                if (this->map_control->maps.size() > teleport_map_id)
                                {
                                    MapCoord teleport_coords;
                                    teleport_coords.x = this->map_control->maps[teleport_map_id - 1].width / 2;
                                    teleport_coords.y = this->map_control->maps[teleport_map_id - 1].height / 2;
                                    player->flush_queue = true;
                                    this->Player_Warp(player, teleport_map_id, teleport_coords, WarpEffect_None, 0);
                                }
                            }
                        }
                        catch (...)
                        {
                        }
                    }
                }
                return true;
            }
            // .. plain chat goes to the players near you, while the chat budget lasts
            if (player->talk_tokens < 1 && player->talk_tokens < 1)
                return true;
            this->Server_BroadcastNearby(player, AC_SET, OP_CHAT, chat_packet);
            player->talk_tokens--;
            return true;
        }
        // .. Talk Open: party chat
        if (action == AC_OPEN)
        {
            if (data.Length() < 1)
                return false;
            if (data.Length() < 1)
                return false;
            this->mysql_controls->NormalizePlayerText(&data);
            if (!player->in_party)
                return true;
            String party_chat_packet = EO_EncodeNumber(player->player_id, 2);
            party_chat_packet.Insert(data, party_chat_packet.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[GRP] [" + player->name + "," + player->remote_ip + "] " + data);
            this->Server_BroadcastToPartyExceptSelf(player, AC_OPEN, OP_CHAT, party_chat_packet);
            return true;
        }
        // .. Talk Request: guild chat
        if (action == AC_REQUEST)
        {
            if (data.Length() < 1)
                return false;
            this->mysql_controls->NormalizePlayerText(&data);
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return false;
            String guild_chat_packet = player->name;
            guild_chat_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), guild_chat_packet.Length() + 1);
            guild_chat_packet.Insert(data, guild_chat_packet.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[GUI] [" + player->name + "," + player->remote_ip + "] " + data);
            this->Guild_BroadcastToAll(player, AC_REQUEST, OP_CHAT, guild_chat_packet);
            return true;
        }
        // .. Talk Tell: a whisper; the reply says when the name is not online
        if (action == AC_PRIVATE)
        {
            if (data.Length() < 0)
                return false;
            this->mysql_controls->NormalizePlayerText(&data);
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            String private_name = PacketReader_GetBreakString();
            String private_message = PacketReader_GetBreakString();
            Player *private_target = this->players->FindByName(private_name);
            if (!private_target)
            {
                String not_found_reply = EO_EncodeNumber(1, 2);
                not_found_reply.Insert(private_name, not_found_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_CHAT, not_found_reply);
                return true;
            }
            if (!private_target->logged_in)
            {
                String offline_reply = EO_EncodeNumber(1, 2);
                offline_reply.Insert(private_name, offline_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_CHAT, offline_reply);
                return true;
            }
            if (private_target->hide_online)
            {
                String hidden_reply = EO_EncodeNumber(1, 2);
                hidden_reply.Insert(private_name, hidden_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_CHAT, hidden_reply);
                return true;
            }
            // a player who blocks whispers
            if (private_target->whispers_disabled)
            {
                String shown_reply = private_name;
                shown_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), shown_reply.Length() + 1);
                shown_reply.Insert("Sorry, " + private_name + " cannot hear any whispers at the moment.",
                    shown_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_PRIVATE, OP_CHAT, shown_reply);
                return true;
            }
            String whisper_reply = player->name;
            whisper_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), whisper_reply.Length() + 1);
            whisper_reply.Insert(private_message, whisper_reply.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[PRV] [" + player->name + "," + player->remote_ip + "] to [" +
                    private_target->name + "," + private_target->remote_ip + "] " + data);
            this->Client_SendEncoded(private_target, AC_PRIVATE, OP_CHAT, whisper_reply);
            return true;
        }
        // .. Talk Msg: world chat, while it is on and the global budget lasts ( not from the jail )
        if (action == AC_PLAY)
        {
            if (data.Length() < 1)
                return true;
            this->mysql_controls->NormalizePlayerText(&data);
            if (!this->settings->GetWorldCommunication())
            {
                String global_chat_reply = "Server";
                global_chat_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), global_chat_reply.Length() + 1);
                global_chat_reply.Insert("This channel is temporary disabled", global_chat_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_PLAY, OP_CHAT, global_chat_reply);
                return true;
            }
            if (this->settings->GetJailMap() == player->map_id)
                return false;
            if (player->global_tokens < 1)
            {
                if (player->admin_level < AdminLevel_LightGuide)
                    return true;
            }
            if (!this->mysql_controls->IsAsciiText(data))
                return true;
            // the adverts of two bot programs flag their sender
            if (AnsiPos("elebot.org for a", data) > 0)
            {
                this->Server_AppendChatLog("[SYS] [" + player->name + "] tagged as a BOT.");
                player->cheater_flag = true;
            }
            if (AnsiPos("Download Oxybot", data) > 0)
            {
                this->Server_AppendChatLog("[SYS] [" + player->name + "] tagged as a BOT.");
                player->cheater_flag = true;
            }
            String gm_report = player->name;
            gm_report.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), gm_report.Length() + 1);
            gm_report.Insert(data, gm_report.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[GLB] [" + player->name + "," + player->remote_ip + "] " + data);
            this->Server_BroadcastToGlobalChat(player, AC_PLAY, OP_CHAT, gm_report);
            player->global_tokens--;
            gm_report.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), gm_report.Length() + 1);
            // the last 7 lines are kept for whoever opens the channel
            for (int i = 1; i < GLOBAL_HISTORY_SIZE; i++)
                this->global_history[i - 1] = this->global_history[i];
            this->global_history[6] = gm_report;
            return true;
        }
        // .. Talk Admin: the admins' channel ( light guide and up )
        if (action == AC_CAST)
        {
            if (player->admin_level < AdminLevel_LightGuide)
                return false;
            if (data.Length() < 0)
                return false;
            this->mysql_controls->NormalizePlayerText(&data);
            String admin_chat_packet = player->name;
            admin_chat_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_chat_packet.Length() + 1);
            admin_chat_packet.Insert(data, admin_chat_packet.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[ADM] [" + player->name + "," + player->remote_ip + "] " + data);
            this->Admin_BroadcastToOtherAdmins(player, AC_CAST, OP_CHAT, admin_chat_packet);
            return true;
        }
        // .. Talk Announce: an announcement ( guardian and up )
        if (action == AC_GLOBAL)
        {
            if (player->admin_level < AdminLevel_Guardian)
                return false;
            if (data.Length() < 1)
                return true;
            this->mysql_controls->NormalizePlayerText(&data);
            if (!this->settings->GetWorldCommunication())
                return true;
            if (AnsiPos("elebot", data) > 0)
                return true;
            String announce_packet = player->name;
            announce_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), announce_packet.Length() + 1);
            announce_packet.Insert(data, announce_packet.Length() + 1);
            if (this->settings->GetChatLog())
                this->Server_AppendChatLog("[GLB2] [" + player->name + "," + player->remote_ip + "] " + data);
            this->Server_BroadcastToAllExceptSelf(player, AC_GLOBAL, OP_CHAT, announce_packet);
            return true;
        }
    }
    // .. Attack: within the attack budget, and queued like a step; a flagged bot loses 6 in 10
    if (family == OP_ATTACK)
    {
        if (!player->logged_in)
            return false;
        if (player->attack_tokens < 1)
            return true;
        player->attack_tokens--;
        if (player->cheater_flag && random(100) > 40)
            return true;
        TTimeStamp attack_stamp = DateTimeToTimeStamp(Now());
        int attack_delay = attack_stamp.Time - player->last_action_tick.Time;
        if (attack_delay > WALK_DELAY_SANITY_MS)
            attack_delay = MIN_ACTION_DELAY_MS;
        if (attack_delay < MIN_ACTION_DELAY_MS)
        {
            if (player->action_queue.size() > ACTION_QUEUE_LIMIT)
                return true;
            PlayerCommand attack_early_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), attack_early_command);
            return true;
        }
        else if (player->action_queue.size() > 0)
        {
            PlayerCommand attack_queued_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), attack_queued_command);
            return true;
        }
        else
            return this->Attack_Execute(player, action, &data);
    }
    // .. Spell: like Attack
    if (family == OP_MAGIC)
    {
        if (!player->logged_in)
            return false;
        if (player->attack_tokens < 1)
            return true;
        player->attack_tokens--;
        if (player->cheater_flag && random(100) > 40)
            return true;
        TTimeStamp spell_stamp = DateTimeToTimeStamp(Now());
        int spell_delay = spell_stamp.Time - player->last_action_tick.Time;
        if (spell_delay > WALK_DELAY_SANITY_MS)
            spell_delay = MIN_ACTION_DELAY_MS;
        if (spell_delay < MIN_ACTION_DELAY_MS)
        {
            if (player->action_queue.size() > ACTION_QUEUE_LIMIT)
                return true;
            PlayerCommand spell_early_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), spell_early_command);
            return true;
        }
        else if (player->action_queue.size() > 0)
        {
            PlayerCommand spell_queued_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), spell_queued_command);
            return true;
        }
        else
            return this->Spell_Execute(player, action, &data);
    }
    // .. Connection: the ping's answer, and the client's accept of the init
    if (family == OP_SECURITY)
    {
        if (action == AC_SYN1)
        {
            player->ping_timeout = 0;
            return true;
        }
        if (action == AC_CONFIRM)
        {
            // the accept must give back the player id and both encryption multiples
            if (data.Length() > 5)
            {
                if (this->EO_DecodeNumber(data.SubString(5, 2)) == (unsigned int)player->player_id &&
                    this->EO_DecodeNumber(data.SubString(3, 2)) == (unsigned int)player->server_encryption_multiple &&
                    this->EO_DecodeNumber(data.SubString(1, 2)) == (unsigned int)player->client_encryption_multiple)
                {
                    player->connected = true;
                    return true;
                }
                return false;
            }
        }
    }
    // .. Login: refused while access is locked, or while the ip logs in too often; the account is looked up
    if (family == OP_LOGIN && action == AC_REQUEST)
    {
        if (player->account_logged_in)
            return false;
        if (player->logged_in)
            return false;
        if (this->settings->GetAccessLock())
        {
            this->Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_Busy, 2) + "NO");
            return false;
        }
        if (this->Login_CheckQueueThreshold() && !this->logins->IsReservedIp(player->socket->RemoteAddress))
        {
            this->Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_Busy, 2) + "NO");
            return false;
        }
        this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
        String login_account = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String password = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        this->mysql_controls->Mysql_SubmitQuery(DbQueryType_Login, player->player_id, player->query_id, data,
            "SELECT ident, account, DECODE(password,'eoeokeyendl') as password, type, signup, serial_c, serial_h, "
            "ipaddress, banned FROM endl_accounts WHERE account = '" +
            login_account + "' LIMIT 1");
        return true;
    }
    // .. Account: Agree changes the password, Request checks a new name, Create makes the account
    if (family == OP_ACCOUNT)
    {
        if (action == AC_UPDATE)
        {
            if (!player->account_logged_in)
                return false;
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_AccountChangePassword, player->player_id,
                player->query_id, data,
                "SELECT ident, account, DECODE(password,'eoeokeyendl') as password, type, signup, serial_c, serial_h, "
                "ipaddress, banned FROM endl_accounts WHERE ident = '" +
                IntToStr((DWORD)player->account_ident) + "' LIMIT 1");
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->connected)
                return false;
            if (this->settings->GetAccountLock())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
                    EO_EncodeNumber(AccountReply_RequestDenied, 2) + "NO");
                return false;
            }
            if (player->remove_timer > 0)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
                    EO_EncodeNumber(AccountReply_RequestDenied, 2) + "NO");
                return false;
            }
            if (data.Length() < 4)
                return false;
            // a few seconds between account requests
            if (player->account_create_cooldown > 4)
                return true;
            player->account_create_cooldown = 6;
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_AccountCheck, player->player_id, player->query_id, data,
                "SELECT ident, account, DECODE(password,'eoeokeyendl') as password, type, signup, serial_c, serial_h, "
                "ipaddress, banned FROM endl_accounts WHERE account = '" +
                this->mysql_controls->Db_SanitizeString(data) + "' LIMIT 1");
            return true;
        }
        if (action == AC_NEW)
        {
            if (this->settings->GetAccountLock())
                return false;
            if (data.Length() < 2)
                return false;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String account = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_AccountCreate, player->player_id, player->query_id,
                data,
                "SELECT ident, account, DECODE(password,'eoeokeyendl') as password, type, signup, serial_c, serial_h, "
                "ipaddress, banned FROM endl_accounts WHERE account = '" +
                account + "' LIMIT 1");
            return true;
        }
    }
    // .. Character: Take asks the delete, Remove deletes, Request asks to make one, Create makes it
    if (family == OP_CHARACTER)
    {
        if (action == AC_GET)
        {
            if (player->account_logged_in)
                this->Client_SendEncoded(player, AC_SET, OP_CHARACTER,
                    EO_EncodeNumber(player->session_id, 2) + data.SubString(1, 4));
            return true;
        }
        if (action == AC_DELETE)
        {
            if (!player->account_logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            // the session id from the account's login
            if (this->EO_DecodeNumber(data.SubString(1, 2)) != (unsigned int)player->session_id)
                return false;
            player->session_id = random(50000) + 10000;
            int character_id = this->EO_DecodeNumber(data.SubString(3, 4));
            this->mysql_controls->Mysql_SubmitExec(player->account_ident,
                "DELETE FROM endl_characters WHERE ident = " + IntToStr(character_id) + " AND ident_account = " +
                IntToStr((DWORD)player->account_ident));
            this->mysql_controls->file_cache->characters_count--;
            if (player->character_slots[0])
            {
                if (player->character_slots[0]->character_id == character_id)
                {
                    Player *first_slot_removed = player->character_slots[0];
                    player->character_slots[0] = player->character_slots[1];
                    player->character_slots[1] = player->character_slots[2];
                    player->character_slots[2] = 0;
                    delete first_slot_removed;
                }
            }
            if (player->character_slots[1])
            {
                if (player->character_slots[1]->character_id == character_id)
                {
                    Player *second_slot_removed = player->character_slots[1];
                    player->character_slots[1] = player->character_slots[2];
                    player->character_slots[2] = 0;
                    delete second_slot_removed;
                }
            }
            if (player->character_slots[2])
            {
                if (player->character_slots[2]->character_id == character_id)
                {
                    Player *third_slot_removed = player->character_slots[2];
                    player->character_slots[2] = 0;
                    delete third_slot_removed;
                }
            }
            // .. the list again, the deleted one gone
            int character_count = 0;
            if (player->character_slots[0])
                character_count = 1;
            if (player->character_slots[1])
                character_count = 2;
            String character_reply = EO_EncodeNumber(CharacterReply_Deleted, 2);
            character_reply.Insert(EO_EncodeNumber(character_count, 1), character_reply.Length() + 1);
            character_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), character_reply.Length() + 1);
            for (int i = 0; i < 2; i++)
            {
                Player *character = player->character_slots[i];
                if (character)
                {
                    character_reply.Insert(character->name, character_reply.Length() + 1);
                    character_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->character_id, 4), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->level, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->gender, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->hair_style, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->hair_color, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->skin, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->admin_level, 1), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->boots_graphic_id, 2),
                        character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->armor_graphic_id, 2),
                        character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->hat_graphic_id, 2), character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->shield_graphic_id, 2),
                        character_reply.Length() + 1);
                    character_reply.Insert(EO_EncodeNumber(character->weapon_graphic_id, 2),
                        character_reply.Length() + 1);
                    character_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), character_reply.Length() + 1);
                }
            }
            this->Client_SendEncoded(player, AC_RESULT, OP_CHARACTER, character_reply);
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->account_logged_in)
                return false;
            if (player->character_slots[0] && player->character_slots[1] && player->character_slots[2])
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_CHARACTER,
                    EO_EncodeNumber(CharacterReply_FullRequest, 2) + "NO");
                return true;
            }
            this->Client_SendEncoded(player, AC_RESULT, OP_CHARACTER,
                EO_EncodeNumber(player->session_id, 2) + "OK");
            return true;
        }
        if (action == AC_NEW)
        {
            if (!player->account_logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            if (this->EO_DecodeNumber(data.SubString(1, 2)) != (unsigned int)player->session_id)
                return false;
            if (player->account_create_cooldown > 4)
                return true;
            player->account_create_cooldown = 6;
            int gender = this->EO_DecodeNumber(data.SubString(3, 2));
            int hair_style = this->EO_DecodeNumber(data.SubString(5, 2));
            int hair_color = this->EO_DecodeNumber(data.SubString(7, 2));
            int skin = this->EO_DecodeNumber(data.SubString(9, 2));
            String character_name = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakStringAt(2, data,
                this->EO_GetBreakByte(EO_BREAK_BYTE)));
            if (this->players->CharName_IsReserved(player, character_name))
                return false;
            if (character_name.Length() > 12)
                return false;
            // the client's ranges; a name of 4 to 12 letters
            if (gender < 0 || gender > 1 || hair_style < 1 || hair_style > 20 || hair_color < 0 || hair_color > 9 ||
                skin < 0 || skin > 3 || character_name.Length() < 4)
                return false;
            else if (player->character_slots[0] && player->character_slots[1] && player->character_slots[2])
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_CHARACTER,
                    EO_EncodeNumber(CharacterReply_Full, 2) + "NO");
                return true;
            }
            if (!this->PassesWordFilter(character_name))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_CHARACTER,
                    EO_EncodeNumber(CharacterReply_NotApproved, 2) + "NO");
                return true;
            }
            // a name with other characters bans for 4 hours
            if (!this->mysql_controls->IsAlphabeticText(character_name))
            {
                this->banned->AddBan(player->remote_ip, player->hdid, false, 14400);
                return false;
            }
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_CharacterCreate, player->player_id, player->query_id,
                data, "SELECT ident FROM endl_characters WHERE name = '" + character_name + "' LIMIT 1");
            return true;
        }
    }
    // .. Welcome: Request picks a character, Msg enters the game, Agree sends a file
    if (family == OP_PLAY)
    {
        if (action == AC_REQUEST)
        {
            if (!player->account_logged_in)
                return false;
            if (player->logged_in)
                return false;
            if (data.Length() != 4)
                return false;
            // a full server lets in only an ip a reserved name logged in from
            if (this->players->GetOnlineCount() > this->settings->GetMaxPlayers())
            {
                if (!this->logins->IsReservedIp(player->socket->RemoteAddress))
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_PLAY,
                        EO_EncodeNumber(WelcomeCode_ServerBusy, 2) + "NO");
                    return true;
                }
            }
            int select_selected_id = this->EO_DecodeNumber(data.SubString(1, 4));
            String welcome_reply = EO_EncodeNumber(WelcomeCode_SelectCharacter, 2);
            welcome_reply.Insert(EO_EncodeNumber(player->session_id, 2), welcome_reply.Length() + 1);
            welcome_reply.Insert(data.SubString(1, 4), welcome_reply.Length() + 1);
            for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
            {
                if (!player->character_slots[i])
                    continue;
                Player *welcome_select_slot = player->character_slots[i];
                if (welcome_select_slot->map_id < 1 || this->map_control->maps.size() <= (unsigned int)welcome_select_slot->map_id)
                    return false;
                if (welcome_select_slot->character_id != select_selected_id)
                    continue;
                // boxed in on all sides: the rescue spot
                if (!this->map_control->CountOpenNeighbors(welcome_select_slot->map_id, welcome_select_slot->x,
                    welcome_select_slot->y))
                {
                    welcome_select_slot->map_id = this->settings->GetRescueMap();
                    welcome_select_slot->x = this->settings->GetRescueX();
                    welcome_select_slot->y = this->settings->GetRescueY();
                }
                // .. the map's and the pub files' ids and sizes, then the character
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->map_id, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->map_control->maps[welcome_select_slot->map_id - 1].rid1, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->map_control->maps[welcome_select_slot->map_id - 1].rid2, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->map_control->maps[welcome_select_slot->map_id - 1].filesize,
                    3), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->item_values->rid[0], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->item_values->rid[1], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->item_values->num_records, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->npc_values->rid[0], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->npc_values->rid[1], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->npc_values->num_records, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->skill_values->rid[0], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->skill_values->rid[1], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->skill_values->num_records, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->class_values->rid[0], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->class_values->rid[1], 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(GUI->class_values->num_records, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(welcome_select_slot->name, welcome_reply.Length() + 1);
                welcome_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_reply.Length() + 1);
                welcome_reply.Insert(welcome_select_slot->title, welcome_reply.Length() + 1);
                welcome_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_reply.Length() + 1);
                welcome_reply.Insert(welcome_select_slot->guild_name, welcome_reply.Length() + 1);
                welcome_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_reply.Length() + 1);
                welcome_reply.Insert(welcome_select_slot->guild_rank_name, welcome_reply.Length() + 1);
                welcome_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->class_id, 1), welcome_reply.Length() + 1);
                String guild_tag = welcome_select_slot->guild_tag;
                // the tag is padded to 3
                if (guild_tag.Length() == 2)
                    guild_tag = guild_tag + " ";
                if (guild_tag.Length() == 1)
                    guild_tag = guild_tag + "  ";
                if (!guild_tag.Length())
                    guild_tag = guild_tag + "   ";
                welcome_reply.Insert(guild_tag, welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->admin_level, 1), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->level, 1), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->experience, 4), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->usage, 4), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->hp, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->max_hp, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->tp, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->max_tp, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->max_sp, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->stat_points, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->skill_points, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->karma + 1000, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->min_damage, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->max_damage, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->accuracy, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->evasion, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->armor, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_strength, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_wisdom, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_intelligence, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_agility, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_constitution, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->adj_charisma, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->boots_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->gloves_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->accessory_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->armor_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->belt_item_id, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->necklace_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->hat_item_id, 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->shield_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->weapon_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->ring1_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->ring2_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->armlet1_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->armlet2_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->bracer1_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->bracer2_item_id, 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(welcome_select_slot->guild_rank_id, 1),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetJailMap(), 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetRescueMap(), 2), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetRescueX(), 1), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetRescueY(), 1), welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetSpyAndLightGuideFloodRate(), 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetGuardianFloodRate(), 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetGameMasterFloodRate(), 2),
                    welcome_reply.Length() + 1);
                welcome_reply.Insert(EO_EncodeNumber(this->settings->GetHighGameMasterFloodRate(), 2),
                    welcome_reply.Length() + 1);
                // 250 on the start map
                if (this->settings->GetStartMap() == welcome_select_slot->map_id)
                    welcome_reply.Insert(EO_EncodeNumber(250, 1), welcome_reply.Length() + 1);
                else
                    welcome_reply.Insert(EO_EncodeNumber(0, 1), welcome_reply.Length() + 1);
                welcome_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_PLAY, welcome_reply);
                return true;
            }
            return false;
        }
        // .. Welcome Msg: the character enters the game
        if (action == AC_PLAY)
        {
            if (!player->account_logged_in)
                return false;
            if (player->logged_in)
                return false;
            if (data.Length() != 7)
                return false;
            if (this->EO_DecodeNumber(data.SubString(1, 3)) != (unsigned int)player->session_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PLAY,
                    EO_EncodeNumber(WelcomeCode_ServerBusy, 2) + "NO");
                return true;
            }
            int enter_selected_id = this->EO_DecodeNumber(data.SubString(4, 4));
            char welcome_found = 0;
            int i;
            for (i = 0; i < CHARACTER_SLOT_COUNT; i++)
            {
                if (!player->character_slots[i])
                    continue;
                Player *welcome_enter_slot = player->character_slots[i];
                // .. the character's row becomes the player
                if (welcome_enter_slot->character_id == enter_selected_id)
                {
                    welcome_found = 1;
                    player->character_id = welcome_enter_slot->character_id;
                    player->class_id = welcome_enter_slot->class_id;
                    player->guild_rank_id = welcome_enter_slot->guild_rank_id;
                    player->account_id = welcome_enter_slot->account_id;
                    player->name = welcome_enter_slot->name;
                    player->partner_name = this->mysql_controls->Mysql_SanitizeString(welcome_enter_slot->partner_name,
                        0);
                    player->guild_tag = this->mysql_controls->Mysql_SanitizeString(welcome_enter_slot->guild_tag, 1);
                    player->title = this->mysql_controls->Mysql_SanitizeString(welcome_enter_slot->title, 0);
                    player->guild_name = this->mysql_controls->Mysql_SanitizeString(welcome_enter_slot->guild_name, 0);
                    player->guild_rank_name = this->mysql_controls->Mysql_SanitizeString(welcome_enter_slot->guild_rank_name,
                        0);
                    player->experience = welcome_enter_slot->experience;
                    player->level = welcome_enter_slot->level;
                    player->signup = welcome_enter_slot->signup;
                    player->gender = welcome_enter_slot->gender;
                    player->hair_style = welcome_enter_slot->hair_style;
                    player->hair_color = welcome_enter_slot->hair_color;
                    player->skin = welcome_enter_slot->skin;
                    player->map_id = welcome_enter_slot->map_id;
                    player->x = welcome_enter_slot->x;
                    player->y = welcome_enter_slot->y;
                    player->direction = welcome_enter_slot->direction;
                    player->base_hp = welcome_enter_slot->base_hp;
                    player->max_hp = welcome_enter_slot->max_hp;
                    player->hp = welcome_enter_slot->hp;
                    player->base_tp = welcome_enter_slot->base_tp;
                    player->max_tp = welcome_enter_slot->max_tp;
                    player->tp = welcome_enter_slot->tp;
                    player->base_sp = welcome_enter_slot->base_sp;
                    player->max_sp = welcome_enter_slot->max_sp;
                    player->usage = welcome_enter_slot->usage;
                    player->money_bank = welcome_enter_slot->money_bank;
                    player->locker_bank = welcome_enter_slot->locker_bank;
                    player->stat_points = welcome_enter_slot->stat_points;
                    player->skill_points = welcome_enter_slot->skill_points;
                    player->karma = welcome_enter_slot->karma;
                    player->base_strength = welcome_enter_slot->base_strength;
                    player->base_wisdom = welcome_enter_slot->base_wisdom;
                    player->base_intelligence = welcome_enter_slot->base_intelligence;
                    player->base_agility = welcome_enter_slot->base_agility;
                    player->base_constitution = welcome_enter_slot->base_constitution;
                    player->base_charisma = welcome_enter_slot->base_charisma;
                    player->adj_strength = welcome_enter_slot->adj_strength;
                    player->adj_wisdom = welcome_enter_slot->adj_wisdom;
                    player->adj_intelligence = welcome_enter_slot->adj_intelligence;
                    player->adj_agility = welcome_enter_slot->adj_agility;
                    player->adj_constitution = welcome_enter_slot->adj_constitution;
                    player->adj_charisma = welcome_enter_slot->adj_charisma;
                    player->boots_graphic_id = welcome_enter_slot->boots_graphic_id;
                    player->boots_item_id = welcome_enter_slot->boots_item_id;
                    player->accessory_graphic_id = welcome_enter_slot->accessory_graphic_id;
                    player->accessory_item_id = welcome_enter_slot->accessory_item_id;
                    player->gloves_graphic_id = welcome_enter_slot->gloves_graphic_id;
                    player->gloves_item_id = welcome_enter_slot->gloves_item_id;
                    player->armor_graphic_id = welcome_enter_slot->armor_graphic_id;
                    player->armor_item_id = welcome_enter_slot->armor_item_id;
                    player->belt_graphic_id = welcome_enter_slot->belt_graphic_id;
                    player->belt_item_id = welcome_enter_slot->belt_item_id;
                    player->necklace_graphic_id = welcome_enter_slot->necklace_graphic_id;
                    player->necklace_item_id = welcome_enter_slot->necklace_item_id;
                    player->hat_graphic_id = welcome_enter_slot->hat_graphic_id;
                    player->hat_item_id = welcome_enter_slot->hat_item_id;
                    player->shield_graphic_id = welcome_enter_slot->shield_graphic_id;
                    player->shield_item_id = welcome_enter_slot->shield_item_id;
                    player->weapon_graphic_id = welcome_enter_slot->weapon_graphic_id;
                    player->weapon_item_id = welcome_enter_slot->weapon_item_id;
                    player->ring1_graphic_id = welcome_enter_slot->ring1_graphic_id;
                    player->ring1_item_id = welcome_enter_slot->ring1_item_id;
                    player->ring2_graphic_id = welcome_enter_slot->ring2_graphic_id;
                    player->ring2_item_id = welcome_enter_slot->ring2_item_id;
                    player->armlet1_graphic_id = welcome_enter_slot->armlet1_graphic_id;
                    player->armlet1_item_id = welcome_enter_slot->armlet1_item_id;
                    player->armlet2_graphic_id = welcome_enter_slot->armlet2_graphic_id;
                    player->armlet2_item_id = welcome_enter_slot->armlet2_item_id;
                    player->bracer1_graphic_id = welcome_enter_slot->bracer1_graphic_id;
                    player->bracer1_item_id = welcome_enter_slot->bracer1_item_id;
                    player->bracer2_graphic_id = welcome_enter_slot->bracer2_graphic_id;
                    player->bracer2_item_id = welcome_enter_slot->bracer2_item_id;
                    player->on_chair = welcome_enter_slot->on_chair;
                    player->admin_level = welcome_enter_slot->admin_level;
                    player->weight_current = welcome_enter_slot->weight_current;
                    player->home_id = welcome_enter_slot->home_id;
                    for (int element_slot = 0; element_slot < ELEMENT_COUNT; element_slot++)
                        player->element_resistances[element_slot] = welcome_enter_slot->element_resistances[element_slot];
                    player->class_min_damage = welcome_enter_slot->class_min_damage;
                    player->class_max_damage = welcome_enter_slot->class_max_damage;
                    player->class_accuracy = welcome_enter_slot->class_accuracy;
                    player->class_evasion = welcome_enter_slot->class_evasion;
                    player->class_armor = welcome_enter_slot->class_armor;
                    player->min_damage = welcome_enter_slot->min_damage;
                    player->max_damage = welcome_enter_slot->max_damage;
                    player->accuracy = welcome_enter_slot->accuracy;
                    player->evasion = welcome_enter_slot->evasion;
                    player->armor = welcome_enter_slot->armor;
                    player->equip_bonus_hp = welcome_enter_slot->equip_bonus_hp;
                    player->equip_bonus_tp = welcome_enter_slot->equip_bonus_tp;
                    player->equip_strength_bonus = welcome_enter_slot->equip_strength_bonus;
                    player->equip_wisdom_bonus = welcome_enter_slot->equip_wisdom_bonus;
                    player->equip_intelligence_bonus = welcome_enter_slot->equip_intelligence_bonus;
                    player->equip_agility_bonus = welcome_enter_slot->equip_agility_bonus;
                    player->equip_constitution_bonus = welcome_enter_slot->equip_constitution_bonus;
                    player->equip_charisma_bonus = welcome_enter_slot->equip_charisma_bonus;
                    player->restore_map = welcome_enter_slot->map_id;
                    player->restore_x = welcome_enter_slot->x;
                    player->restore_y = welcome_enter_slot->y;
                    player->on_chair = welcome_enter_slot->on_chair;
                    player->sitting = welcome_enter_slot->sitting;
                    player->inventory.clear();
                    player->trade_items.clear();
                    player->bank.clear();
                    player->action_queue.clear();
                    player->last_action_tick = DateTimeToTimeStamp(Now());
                    player->quest_trackers.clear();
                    player->quest_history.clear();
                    int total_gold = player->money_bank;
                    // .. the saved lists, id:amount: each: the bank, inventory, spells, quests
                    if (welcome_enter_slot->bankblob != "EOF" && welcome_enter_slot->bankblob.Length() > 3)
                    {
                        this->PacketReader_Init(welcome_enter_slot->bankblob, 58);
                        char bank_done = 0;
                        while (bank_done == 0)
                        {
                            String bank_id_text = PacketReader_GetBreakString();
                            if (bank_id_text != "EOF" && bank_id_text.Length() > 0)
                            {
                                try
                                {
                                    int bank_item_id = StrToInt(bank_id_text);
                                    int bank_amount = StrToInt(PacketReader_GetBreakString());
                                    PlayerInventory bank_inv(bank_item_id);
                                    bank_inv.amount = bank_amount;
                                    player->bank.insert(player->bank.end(), bank_inv);
                                }
                                catch (...)
                                {
                                    bank_done = 1;
                                }
                            }
                            else
                                bank_done = 1;
                        }
                    }
                    if (welcome_enter_slot->invblob1 != "EOF" && welcome_enter_slot->invblob1.Length() > 3)
                    {
                        this->PacketReader_Init(welcome_enter_slot->invblob1, 58);
                        char inventory_done = 0;
                        while (inventory_done == 0)
                        {
                            String inventory_id_text = PacketReader_GetBreakString();
                            if (inventory_id_text != "EOF" && inventory_id_text.Length() > 0)
                            {
                                try
                                {
                                    int inventory_item_id = StrToInt(inventory_id_text);
                                    int inventory_amount = StrToInt(PacketReader_GetBreakString());
                                    if (inventory_amount < 0)
                                        continue;
                                    // gold: a stack is held to 2000000
                                    if (inventory_item_id == 1)
                                    {
                                        total_gold += inventory_amount;
                                        if (inventory_amount > 2000000)
                                            inventory_amount = 2000000;
                                    }
                                    player->weight_current += inventory_amount *
                                        GUI->item_values->GetWeight(inventory_item_id);
                                    PlayerInventory inventory_inv(inventory_item_id);
                                    inventory_inv.amount = inventory_amount;
                                    player->inventory.insert(player->inventory.end(), inventory_inv);
                                }
                                catch (...)
                                {
                                    inventory_done = 1;
                                }
                            }
                            else
                                inventory_done = 1;
                        }
                    }
                    if (welcome_enter_slot->skillblob != "EOF" && welcome_enter_slot->skillblob.Length() > 3)
                    {
                        this->PacketReader_Init(welcome_enter_slot->skillblob, 58);
                        char skill_done = 0;
                        while (skill_done == 0)
                        {
                            String skill_id_text = PacketReader_GetBreakString();
                            if (skill_id_text != "EOF" && skill_id_text.Length() > 0)
                            {
                                try
                                {
                                    int skill_id = StrToInt(skill_id_text);
                                    int level = StrToInt(PacketReader_GetBreakString());
                                    PlayerSkill welcome_skill(skill_id);
                                    welcome_skill.level = level;
                                    player->spells.insert(player->spells.end(), welcome_skill);
                                }
                                catch (...)
                                {
                                    skill_done = 1;
                                }
                            }
                            else
                                skill_done = 1;
                        }
                    }
                    String quest_cache = welcome_enter_slot->quest_cache;
                    String quest_blob = welcome_enter_slot->quest_blob;
                    if (welcome_enter_slot->quest_cache != "EOF" && welcome_enter_slot->quest_cache.Length() > 3)
                    {
                        this->PacketReader_Init(welcome_enter_slot->quest_cache, 58);
                        char quest_cache_done = 0;
                        while (quest_cache_done == 0)
                        {
                            String quest_cache_id_text = PacketReader_GetBreakString();
                            if (quest_cache_id_text != "EOF" && quest_cache_id_text.Length() > 0)
                            {
                                try
                                {
                                    int cache_quest_id = StrToInt(quest_cache_id_text);
                                    int state_index = StrToInt(PacketReader_GetBreakString());
                                    int quest_cache_version = StrToInt(PacketReader_GetBreakString());
                                    // a quest in progress is kept only when its file is the same version
                                    if (this->quest_engine->GetQuestVersion(cache_quest_id) == quest_cache_version)
                                    {
                                        PlayerQuest cache_quest(cache_quest_id, state_index, quest_cache_version);
                                        cache_quest.counters[0] = StrToInt(PacketReader_GetBreakString());
                                        cache_quest.counters[1] = StrToInt(PacketReader_GetBreakString());
                                        cache_quest.counters[2] = StrToInt(PacketReader_GetBreakString());
                                        cache_quest.counters[3] = StrToInt(PacketReader_GetBreakString());
                                        cache_quest.counters[4] = StrToInt(PacketReader_GetBreakString());
                                        player->quest_trackers.insert(player->quest_trackers.end(), cache_quest);
                                        this->Player_ApplyQuestActions(player, &cache_quest, 0);
                                    }
                                }
                                catch (...)
                                {
                                    quest_cache_done = 1;
                                }
                            }
                            else
                                quest_cache_done = 1;
                        }
                    }
                    if (welcome_enter_slot->quest_blob != "EOF" && welcome_enter_slot->quest_blob.Length() > 3)
                    {
                        this->PacketReader_Init(welcome_enter_slot->quest_blob, 58);
                        char quest_done = 0;
                        while (quest_done == 0)
                        {
                            String quest_id_text = PacketReader_GetBreakString();
                            if (quest_id_text != "EOF" && quest_id_text.Length() > 0)
                            {
                                try
                                {
                                    int welcome_quest_id = StrToInt(quest_id_text);
                                    PlayerQuest quest(welcome_quest_id, 0, 0);
                                    player->quest_history.insert(player->quest_history.end(), quest);
                                }
                                catch (...)
                                {
                                    quest_done = 1;
                                }
                            }
                            else
                                quest_done = 1;
                        }
                    }
                    player->home_name = GUI->inn_values->GetName(player->home_id);
                    player->enter_game_timestamp = Now();
                    player->weight_max = player->adj_strength + 70;
                    if (player->weight_max > WEIGHT_DISPLAY_MAX)
                        player->weight_max = WEIGHT_DISPLAY_MAX;
                    break;
                }
            }
            // not one of the account's characters: a 4 hour ban
            if (!welcome_found)
            {
                this->banned->AddBan(player->remote_ip, player->hdid, false, 14400);
                return false;
            }
            if (!player->map_id)
                return false;
            if (!this->mysql_controls->IsAlphabeticText(player->name) && (player->name != "vult-r") != 0)
                return false;
            player->map_has_quakes = this->map_control->maps[player->map_id - 1].has_quakes;
            player->map_has_hp_drain = this->map_control->maps[player->map_id - 1].has_hp_drain;
            player->map_has_tp_drain = this->map_control->maps[player->map_id - 1].has_tp_drain;
            player->map_has_spikes = this->map_control->maps[player->map_id - 1].has_spikes;
            this->map_control->IncPlayerCount(player->map_id);
            // .. the join message, the 8 news lines, the weight, items, spells and what is in view
            String welcome_packet = EO_EncodeNumber(WelcomeCode_EnterGame, 2);
            welcome_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_packet.Length() + 1);
            welcome_packet.Insert(this->settings->GetJoinMessage(), welcome_packet.Length() + 1);
            welcome_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_packet.Length() + 1);
            for (int n = 0; n < 8; n++)
            {
                welcome_packet.Insert(GUI->news_control->Get(n), welcome_packet.Length() + 1);
                welcome_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_packet.Length() + 1);
            }
            int welcome_weight_current = player->weight_current;
            int welcome_weight_max = player->weight_max;
            if (welcome_weight_current > WEIGHT_DISPLAY_MAX)
                welcome_weight_current = WEIGHT_DISPLAY_MAX;
            if (welcome_weight_max > WEIGHT_DISPLAY_MAX)
                welcome_weight_max = WEIGHT_DISPLAY_MAX;
            welcome_packet.Insert(EO_EncodeNumber(welcome_weight_current, 1), welcome_packet.Length() + 1);
            welcome_packet.Insert(EO_EncodeNumber(welcome_weight_max, 1), welcome_packet.Length() + 1);
            vector<PlayerInventory>::iterator inventory_item;
            for (inventory_item = player->inventory.begin(); inventory_item != player->inventory.end();
                inventory_item++)
            {
                welcome_packet.Insert(EO_EncodeNumber(inventory_item->item_id, 2), welcome_packet.Length() + 1);
                welcome_packet.Insert(EO_EncodeNumber(inventory_item->amount, 4), welcome_packet.Length() + 1);
            }
            welcome_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_packet.Length() + 1);
            vector<PlayerSkill>::iterator spell;
            for (spell = player->spells.begin(); spell != player->spells.end(); spell++)
            {
                welcome_packet.Insert(EO_EncodeNumber(spell->skill_id, 2), welcome_packet.Length() + 1);
                welcome_packet.Insert(EO_EncodeNumber(spell->level, 2), welcome_packet.Length() + 1);
            }
            welcome_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), welcome_packet.Length() + 1);
            welcome_packet.Insert(Refresh_BuildReply(player), welcome_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_PLAY, welcome_packet);
            // the players near them see them arrive
            welcome_packet = this->EO_GetBreakByte(EO_BREAK_BYTE);
            welcome_packet.Insert(Player_SerializeAvatar(player, -1), welcome_packet.Length() + 1);
            welcome_packet.Insert(EO_EncodeNumber(1, 1), welcome_packet.Length() + 1);
            this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYERS, welcome_packet);
            player->logged_in = true;
            player->session_id = random(50000) + 10000;
            this->mysql_controls->Mysql_SubmitExec(player->account_ident,
                "UPDATE endl_characters SET online = 1 WHERE ident = " + IntToStr(player->character_id));
            this->players->UpdatePeakOnline();
            return true;
        }
        // .. Welcome Agree: the map, or a part of the item, npc, spell or class file
        if (action == AC_UPDATE)
        {
            if (data.Length() < 3)
                return false;
            if (this->EO_DecodeNumber(data.SubString(2, 2)) != (unsigned int)player->session_id)
                return false;
            int code = this->EO_DecodeNumber(String(data[1]));
            if (code == PubFileType_Emf)
                this->Client_SendRaw(player,
                    this->map_control->ReadRawFile(this->EO_DecodeNumber(data.SubString(4, 2))), 5);
            if (code == PubFileType_Eif)
            {
                int item_list_index = this->EO_DecodeNumber(data.SubString(4, 1));
                if (item_list_index > 0)
                {
                    if (GUI->item_values->string_list->Count >= item_list_index)
                        this->Client_SendRaw(player,
                            EO_EncodeNumber(item_list_index, 1) +
                            GUI->item_values->string_list->Strings[item_list_index - 1], 6);
                }
            }
            if (code == PubFileType_Enf)
            {
                int npc_list_index = this->EO_DecodeNumber(data.SubString(4, 1));
                if (npc_list_index > 0)
                {
                    if (GUI->npc_values->string_list->Count >= npc_list_index)
                        this->Client_SendRaw(player,
                            EO_EncodeNumber(npc_list_index, 1) +
                            GUI->npc_values->string_list->Strings[npc_list_index - 1], 7);
                }
            }
            if (code == PubFileType_Esf)
            {
                int skill_list_index = this->EO_DecodeNumber(data.SubString(4, 1));
                if (skill_list_index > 0)
                {
                    if (GUI->skill_values->string_list->Count >= skill_list_index)
                        this->Client_SendRaw(player,
                            EO_EncodeNumber(skill_list_index, 1) +
                            GUI->skill_values->string_list->Strings[skill_list_index - 1], 8);
                }
            }
            if (code == PubFileType_Ecf)
            {
                int class_list_index = this->EO_DecodeNumber(data.SubString(4, 1));
                if (class_list_index > 0)
                {
                    if (GUI->class_values->string_list->Count >= class_list_index)
                        this->Client_SendRaw(player,
                            EO_EncodeNumber(class_list_index, 1) +
                            GUI->class_values->string_list->Strings[class_list_index - 1], 12);
                }
            }
            return true;
        }
    }
    // .. Range: the client asks about players and npcs it sees but does not know
    if (family == OP_RANGE && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 1)
            return false;
        this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
        String players = PacketReader_GetBreakString();
        String npcs = PacketReader_GetBreakString();
        String range_reply = (char)this->EO_GetBreakByte(EO_BREAK_BYTE);
        int range_count = 0;
        if (players.Length() > 0)
        {
            int i;
            for (i = 0; players.Length() >= 2 * i + 2; i++)
            {
                int range_player_id = this->EO_DecodeNumber(players.SubString(2 * i + 1, 2));
                Player *range_target = this->players->GetById(range_player_id);
                if (range_target && range_target->map_id == player->map_id)
                {
                    range_count++;
                    range_reply.Insert(Player_SerializeAvatar(range_target, -1), range_reply.Length() + 1);
                }
            }
        }
        range_reply.Insert(EO_EncodeNumber(range_count, 1), 1);
        if (npcs.Length() > 0)
        {
            for (int j = 0; npcs.Length() >= j + 1; j++)
            {
                int range_npc_id = this->EO_DecodeNumber(npcs.SubString(j + 1, 1));
                String range_name = NpcRange_Lookup(player, range_npc_id);
                if (range_name.Length() > 0)
                    range_reply.Insert(range_name, range_reply.Length() + 1);
            }
        }
        if (range_reply.Length() < 3)
            return true;
        this->Client_SendEncoded(player, AC_RESULT, OP_RANGE, range_reply);
        return true;
    }
    // .. PlayerRange: one player on the same map
    if (family == OP_PLAYERRANGE && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 2)
            return false;
        int player_range_target_id = this->EO_DecodeNumber(data.SubString(1, 2));
        Player *player_range_target = this->players->GetById(player_range_target_id);
        if (!player_range_target)
            return true;
        if (player_range_target->map_id != player->map_id)
            return true;
        String player_range_reply = (char)this->EO_GetBreakByte(EO_BREAK_BYTE);
        player_range_reply.Insert(Player_SerializeAvatar(player_range_target, -1), player_range_reply.Length() + 1);
        player_range_reply.Insert(EO_EncodeNumber(1, 1), 1);
        if (player_range_reply.Length() < 3)
            return true;
        this->Client_SendEncoded(player, AC_RESULT, OP_RANGE, player_range_reply);
        return true;
    }
    // .. NpcRange: npcs by their index on the map
    if (family == OP_NPCRANGE && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 3)
            return false;
        int id_count = data.Length() - 2;
        int npc_range_count = 0;
        String npc_range_names = "";
        int i;
        for (i = 0; i < id_count; i++)
        {
            int npc_range_id = this->EO_DecodeNumber(data.SubString(i + 3, 1));
            String npc_range_name = NpcRange_Lookup(player, npc_range_id);
            if (npc_range_name.Length() > 0)
            {
                npc_range_names.Insert(npc_range_name, npc_range_names.Length() + 1);
                npc_range_count++;
            }
        }
        if (npc_range_count < 1)
            return true;
        npc_range_names.Insert(EO_EncodeNumber(npc_range_count, 1), 1);
        this->Client_SendEncoded(player, AC_UPDATE, OP_NPC, npc_range_names);
        return true;
    }
    // .. Face: a turn, queued like a step
    if (family == OP_DIR)
    {
        TTimeStamp face_stamp = DateTimeToTimeStamp(Now());
        int face_delay = face_stamp.Time - player->last_action_tick.Time;
        if (face_delay > WALK_DELAY_SANITY_MS)
            face_delay = MIN_ACTION_DELAY_MS;
        if (face_delay < MIN_ACTION_DELAY_MS)
        {
            if (player->action_queue.size() > ACTION_QUEUE_LIMIT)
                return true;
            PlayerCommand face_early_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), face_early_command);
            return true;
        }
        else if (player->action_queue.size() > 0)
        {
            PlayerCommand face_queued_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), face_queued_command);
            return true;
        }
        else
            return this->Face_Execute(player, action, &data);
    }
    // .. Players: List the online names, Accept finds a player, Request the full online list
    if (family == OP_PLAYERS)
    {
        if (action == AC_LIST)
        {
            if (!player->logged_in)
                return false;
            this->Client_SendRaw(player, Server_BuildOnlineNames(), 11);
            return true;
        }
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4 || data.Length() > 16)
                return false;
            Player *found_player = this->players->FindByName(data);
            if (!found_player)
            {
                this->Client_SendEncoded(player, AC_SYN1, OP_PLAYERS, data);
                return true;
            }
            // a hidden player is not found
            if (found_player->hide_online)
            {
                this->Client_SendEncoded(player, AC_SYN1, OP_PLAYERS, data);
                return true;
            }
            // the reply says whether they are on your map
            if (found_player->map_id == player->map_id)
            {
                this->Client_SendEncoded(player, AC_SYN2, OP_PLAYERS, data);
                return true;
            }
            this->Client_SendEncoded(player, AC_SYN3, OP_PLAYERS, data);
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            this->Client_SendRaw(player, Server_BuildOnlineList(), 9);
            return true;
        }
    }
    // .. Emote: emotes 1 to 10 and 14
    if (family == OP_EMOTE && action == AC_PUBLIC)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 1)
            return false;
        int emote = this->EO_DecodeNumber(String(data[1]));
        if (this->EO_DecodeNumber(String(data[1])) > 10 && this->EO_DecodeNumber(String(data[1])) != 14)
            return true;
        String emote_packet = EO_EncodeNumber(player->player_id, 2);
        emote_packet.Insert(data[1], emote_packet.Length() + 1);
        this->Server_BroadcastNearby(player, AC_SET, OP_EMOTE, emote_packet);
        return true;
    }
    // .. Refresh: everything in view again; queued actions are dropped
    if (family == OP_REFRESH && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        player->flush_queue = true;
        this->Client_SendEncoded(player, AC_RESULT, OP_REFRESH, Refresh_BuildReply(player));
        return true;
    }
    // .. Sit: 1 sits on the floor or stands up; on a chair it goes to Chair
    if (family == OP_REST && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 1)
            return false;
        int sit_action = this->EO_DecodeNumber(String(data[1]));
        if (sit_action == 1)
        {
            if (!player->sitting && !player->on_chair)
            {
                player->sitting = 1;
                player->on_chair = 0;
                String sit_down_msg = EO_EncodeNumber(player->player_id, 2);
                sit_down_msg.Insert(EO_EncodeNumber(player->x, 1), sit_down_msg.Length() + 1);
                sit_down_msg.Insert(EO_EncodeNumber(player->y, 1), sit_down_msg.Length() + 1);
                sit_down_msg.Insert(EO_EncodeNumber(player->direction, 1), sit_down_msg.Length() + 1);
                sit_down_msg.Insert(EO_EncodeNumber(0, 1), sit_down_msg.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_REST, sit_down_msg);
                this->Server_BroadcastNearby(player, AC_SET, OP_REST, sit_down_msg);
                return true;
            }
            else
            {
                if (!player->on_chair)
                {
                    player->on_chair = 0;
                    player->sitting = 0;
                    String stand_up_msg = EO_EncodeNumber(player->player_id, 2);
                    stand_up_msg.Insert(EO_EncodeNumber(player->x, 1), stand_up_msg.Length() + 1);
                    stand_up_msg.Insert(EO_EncodeNumber(player->y, 1), stand_up_msg.Length() + 1);
                    this->Client_SendEncoded(player, AC_CLOSE, OP_REST, stand_up_msg);
                    this->Server_BroadcastNearby(player, AC_DELETE, OP_REST, stand_up_msg);
                    return true;
                }
                else
                    family = OP_SIT;
            }
        }
        else
        {
            if (!player->sitting)
                return true;
            player->on_chair = 0;
            player->sitting = 0;
            String floor_stand_msg = EO_EncodeNumber(player->player_id, 2);
            floor_stand_msg.Insert(EO_EncodeNumber(player->x, 1), floor_stand_msg.Length() + 1);
            floor_stand_msg.Insert(EO_EncodeNumber(player->y, 1), floor_stand_msg.Length() + 1);
            this->Client_SendEncoded(player, AC_CLOSE, OP_REST, floor_stand_msg);
            this->Server_BroadcastNearby(player, AC_DELETE, OP_REST, floor_stand_msg);
            return true;
        }
    }
    // .. Chair: queued like a step
    if (family == OP_SIT)
    {
        TTimeStamp chair_stamp = DateTimeToTimeStamp(Now());
        int chair_delay = chair_stamp.Time - player->last_action_tick.Time;
        if (chair_delay > WALK_DELAY_SANITY_MS)
            chair_delay = MIN_ACTION_DELAY_MS;
        if (chair_delay < MIN_ACTION_DELAY_MS)
        {
            if (player->action_queue.size() > ACTION_QUEUE_LIMIT)
                return true;
            PlayerCommand chair_early_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), chair_early_command);
            return true;
        }
        else if (player->action_queue.size() > 0)
        {
            PlayerCommand chair_queued_command(family, action, data);
            player->action_queue.insert(player->action_queue.end(), chair_queued_command);
            return true;
        }
        else
            return this->Chair_Execute(player, action, &data);
    }
    // .. Door: opens a door in view; a locked one needs its key
    if (family == OP_DOOR && action == AC_OPEN)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 2)
            return false;
        MapCoord coord;
        coord.x = this->EO_DecodeNumber(data.SubString(1, 1));
        coord.y = this->EO_DecodeNumber(data.SubString(2, 1));
        if (this->Server_InViewRange(coord.x, coord.y, player->x, player->y))
        {
            String encoded = EO_EncodeNumber(coord.x, 1);
            encoded.Insert(EO_EncodeNumber(coord.y, 2), encoded.Length() + 1);
            int door_key = this->map_control->GetDoorKeyAt(player->map_id, coord);
            if (door_key > 0)
            {
                if (this->players->Player_HasKeyItem(player, door_key))
                {
                    if (this->map_control->OpenDoor(player->map_id, coord.x, coord.y))
                        this->Server_BroadcastNearTile(-13, player->map_id, coord, AC_OPEN, OP_DOOR,
                            encoded);
                }
                else
                    this->Client_SendEncoded(player, AC_CLOSE, OP_DOOR,
                        EO_EncodeNumber(door_key, 2));
                return true;
            }
            if (this->map_control->OpenDoor(player->map_id, coord.x, coord.y))
                this->Server_BroadcastNearTile(-13, player->map_id, coord, AC_OPEN, OP_DOOR,
                    encoded);
        }
        return true;
    }
    if (family == OP_ITEM)
    {
        // .. Item Use: one of the item is used up, then it acts by its type
        if (action == AC_DO)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int use_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            int item_type = GUI->item_values->GetType(use_item_id);
            if (!this->players->Player_RemoveItem(player, use_item_id, 1))
            {
                this->Client_SendEncoded(player, AC_UPDATE, OP_ITEM,
                    EO_EncodeNumber(use_item_id, 2));
                return true;
            }
            player->weight_current -= GUI->item_values->GetWeight(use_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int item_weight_current = player->weight_current;
            int item_weight_max = player->weight_max;
            if (item_weight_current > WEIGHT_DISPLAY_MAX)
                item_weight_current = WEIGHT_DISPLAY_MAX;
            if (item_weight_max > WEIGHT_DISPLAY_MAX)
                item_weight_max = WEIGHT_DISPLAY_MAX;
            // takes off the cursed items
            if (item_type == EifItemType_CureCurse)
            {
                if (this->players->Player_RemoveCursedItems(player))
                {
                    String cure_curse_packet = "";
                    cure_curse_packet.Insert(EO_EncodeNumber(player->player_id, 2), cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(1, 1), cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(0, 1), cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(player->boots_graphic_id, 2),
                        cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(player->armor_graphic_id, 2),
                        cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(player->hat_graphic_id, 2),
                        cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(player->weapon_graphic_id, 2),
                        cure_curse_packet.Length() + 1);
                    cure_curse_packet.Insert(EO_EncodeNumber(player->shield_graphic_id, 2),
                        cure_curse_packet.Length() + 1);
                    this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYER, cure_curse_packet);
                }
                player->CalculateAdjStats();
                this->Player_CalculateStats(player);
                player->CalculateHP_TP_SP();
                String cure_curse_reply = EO_EncodeNumber(item_type, 1);
                cure_curse_reply.Insert(EO_EncodeNumber(use_item_id, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(item_weight_current, 1), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(item_weight_max, 1), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->max_hp, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->max_tp, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_strength, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_intelligence, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_wisdom, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_agility, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_constitution, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->adj_charisma, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->min_damage, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->max_damage, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->accuracy, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->evasion, 2), cure_curse_reply.Length() + 1);
                cure_curse_reply.Insert(EO_EncodeNumber(player->armor, 2), cure_curse_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, cure_curse_reply);
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            // shows its effect to the players near them
            if (item_type == EifItemType_EffectPotion)
            {
                int effect_id = GUI->item_values->GetSpec1(use_item_id);
                String effect_potion_packet = EO_EncodeNumber(item_type, 1);
                effect_potion_packet.Insert(EO_EncodeNumber(use_item_id, 2), effect_potion_packet.Length() + 1);
                effect_potion_packet.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    effect_potion_packet.Length() + 1);
                effect_potion_packet.Insert(EO_EncodeNumber(item_weight_current, 1), effect_potion_packet.Length() + 1);
                effect_potion_packet.Insert(EO_EncodeNumber(item_weight_max, 1), effect_potion_packet.Length() + 1);
                effect_potion_packet.Insert(EO_EncodeNumber(effect_id, 2), effect_potion_packet.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, effect_potion_packet);
                effect_potion_packet = EO_EncodeNumber(player->player_id, 2);
                effect_potion_packet.Insert(EO_EncodeNumber(effect_id, 3), effect_potion_packet.Length() + 1);
                this->Server_BroadcastNearby(player, AC_SET, OP_EFFECT, effect_potion_packet);
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            if (item_type == EifItemType_HairDye)
            {
                int dye_color = GUI->item_values->GetSpec1(use_item_id);
                player->hair_color = dye_color;
                String hair_dye_packet = EO_EncodeNumber(item_type, 1);
                hair_dye_packet.Insert(EO_EncodeNumber(use_item_id, 2), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(player->item_change_remaining, 4), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(item_weight_current, 1), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(item_weight_max, 1), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(dye_color, 1), hair_dye_packet.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, hair_dye_packet);
                hair_dye_packet = EO_EncodeNumber(player->player_id, 2);
                hair_dye_packet.Insert(EO_EncodeNumber(3, 1), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(0, 1), hair_dye_packet.Length() + 1);
                hair_dye_packet.Insert(EO_EncodeNumber(dye_color, 1), hair_dye_packet.Length() + 1);
                this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYER, hair_dye_packet);
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            if (item_type == EifItemType_Alcohol)
            {
                String alcohol_reply = EO_EncodeNumber(item_type, 1);
                alcohol_reply.Insert(EO_EncodeNumber(use_item_id, 2), alcohol_reply.Length() + 1);
                alcohol_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), alcohol_reply.Length() + 1);
                alcohol_reply.Insert(EO_EncodeNumber(item_weight_current, 1), alcohol_reply.Length() + 1);
                alcohol_reply.Insert(EO_EncodeNumber(item_weight_max, 1), alcohol_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, alcohol_reply);
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            // the experience in spec1, and the levels it brings
            if (item_type == EifItemType_ExpReward)
            {
                player->experience += GUI->item_values->GetSpec1(use_item_id);
                int levels = this->players->Player_TryLevelUp(player);
                if (levels > 0)
                    this->Server_BroadcastNearby(player, AC_CONFIRM, OP_ITEM,
                        EO_EncodeNumber(player->player_id, 2));
                String exp_reward_reply = EO_EncodeNumber(item_type, 1);
                exp_reward_reply.Insert(EO_EncodeNumber(use_item_id, 2), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(item_weight_current, 1), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(item_weight_max, 1), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->experience, 4), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(levels, 1), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->stat_points, 2), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->skill_points, 2), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->max_hp, 2), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->max_tp, 2), exp_reward_reply.Length() + 1);
                exp_reward_reply.Insert(EO_EncodeNumber(player->max_sp, 2), exp_reward_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, exp_reward_reply);
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            if (item_type == EifItemType_Heal)
            {
                int hp = GUI->item_values->GetHP(use_item_id);
                int tp = GUI->item_values->GetTP(use_item_id);
                player->hp += hp;
                player->tp += tp;
                if (player->hp > player->max_hp)
                    player->hp = player->max_hp;
                if (player->tp > player->max_tp)
                    player->tp = player->max_tp;
                String heal_packet = EO_EncodeNumber(item_type, 1);
                heal_packet.Insert(EO_EncodeNumber(use_item_id, 2), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(player->item_change_remaining, 4), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(item_weight_current, 1), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(item_weight_max, 1), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(hp, 4), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(player->hp, 2), heal_packet.Length() + 1);
                heal_packet.Insert(EO_EncodeNumber(player->tp, 2), heal_packet.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, heal_packet);
                if (hp > 0)
                {
                    int percent = 100 * player->hp / player->max_hp;
                    heal_packet = EO_EncodeNumber(player->player_id, 2);
                    heal_packet.Insert(EO_EncodeNumber(hp, 4), heal_packet.Length() + 1);
                    heal_packet.Insert(EO_EncodeNumber(percent, 1), heal_packet.Length() + 1);
                    this->Server_BroadcastNearby(player, AC_UPDATE, OP_GAMEBAR, heal_packet);
                    if (player->in_party)
                    {
                        String heal_msg = EO_EncodeNumber(player->player_id, 2);
                        heal_msg.Insert(EO_EncodeNumber(player->HpPercent(), 1), heal_msg.Length() + 1);
                        this->Server_BroadcastToParty(player, AC_UPDATE, OP_GROUP, heal_msg);
                    }
                }
                this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
                return true;
            }
            // .. a scroll: to its map and spot, or with none set to their inn
            if (item_type == EifItemType_Teleport)
            {
                if (!player->map_id)
                    return true;
                // a map with no_scroll set gives the scroll back unused
                if (this->map_control->GetNoScroll(player->map_id))
                {
                    this->players->Player_AddItem(player, use_item_id, 1);
                    return true;
                }
                if (GUI->item_values->GetLevelRequirement(use_item_id) > player->level)
                {
                    String teleport_reply = "The scroll is unreadable, it requires level " +
                        IntToStr(GUI->item_values->GetLevelRequirement(use_item_id));
                    this->Client_SendEncoded(player, AC_SYSTEM, OP_CHAT, teleport_reply);
                    this->players->Player_AddItem(player, use_item_id, 1);
                    return true;
                }
                int scroll_map = GUI->item_values->GetScrollMap(use_item_id);
                MapCoord spec = GUI->item_values->GetSpecXY(use_item_id);
                if (scroll_map < 1 || (int)((int)this->map_control->maps.size()) < scroll_map)
                {
                    if (!scroll_map && !spec.spec2 && !spec.spec3)
                    {
                        scroll_map = GUI->inn_values->GetSpawnMap(player->home_id, player->level);
                        spec.spec2 = GUI->inn_values->GetSpawnX(player->home_id, player->level);
                        spec.spec3 = GUI->inn_values->GetSpawnY(player->home_id, player->level);
                    }
                    else
                        return true;
                }
                if (this->map_control->maps[scroll_map - 1].width < 1 ||
                    this->map_control->maps[scroll_map - 1].height < 1)
                    return true;
                String teleport_item_reply = EO_EncodeNumber(4, 1);
                teleport_item_reply.Insert(EO_EncodeNumber(use_item_id, 2), teleport_item_reply.Length() + 1);
                teleport_item_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    teleport_item_reply.Length() + 1);
                teleport_item_reply.Insert(EO_EncodeNumber(item_weight_current, 1), teleport_item_reply.Length() + 1);
                teleport_item_reply.Insert(EO_EncodeNumber(item_weight_max, 1), teleport_item_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_ITEM, teleport_item_reply);
                this->Player_Warp(player, scroll_map, spec, WarpEffect_Scroll, 0);
                return true;
            }
        }
        // .. Item Drop: within 2 tiles, on a clear tile, within the drop budget; lore items stay
        if (action == AC_DROP)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 7)
                return false;
            int drop_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            unsigned int item_drop_amount = this->EO_DecodeNumber(data.SubString(3, 3));
            int x = this->EO_DecodeNumber(data.SubString(6, 1));
            int y = this->EO_DecodeNumber(data.SubString(7, 1));
            if (GUI->item_values->GetSpecial(drop_item_id) == ItemSpecial_Lore)
                return true;
            if (this->settings->GetJailMap() == player->map_id)
                return false;
            if (player->drop_counter < 1)
                return true;
            player->drop_counter--;
            if (item_drop_amount > 10000000 || item_drop_amount < 1)
                return true;
            if (x > 250 || y > 250)
            {
                x = player->x;
                y = player->y;
            }
            // a map holds 1000 items at most
            if (this->map_control->maps[player->map_id - 1].ground_items.size() > 999)
                return true;
            if (!this->Coords_IsWithinTwo(player->x, player->y, x, y))
                return true;
            if (!this->map_control->IsDropTileClear(player->map_id, x, y))
                return true;
            if (!this->map_control->CanDropItemAt(player->map_id, x, y, player->account_ident))
                return true;
            if (!this->players->Player_RemoveItem(player, drop_item_id, item_drop_amount))
            {
                this->Client_SendEncoded(player, AC_UPDATE, OP_ITEM,
                    EO_EncodeNumber(drop_item_id, 2));
                return true;
            }
            // theirs alone for 6 seconds
            int drop_ground_index = this->map_control->AddGroundItem(player->map_id, drop_item_id, x, y,
                player->item_change_count, player->account_ident, 6);
            if (drop_ground_index < 0)
                return true;
            player->weight_current -= player->item_change_count * GUI->item_values->GetWeight(drop_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int drop_weight_current = player->weight_current;
            int drop_weight_max = player->weight_max;
            if (drop_weight_current > WEIGHT_DISPLAY_MAX)
                drop_weight_current = WEIGHT_DISPLAY_MAX;
            if (drop_weight_max > WEIGHT_DISPLAY_MAX)
                drop_weight_max = WEIGHT_DISPLAY_MAX;
            String item_drop_packet = EO_EncodeNumber(drop_item_id, 2);
            item_drop_packet.Insert(EO_EncodeNumber(drop_ground_index, 2), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(player->item_change_count, 3), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(x, 1), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(y, 1), item_drop_packet.Length() + 1);
            this->Server_BroadcastNearby(player, AC_ADD, OP_ITEM, item_drop_packet);
            item_drop_packet = EO_EncodeNumber(drop_item_id, 2);
            item_drop_packet.Insert(EO_EncodeNumber(player->item_change_count, 3), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(player->item_change_remaining, 4), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(drop_ground_index, 2), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(x, 1), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(y, 1), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(drop_weight_current, 1), item_drop_packet.Length() + 1);
            item_drop_packet.Insert(EO_EncodeNumber(drop_weight_max, 1), item_drop_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_DROP, OP_ITEM, item_drop_packet);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        // .. Item Junk: destroys the amount
        if (action == AC_JUNK)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 5)
                return false;
            int junk_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            unsigned int item_junk_amount = this->EO_DecodeNumber(data.SubString(3, 3));
            if (item_junk_amount > 10000000)
                return true;
            if (!this->players->Player_RemoveItem(player, junk_item_id, item_junk_amount))
            {
                this->Client_SendEncoded(player, AC_UPDATE, OP_ITEM,
                    EO_EncodeNumber(junk_item_id, 2));
                return true;
            }
            player->weight_current -= player->item_change_count * GUI->item_values->GetWeight(junk_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int junk_weight_current = player->weight_current;
            int junk_weight_max = player->weight_max;
            if (junk_weight_current > WEIGHT_DISPLAY_MAX)
                junk_weight_current = WEIGHT_DISPLAY_MAX;
            if (junk_weight_max > WEIGHT_DISPLAY_MAX)
                junk_weight_max = WEIGHT_DISPLAY_MAX;
            String item_junk_reply = EO_EncodeNumber(junk_item_id, 2);
            item_junk_reply.Insert(EO_EncodeNumber(player->item_change_count, 3), item_junk_reply.Length() + 1);
            item_junk_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), item_junk_reply.Length() + 1);
            item_junk_reply.Insert(EO_EncodeNumber(junk_weight_current, 1), item_junk_reply.Length() + 1);
            item_junk_reply.Insert(EO_EncodeNumber(junk_weight_max, 1), item_junk_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_JUNK, OP_ITEM, item_junk_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        // .. Item Get: picks up an item within 2 tiles
        if (action == AC_PICKUP)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int pickup_ground_index = this->EO_DecodeNumber(data.SubString(1, 2));
            GroundItemInfo info = this->map_control->GetGroundItemInfo(player->map_id, pickup_ground_index,
                player->account_ident);
            // someone else's, still theirs alone
            if (info.x == -2)
            {
                String item_pickup_reply = EO_EncodeNumber(2, 2);
                this->Client_SendEncoded(player, AC_HIT, OP_ITEM, item_pickup_reply);
                return true;
            }
            if (info.x < 0 || info.y < 0)
                return true;
            if (!this->Coords_IsWithinTwo(player->x, player->y, info.x, info.y))
                return true;
            this->map_control->RemoveGroundItem(player->map_id, pickup_ground_index);
            this->players->Player_AddItem(player, info.item_id, info.amount);
            player->weight_current += info.amount * GUI->item_values->GetWeight(info.item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int get_weight_current = player->weight_current;
            int get_weight_max = player->weight_max;
            if (get_weight_current > WEIGHT_DISPLAY_MAX)
                get_weight_current = WEIGHT_DISPLAY_MAX;
            if (get_weight_max > WEIGHT_DISPLAY_MAX)
                get_weight_max = WEIGHT_DISPLAY_MAX;
            String pickup_reply = EO_EncodeNumber(pickup_ground_index, 2);
            this->Server_BroadcastNearby(player, AC_DELETE, OP_ITEM, pickup_reply);
            pickup_reply = EO_EncodeNumber(pickup_ground_index, 2);
            pickup_reply.Insert(EO_EncodeNumber(info.item_id, 2), pickup_reply.Length() + 1);
            pickup_reply.Insert(EO_EncodeNumber(info.amount, 3), pickup_reply.Length() + 1);
            pickup_reply.Insert(EO_EncodeNumber(get_weight_current, 1), pickup_reply.Length() + 1);
            pickup_reply.Insert(EO_EncodeNumber(get_weight_max, 1), pickup_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_PICKUP, OP_ITEM, pickup_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
    }
    // .. Warp: Take asks for the new map's file, Accept arrives at the warp's spot
    if (family == OP_MAP)
    {
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int read_map = this->EO_DecodeNumber(data.SubString(1, 2));
            int warp_session_id = this->EO_DecodeNumber(data.SubString(3, 2));
            if (player->session_id != warp_session_id)
                return false;
            if (player->warp_map != read_map)
                return false;
            // an open trade is closed
            if (player->trade_partner_id > 0)
            {
                Player *warp_switch_target = this->players->by_id[player->trade_partner_id];
                if (warp_switch_target)
                    this->Client_SendEncoded(warp_switch_target, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(player->player_id, 2));
            }
            this->Server_BroadcastNearby(player, AC_DELETE, OP_PLAYER,
                EO_EncodeNumber(player->player_id, 2) + EO_EncodeNumber(player->warp_effect, 1));
            if (player->map_id > 0)
            {
                player->map_has_quakes = 0;
                player->map_has_hp_drain = 0;
                player->map_has_tp_drain = 0;
                player->map_has_spikes = 0;
                this->map_control->DecPlayerCount(player->map_id);
            }
            player->map_switch_pending = true;
            player->restore_map = player->map_id;
            player->restore_x = player->x;
            player->restore_y = player->y;
            player->map_id = 0;
            player->x = 0;
            player->y = 0;
            player->idle_ticks = 0;
            player->dead = false;
            player->trade_partner_id = -1;
            player->guild_inviter_id = -1;
            player->session_token = -1;
            player->guild_recruiter_name = "";
            this->Client_SendRaw(player, this->map_control->ReadRawFile(read_map), 4);
            return true;
        }
        // .. Warp Accept
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            if (player->warp_effect < 0)
                return true;
            int warp_map = this->EO_DecodeNumber(data.SubString(1, 2));
            int warp_token = this->EO_DecodeNumber(data.SubString(3, 2));
            if (player->session_id != warp_token)
                return false;
            if (player->warp_map != warp_map)
                return false;
            if (player->warp_x > 240 || player->warp_x < 0)
                return true;
            if (player->warp_y > 240 || player->warp_y < 0)
                return true;
            if (player->trade_partner_id > 0)
            {
                Player *warp_accept_target = this->players->by_id[player->trade_partner_id];
                if (warp_accept_target)
                    this->Client_SendEncoded(warp_accept_target, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(player->player_id, 2));
            }
            if (player->map_id > 0)
            {
                this->Server_BroadcastNearby(player, AC_DELETE, OP_PLAYER,
                    EO_EncodeNumber(player->player_id, 2) + EO_EncodeNumber(player->warp_effect, 1));
                this->map_control->DecPlayerCount(player->map_id);
            }
            int saved_state = player->warp_effect;
            player->map_id = player->warp_map;
            player->x = player->warp_x;
            player->y = player->warp_y;
            player->idle_ticks = 0;
            player->dead = false;
            // after a warp a player is in the arena's fight only when its launch warped them in
            if (!player->arena_entering)
                player->in_arena = false;
            if (player->arena_entering)
                player->in_arena = true;
            player->arena_entering = false;
            player->trade_partner_id = -1;
            player->guild_inviter_id = -1;
            player->session_token = -1;
            player->guild_recruiter_name = "";
            player->warp_effect = 0;
            player->warp_pending = false;
            player->map_switch_pending = false;
            player->on_chair = 0;
            player->sitting = 0;
            player->map_has_quakes = this->map_control->maps[player->map_id - 1].has_quakes;
            player->map_has_hp_drain = this->map_control->maps[player->map_id - 1].has_hp_drain;
            player->map_has_tp_drain = this->map_control->maps[player->map_id - 1].has_tp_drain;
            player->map_has_spikes = this->map_control->maps[player->map_id - 1].has_spikes;
            this->map_control->IncPlayerCount(player->map_id);
            String warp_packet = (char)this->EO_GetBreakByte(EO_BREAK_BYTE);
            warp_packet.Insert(Player_SerializeAvatar(player, saved_state), warp_packet.Length() + 1);
            warp_packet.Insert(EO_EncodeNumber(1, 1), warp_packet.Length() + 1);
            this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYERS, warp_packet);
            warp_packet = EO_EncodeNumber(2, 1);
            warp_packet.Insert(EO_EncodeNumber(player->map_id, 2), warp_packet.Length() + 1);
            warp_packet.Insert(EO_EncodeNumber(saved_state, 1), warp_packet.Length() + 1);
            warp_packet.Insert(Refresh_BuildReply(player), warp_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_UPDATE, OP_MAP, warp_packet);
            this->Player_FireQuestTriggers(player, QuestCondition_EnterMap, 0);
            return true;
        }
    }
    // .. Paperdoll: Request shows a player's equipment, Add puts on, Remove takes off
    if (family == OP_PAPERDOLL)
    {
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int paperdoll_id = this->EO_DecodeNumber(data.SubString(1, 2));
            if (player->player_id == paperdoll_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PAPERDOLL,
                    Paperdoll_BuildReply(player));
                return true;
            }
            Player *paperdoll_target = this->players->GetById(paperdoll_id);
            if (!paperdoll_target)
                return true;
            this->Client_SendEncoded(player, AC_RESULT, OP_PAPERDOLL,
                Paperdoll_BuildReply(paperdoll_target));
            return true;
        }
        if (action == AC_ADD)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int paperdoll_equip_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            int paperdoll_equip_slot = this->EO_DecodeNumber(data.SubString(3, 1));
            ItemValue *equip_item = GUI->item_values->GetByIndex(paperdoll_equip_item_id - 1);
            // the class, the level and each stat must meet the item's
            if (!GUI->class_values->ClassMatches(player->class_id, equip_item->class_requirement))
            {
                this->Client_SendEncoded(player, AC_SYN1, OP_PAPERDOLL,
                    EO_EncodeNumber(player->class_id, 1));
                return true;
            }
            if (player->level < equip_item->level_requirement)
                return true;
            if (player->adj_strength < equip_item->strength_requirement)
                return true;
            if (player->adj_intelligence < equip_item->intelligence_requirement)
                return true;
            if (player->adj_wisdom < equip_item->wisdom_requirement)
                return true;
            if (player->adj_agility < equip_item->agility_requirement)
                return true;
            if (player->adj_constitution < equip_item->constitution_requirement)
                return true;
            if (player->adj_charisma < equip_item->charisma_requirement)
                return true;
            if (!this->players->Player_EquipItem(player, paperdoll_equip_item_id, paperdoll_equip_slot))
                return true;
            // .. the item's bonuses
            if (equip_item->element < ELEMENT_COUNT)
                player->element_resistances[equip_item->element] += equip_item->element_damage;
            player->min_damage += equip_item->min_damage;
            player->max_damage += equip_item->max_damage;
            player->accuracy += equip_item->accuracy;
            player->evasion += equip_item->evade;
            player->armor += equip_item->armor;
            player->equip_bonus_hp += equip_item->hp;
            player->equip_bonus_tp += equip_item->tp;
            player->equip_strength_bonus += equip_item->strength;
            player->equip_wisdom_bonus += equip_item->wisdom;
            player->equip_intelligence_bonus += equip_item->intelligence;
            player->equip_agility_bonus += equip_item->agility;
            player->equip_constitution_bonus += equip_item->constitution;
            player->equip_charisma_bonus += equip_item->charisma;
            player->CalculateAdjStats();
            this->Player_CalculateStats(player);
            player->CalculateHP_TP_SP();
            String paperdoll_equip_packet = "";
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->player_id, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(1, 1), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(0, 1), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->boots_graphic_id, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->armor_graphic_id, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->hat_graphic_id, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->weapon_graphic_id, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->shield_graphic_id, 2),
                paperdoll_equip_packet.Length() + 1);
            // a change that shows goes to the players near them
            if (player->equip_result > 1)
                this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYER, paperdoll_equip_packet);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(paperdoll_equip_item_id, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->equip_result_count, 3),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(paperdoll_equip_slot, 1),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->max_hp, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->max_tp, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_strength, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_intelligence, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_wisdom, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_agility, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_constitution, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->adj_charisma, 2),
                paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->min_damage, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->max_damage, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->accuracy, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->evasion, 2), paperdoll_equip_packet.Length() + 1);
            paperdoll_equip_packet.Insert(EO_EncodeNumber(player->armor, 2), paperdoll_equip_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_UPDATE, OP_PAPERDOLL, paperdoll_equip_packet);
            return true;
        }
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int paperdoll_unequip_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            int paperdoll_unequip_slot = this->EO_DecodeNumber(data.SubString(3, 1));
            // a cursed item does not come off
            if (GUI->item_values->GetSpecial(paperdoll_unequip_item_id) == ItemSpecial_Cursed)
                return true;
            if (!this->players->Player_UnequipItem(player, paperdoll_unequip_item_id, paperdoll_unequip_slot))
                return true;
            ItemValue *unequip_item = GUI->item_values->GetByIndex(paperdoll_unequip_item_id - 1);
            if (unequip_item->element < ELEMENT_COUNT)
                player->element_resistances[unequip_item->element] -= unequip_item->element_damage;
            player->min_damage -= unequip_item->min_damage;
            player->max_damage -= unequip_item->max_damage;
            player->accuracy -= unequip_item->accuracy;
            player->evasion -= unequip_item->evade;
            player->armor -= unequip_item->armor;
            player->equip_bonus_hp -= unequip_item->hp;
            player->equip_bonus_tp -= unequip_item->tp;
            player->equip_strength_bonus -= unequip_item->strength;
            player->equip_wisdom_bonus -= unequip_item->wisdom;
            player->equip_intelligence_bonus -= unequip_item->intelligence;
            player->equip_agility_bonus -= unequip_item->agility;
            player->equip_constitution_bonus -= unequip_item->constitution;
            player->equip_charisma_bonus -= unequip_item->charisma;
            player->CalculateAdjStats();
            this->Player_CalculateStats(player);
            player->CalculateHP_TP_SP();
            String paperdoll_unequip_packet = "";
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->player_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(1, 1), paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(0, 1), paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->boots_graphic_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->armor_graphic_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->hat_graphic_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->weapon_graphic_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->shield_graphic_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            // a change that shows goes to the players near them
            if (player->equip_result > 1)
                this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYER, paperdoll_unequip_packet);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(paperdoll_unequip_item_id, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(paperdoll_unequip_slot, 1),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->max_hp, 2), paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->max_tp, 2), paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_strength, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_intelligence, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_wisdom, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_agility, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_constitution, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->adj_charisma, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->min_damage, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->max_damage, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->accuracy, 2),
                paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->evasion, 2), paperdoll_unequip_packet.Length() + 1);
            paperdoll_unequip_packet.Insert(EO_EncodeNumber(player->armor, 2), paperdoll_unequip_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_DELETE, OP_PAPERDOLL, paperdoll_unequip_packet);
            return true;
        }
    }
    // .. Book: another player's paperdoll
    if (family == OP_BOOK && action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if (data.Length() < 2)
            return false;
        int book_target_id = this->EO_DecodeNumber(data.SubString(1, 2));
        if (player->player_id == book_target_id)
            return true;
        Player *book_target = this->players->GetById(book_target_id);
        if (!book_target)
            return true;
        this->Client_SendEncoded(player, AC_RESULT, OP_BOOK, Book_BuildReply(book_target));
        return true;
    }
    // .. Chest: Take, Add and Open, from a tile next to the chest
    if (family == OP_CHEST)
    {
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            MapCoord chest_take_coords;
            chest_take_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            chest_take_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            int chest_take_item_id = this->EO_DecodeNumber(data.SubString(3, 2));
            if (!this->Coords_IsAdjacent(chest_take_coords.x, chest_take_coords.y, player->x, player->y))
                return true;
            // the players next to the chest see what is left
            MapCoord stack = this->map_control->TakeChestItem(player->map_id, chest_take_coords, chest_take_item_id);
            if (stack.x < 1)
                return true;
            this->players->Player_AddItem(player, stack.x, stack.y);
            player->weight_current += stack.y * GUI->item_values->GetWeight(stack.x);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int chest_take_weight = player->weight_current;
            int chest_take_weight_max = player->weight_max;
            if (chest_take_weight > WEIGHT_DISPLAY_MAX)
                chest_take_weight = WEIGHT_DISPLAY_MAX;
            if (chest_take_weight_max > WEIGHT_DISPLAY_MAX)
                chest_take_weight_max = WEIGHT_DISPLAY_MAX;
            String take_item_str = this->map_control->BuildChestItemsString(player->map_id, chest_take_coords);
            this->Server_BroadcastAdjacent(player, chest_take_coords, AC_UPDATE, OP_CHEST,
                take_item_str);
            String chest_take_reply = EO_EncodeNumber(stack.x, 2);
            chest_take_reply.Insert(EO_EncodeNumber(stack.y, 3), chest_take_reply.Length() + 1);
            chest_take_reply.Insert(EO_EncodeNumber(chest_take_weight, 1), chest_take_reply.Length() + 1);
            chest_take_reply.Insert(EO_EncodeNumber(chest_take_weight_max, 1), chest_take_reply.Length() + 1);
            chest_take_reply.Insert(take_item_str, chest_take_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_PICKUP, OP_CHEST, chest_take_reply);
            return true;
        }
        if (action == AC_ADD)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 7)
                return false;
            MapCoord chest_add_coords;
            chest_add_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            chest_add_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            int chest_item_id = this->EO_DecodeNumber(data.SubString(3, 2));
            unsigned int chest_amount = this->EO_DecodeNumber(data.SubString(5, 3));
            if (GUI->item_values->GetSpecial(chest_item_id) == ItemSpecial_Lore)
                return true;
            if (chest_amount > 10000000)
                return true;
            int slot_count = this->map_control->GetChestSlotCount(player->map_id, chest_add_coords);
            // a chest holds 5 kinds of item
            if (slot_count < 0 || slot_count > 4)
            {
                this->Client_SendEncoded(player, AC_HIT, OP_CHEST, "0");
                return true;
            }
            if (!this->Coords_IsAdjacent(chest_add_coords.x, chest_add_coords.y, player->x, player->y))
                return true;
            if (!this->players->Player_RemoveItem(player, chest_item_id, chest_amount))
            {
                this->Client_SendEncoded(player, AC_UPDATE, OP_ITEM,
                    EO_EncodeNumber(chest_item_id, 2));
                return true;
            }
            player->weight_current -= player->item_change_count * GUI->item_values->GetWeight(chest_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int chest_add_weight = player->weight_current;
            int chest_add_weight_max = player->weight_max;
            if (chest_add_weight > WEIGHT_DISPLAY_MAX)
                chest_add_weight = WEIGHT_DISPLAY_MAX;
            if (chest_add_weight_max > WEIGHT_DISPLAY_MAX)
                chest_add_weight_max = WEIGHT_DISPLAY_MAX;
            this->map_control->AddChestItem(player->map_id, chest_add_coords, chest_item_id, player->item_change_count);
            String add_item_str = this->map_control->BuildChestItemsString(player->map_id, chest_add_coords);
            this->Server_BroadcastAdjacent(player, chest_add_coords, AC_UPDATE, OP_CHEST,
                add_item_str);
            String chest_add_reply = EO_EncodeNumber(chest_item_id, 2);
            chest_add_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), chest_add_reply.Length() + 1);
            chest_add_reply.Insert(EO_EncodeNumber(chest_add_weight, 1), chest_add_reply.Length() + 1);
            chest_add_reply.Insert(EO_EncodeNumber(chest_add_weight_max, 1), chest_add_reply.Length() + 1);
            chest_add_reply.Insert(add_item_str, chest_add_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_CHEST, chest_add_reply);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            MapCoord chest_open_coords;
            chest_open_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            chest_open_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            if (!this->Coords_IsAdjacent(chest_open_coords.x, chest_open_coords.y, player->x, player->y))
                return true;
            String chest_open_reply = this->map_control->BuildChestItemsString(player->map_id, chest_open_coords);
            if (chest_open_reply == "N")
            {
                this->Client_SendEncoded(player, AC_CLOSE, OP_CHEST, "N");
                return true;
            }
            int chest_key = this->map_control->GetChestKeyAt(player->map_id, chest_open_coords);
            // a chest with no key opens for anyone
            if (chest_key < 1)
            {
                chest_open_reply.Insert(data.SubString(1, 2), 1);
                this->Client_SendEncoded(player, AC_OPEN, OP_CHEST, chest_open_reply);
                return true;
            }
            if (this->players->Player_HasKeyItem(player, chest_key))
            {
                chest_open_reply.Insert(data.SubString(1, 2), 1);
                this->Client_SendEncoded(player, AC_OPEN, OP_CHEST, chest_open_reply);
                return true;
            }
            this->Client_SendEncoded(player, AC_CLOSE, OP_CHEST, EO_EncodeNumber(chest_key, 2));
            return true;
        }
    }
    // .. Shop: Create crafts, Buy, Sell and Open, with the shop the player opened last
    if (family == OP_SHOP)
    {
        if (action == AC_NEW)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int craft_id = this->EO_DecodeNumber(data.SubString(1, 2));
            int craft_shop_id = this->EO_DecodeNumber(data.SubString(3, 4));
            if (player->session_token != craft_shop_id)
                return true;
            MapCoord ingredient1 = GUI->shop_values->GetCraftIngredient1(craft_shop_id, craft_id);
            MapCoord ingredient2 = GUI->shop_values->GetCraftIngredient2(craft_shop_id, craft_id);
            MapCoord ingredient3 = GUI->shop_values->GetCraftIngredient3(craft_shop_id, craft_id);
            MapCoord ingredient4 = GUI->shop_values->GetCraftIngredient4(craft_shop_id, craft_id);
            if (ingredient1.item_id < 1 && ingredient2.item_id < 1 && ingredient3.item_id < 1 &&
                ingredient4.item_id < 1)
                return true;
            // .. the player must carry every ingredient; all of them are taken for one of the item
            char can_craft = 1;
            char any_ingredient = 0;
            if (ingredient1.item_id > 0 && ingredient1.amount > 0)
            {
                any_ingredient = 1;
                if (this->players->GetItemAmount(player, ingredient1.item_id) < ingredient1.amount)
                    can_craft = 0;
            }
            if (ingredient2.item_id > 0 && ingredient2.amount > 0)
            {
                any_ingredient = 1;
                if (this->players->GetItemAmount(player, ingredient2.item_id) < ingredient2.amount)
                    can_craft = 0;
            }
            if (ingredient3.item_id > 0 && ingredient3.amount > 0)
            {
                any_ingredient = 1;
                if (this->players->GetItemAmount(player, ingredient3.item_id) < ingredient3.amount)
                    can_craft = 0;
            }
            if (ingredient4.item_id > 0 && ingredient4.amount > 0)
            {
                any_ingredient = 1;
                if (this->players->GetItemAmount(player, ingredient4.item_id) < ingredient4.amount)
                    can_craft = 0;
            }
            if (!can_craft || !any_ingredient)
                return true;
            String shop_craft_reply = EO_EncodeNumber(craft_id, 2);
            if (ingredient1.item_id > 0 && ingredient1.amount > 0 &&
                this->players->Player_RemoveItem(player, ingredient1.item_id, ingredient1.amount))
            {
                shop_craft_reply.Insert(EO_EncodeNumber(ingredient1.item_id, 2), shop_craft_reply.Length() + 1);
                shop_craft_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    shop_craft_reply.Length() + 1);
            }
            if (ingredient2.item_id > 0 && ingredient2.amount > 0 &&
                this->players->Player_RemoveItem(player, ingredient2.item_id, ingredient2.amount))
            {
                shop_craft_reply.Insert(EO_EncodeNumber(ingredient2.item_id, 2), shop_craft_reply.Length() + 1);
                shop_craft_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    shop_craft_reply.Length() + 1);
            }
            if (ingredient3.item_id > 0 && ingredient3.amount > 0 &&
                this->players->Player_RemoveItem(player, ingredient3.item_id, ingredient3.amount))
            {
                shop_craft_reply.Insert(EO_EncodeNumber(ingredient3.item_id, 2), shop_craft_reply.Length() + 1);
                shop_craft_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    shop_craft_reply.Length() + 1);
            }
            if (ingredient4.item_id > 0 && ingredient4.amount > 0 &&
                this->players->Player_RemoveItem(player, ingredient4.item_id, ingredient4.amount))
            {
                shop_craft_reply.Insert(EO_EncodeNumber(ingredient4.item_id, 2), shop_craft_reply.Length() + 1);
                shop_craft_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4),
                    shop_craft_reply.Length() + 1);
            }
            this->players->Player_AddItem(player, craft_id, 1);
            player->weight_current += GUI->item_values->GetWeight(craft_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int shop_craft_weight = player->weight_current;
            int shop_craft_weight_max = player->weight_max;
            if (shop_craft_weight > WEIGHT_DISPLAY_MAX)
                shop_craft_weight = WEIGHT_DISPLAY_MAX;
            if (shop_craft_weight_max > WEIGHT_DISPLAY_MAX)
                shop_craft_weight_max = WEIGHT_DISPLAY_MAX;
            shop_craft_reply.Insert(EO_EncodeNumber(shop_craft_weight_max, 1), 3);
            shop_craft_reply.Insert(EO_EncodeNumber(shop_craft_weight, 1), 3);
            this->Client_SendEncoded(player, AC_NEW, OP_SHOP, shop_craft_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_BUY)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            int shop_buy_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            unsigned int shop_buy_amount = this->EO_DecodeNumber(data.SubString(3, 4));
            int buy_shop_id = this->EO_DecodeNumber(data.SubString(7, 4));
            // more than 100 at once: a 20 minute ban
            if (shop_buy_amount > 100)
            {
                this->banned->AddBan(player->remote_ip, player->hdid, (char)0, 1200);
                return false;
            }
            if (player->session_token != buy_shop_id)
                return true;
            int shop_buy_price = GUI->shop_values->GetBuyPrice(buy_shop_id, shop_buy_item_id, shop_buy_amount);
            if (shop_buy_price < 0)
                return true;
            if (shop_buy_amount < 1)
                return true;
            // item 1 is gold
            if (!this->players->Player_RemoveItem(player, 1, shop_buy_price))
                return true;
            this->players->Player_AddItem(player, shop_buy_item_id, shop_buy_amount);
            player->weight_current += shop_buy_amount * GUI->item_values->GetWeight(shop_buy_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int shop_buy_weight = player->weight_current;
            int shop_buy_weight_max = player->weight_max;
            if (shop_buy_weight > WEIGHT_DISPLAY_MAX)
                shop_buy_weight = WEIGHT_DISPLAY_MAX;
            if (shop_buy_weight_max > WEIGHT_DISPLAY_MAX)
                shop_buy_weight_max = WEIGHT_DISPLAY_MAX;
            String shop_buy_reply = EO_EncodeNumber(player->item_change_remaining, 4);
            shop_buy_reply.Insert(EO_EncodeNumber(shop_buy_item_id, 2), shop_buy_reply.Length() + 1);
            shop_buy_reply.Insert(EO_EncodeNumber(shop_buy_amount, 4), shop_buy_reply.Length() + 1);
            shop_buy_reply.Insert(EO_EncodeNumber(shop_buy_weight, 1), shop_buy_reply.Length() + 1);
            shop_buy_reply.Insert(EO_EncodeNumber(shop_buy_weight_max, 1), shop_buy_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_BUY, OP_SHOP, shop_buy_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_SELL)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            int shop_sell_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            unsigned int shop_sell_amount = this->EO_DecodeNumber(data.SubString(3, 4));
            int sell_shop_id = this->EO_DecodeNumber(data.SubString(7, 4));
            // more than 100 at once: a 20 minute ban
            if (shop_sell_amount > 100)
            {
                this->banned->AddBan(player->remote_ip, player->hdid, (char)0, 1200);
                return false;
            }
            if (player->session_token != sell_shop_id)
                return true;
            if (!this->players->Player_RemoveItem(player, shop_sell_item_id, shop_sell_amount))
                return true;
            int shop_sell_price = GUI->shop_values->GetSellPrice(sell_shop_id, shop_sell_item_id,
                player->item_change_count);
            if (shop_sell_price < 0)
                return true;
            this->players->Player_AddItem(player, 1, shop_sell_price);
            player->weight_current -= player->item_change_count * GUI->item_values->GetWeight(shop_sell_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int shop_sell_weight = player->weight_current;
            int shop_sell_weight_max = player->weight_max;
            if (shop_sell_weight > WEIGHT_DISPLAY_MAX)
                shop_sell_weight = WEIGHT_DISPLAY_MAX;
            if (shop_sell_weight_max > WEIGHT_DISPLAY_MAX)
                shop_sell_weight_max = WEIGHT_DISPLAY_MAX;
            int shop_gold = this->players->GetItemAmount(player, 1);
            String shop_sell_reply = EO_EncodeNumber(player->item_change_remaining, 4);
            shop_sell_reply.Insert(EO_EncodeNumber(shop_sell_item_id, 2), shop_sell_reply.Length() + 1);
            shop_sell_reply.Insert(EO_EncodeNumber(shop_gold, 4), shop_sell_reply.Length() + 1);
            shop_sell_reply.Insert(EO_EncodeNumber(shop_sell_weight, 1), shop_sell_reply.Length() + 1);
            shop_sell_reply.Insert(EO_EncodeNumber(shop_sell_weight_max, 1), shop_sell_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_SELL, OP_SHOP, shop_sell_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int shop_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord shop_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, shop_npc_index);
            if (shop_coords.x < 0 || shop_coords.y < 0)
                return true;
            if (!this->Server_InViewRange(shop_coords.x, shop_coords.y, player->x, player->y))
                return true;
            int shop_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, shop_npc_index);
            MapCoord shop_type_info = GUI->npc_values->GetType(shop_npc_id);
            // an npc in view that is a shop
            if (shop_type_info.type != NpcType_Shop)
                return true;
            player->session_token = shop_type_info.behavior_id;
            this->Client_SendEncoded(player, AC_OPEN, OP_SHOP,
                GUI->shop_values->BuildOpenData(shop_type_info.behavior_id));
            return true;
        }
    }
    // .. Locker: Take, Add, Open and Buy, from a tile next to a bank vault
    if (family == OP_LOCKER)
    {
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            MapCoord locker_take_coords;
            locker_take_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            locker_take_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            int locker_take_item_id = this->EO_DecodeNumber(data.SubString(3, 2));
            if (!this->Coords_IsAdjacent(locker_take_coords.x, locker_take_coords.y, player->x, player->y))
                return true;
            if (this->map_control->GetTileSpecValueAt(player->map_id, locker_take_coords.x, locker_take_coords.y) !=
                TileSpec_BankVault)
                return true;
            if (!this->players->Player_RemoveBankItem(player, locker_take_item_id))
                return true;
            this->players->Player_AddItem(player, locker_take_item_id, player->item_change_count);
            player->weight_current += player->item_change_count * GUI->item_values->GetWeight(locker_take_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int take_weight_current = player->weight_current;
            int take_weight_max = player->weight_max;
            if (take_weight_current > WEIGHT_DISPLAY_MAX)
                take_weight_current = WEIGHT_DISPLAY_MAX;
            if (take_weight_max > WEIGHT_DISPLAY_MAX)
                take_weight_max = WEIGHT_DISPLAY_MAX;
            String locker_take_reply = EO_EncodeNumber(locker_take_item_id, 2);
            locker_take_reply.Insert(EO_EncodeNumber(player->item_change_count, 3), locker_take_reply.Length() + 1);
            locker_take_reply.Insert(EO_EncodeNumber(take_weight_current, 1), locker_take_reply.Length() + 1);
            locker_take_reply.Insert(EO_EncodeNumber(take_weight_max, 1), locker_take_reply.Length() + 1);
            vector<PlayerInventory>::iterator locker_take_item;
            for (locker_take_item = player->bank.begin(); locker_take_item != player->bank.end(); locker_take_item++)
            {
                locker_take_reply.Insert(EO_EncodeNumber(locker_take_item->item_id, 2), locker_take_reply.Length() + 1);
                locker_take_reply.Insert(EO_EncodeNumber(locker_take_item->amount, 3), locker_take_reply.Length() + 1);
            }
            this->Client_SendEncoded(player, AC_PICKUP, OP_LOCKER, locker_take_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_ADD)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 7)
                return false;
            MapCoord locker_add_coords;
            locker_add_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            locker_add_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            int locker_add_item_id = this->EO_DecodeNumber(data.SubString(3, 2));
            unsigned int locker_amount = this->EO_DecodeNumber(data.SubString(5, 3));
            // no gold in the locker
            if (locker_amount > 200 || locker_add_item_id < 2)
                return true;
            if (player->bank.size() > 60)
            {
                this->Client_SendEncoded(player, AC_HIT, OP_LOCKER,
                    EO_EncodeNumber(player->locker_bank, 1));
                return true;
            }
            // 25 kinds of item, and 5 more for each upgrade
            if (player->bank.size() > (unsigned int)(5 * player->locker_bank + 24) &&
                !this->players->Player_HasBankItem(player, locker_add_item_id))
            {
                this->Client_SendEncoded(player, AC_HIT, OP_LOCKER,
                    EO_EncodeNumber(player->locker_bank, 1));
                return true;
            }
            if (!this->Coords_IsAdjacent(locker_add_coords.x, locker_add_coords.y, player->x, player->y))
                return true;
            if (this->map_control->GetTileSpecValueAt(player->map_id, locker_add_coords.x, locker_add_coords.y) !=
                TileSpec_BankVault)
                return true;
            if (!this->players->Player_RemoveItem(player, locker_add_item_id, locker_amount))
            {
                this->Client_SendEncoded(player, AC_UPDATE, OP_ITEM,
                    EO_EncodeNumber(locker_add_item_id, 2));
                return true;
            }
            player->weight_current -= locker_amount * GUI->item_values->GetWeight(locker_add_item_id);
            if (player->weight_current < 0)
                player->weight_current = 0;
            int add_weight_current = player->weight_current;
            int add_weight_max = player->weight_max;
            if (add_weight_current > WEIGHT_DISPLAY_MAX)
                add_weight_current = WEIGHT_DISPLAY_MAX;
            if (add_weight_max > WEIGHT_DISPLAY_MAX)
                add_weight_max = WEIGHT_DISPLAY_MAX;
            this->players->Player_AddBankItem(player, locker_add_item_id, player->item_change_count);
            String locker_add_reply = EO_EncodeNumber(locker_add_item_id, 2);
            locker_add_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), locker_add_reply.Length() + 1);
            locker_add_reply.Insert(EO_EncodeNumber(add_weight_current, 1), locker_add_reply.Length() + 1);
            locker_add_reply.Insert(EO_EncodeNumber(add_weight_max, 1), locker_add_reply.Length() + 1);
            vector<PlayerInventory>::iterator locker_add_item;
            for (locker_add_item = player->bank.begin(); locker_add_item != player->bank.end(); locker_add_item++)
            {
                locker_add_reply.Insert(EO_EncodeNumber(locker_add_item->item_id, 2), locker_add_reply.Length() + 1);
                locker_add_reply.Insert(EO_EncodeNumber(locker_add_item->amount, 3), locker_add_reply.Length() + 1);
            }
            this->Client_SendEncoded(player, AC_RESULT, OP_LOCKER, locker_add_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            MapCoord locker_open_coords;
            locker_open_coords.x = this->EO_DecodeNumber(data.SubString(1, 1));
            locker_open_coords.y = this->EO_DecodeNumber(data.SubString(2, 1));
            if (!this->Coords_IsAdjacent(locker_open_coords.x, locker_open_coords.y, player->x, player->y))
                return true;
            if (this->map_control->GetTileSpecValueAt(player->map_id, locker_open_coords.x, locker_open_coords.y) !=
                TileSpec_BankVault)
                return true;
            String locker_open_reply = data.SubString(1, 2);
            for (vector<PlayerInventory>::iterator locker_open_item = player->bank.begin();
                locker_open_item != player->bank.end(); locker_open_item++)
            {
                locker_open_reply.Insert(EO_EncodeNumber(locker_open_item->item_id, 2), locker_open_reply.Length() + 1);
                locker_open_reply.Insert(EO_EncodeNumber(locker_open_item->amount, 3), locker_open_reply.Length() + 1);
            }
            this->Client_SendEncoded(player, AC_OPEN, OP_LOCKER, locker_open_reply);
            return true;
        }
        if (action == AC_BUY)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 1)
                return false;
            if (player->locker_bank > 6)
                return false;
            // 7 upgrades, each 1000 gold more than the last
            int locker_cost = 1000 * player->locker_bank + 1000;
            if (!this->players->Player_RemoveItem(player, 1, locker_cost))
                return true;
            player->locker_bank++;
            String locker_upgrade_reply = EO_EncodeNumber(player->item_change_remaining, 4);
            locker_upgrade_reply.Insert(EO_EncodeNumber(player->locker_bank, 1), locker_upgrade_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_BUY, OP_LOCKER, locker_upgrade_reply);
            return true;
        }
    }
    // .. Board: Remove deletes a post by its id, Create posts, Take reads a post, Open lists a board
    if (family == OP_MSGBOARD)
    {
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int delete_board = this->EO_DecodeNumber(data.SubString(1, 2));
            int delete_post_id = this->EO_DecodeNumber(data.SubString(3, 2));
            GUI->msgboard_control->DeletePost(delete_board, delete_post_id);
            return true;
        }
        if (action == AC_NEW)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int post_board = this->EO_DecodeNumber(data.SubString(1, 2));
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String subject = PacketReader_GetBreakString();
            String board_message = PacketReader_GetBreakString();
            if (subject.Length() < 1 || board_message.Length() < 1)
                return true;
            // a subject of 32 characters, a message of 1200
            if (subject.Length() > 32)
                subject = subject.SubString(1, 32);
            if (board_message.Length() > 1200)
                board_message = board_message.SubString(1, 1200);
            // two posts a board for each player
            if (GUI->msgboard_control->CountPosts(post_board, player->name) > 1)
                return true;
            GUI->msgboard_control->AddPost(post_board, player->name, subject, board_message, 0);
            return true;
        }
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int read_board = this->EO_DecodeNumber(data.SubString(1, 2));
            int read_post_id = this->EO_DecodeNumber(data.SubString(3, 2));
            String post = GUI->msgboard_control->GetPost(read_board, read_post_id);
            if (post.Length() < 1)
                return true;
            this->Client_SendEncoded(player, AC_SET, OP_MSGBOARD, post);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int open_board = this->EO_DecodeNumber(data.SubString(1, 2));
            String board_data = GUI->msgboard_control->GetBoard(open_board + 1);
            if (board_data.Length() < 1)
                return true;
            this->Client_SendEncoded(player, AC_OPEN, OP_MSGBOARD, board_data);
            return true;
        }
    }
    // .. Barber: Open at a barber in view, Buy a new hair style and color
    if (family == OP_BARBER)
    {
        if (action == AC_BUY)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int style = this->EO_DecodeNumber(data.SubString(1, 1));
            int color = this->EO_DecodeNumber(data.SubString(2, 1));
            int barber_session = this->EO_DecodeNumber(data.SubString(3, 4));
            if (style < 0 || style > 20)
                return false;
            if (color < 0 || color > 9)
                return false;
            if (barber_session < 200000 || barber_session > 300000)
                return false;
            if (player->session_token != barber_session)
                return false;
            // 200 gold a level
            int barber_price = 200;
            if (player->level > 0)
                barber_price = 200 * player->level;
            if (!this->players->Player_RemoveItem(player, 1, barber_price))
                return true;
            player->hair_style = style;
            player->hair_color = color;
            String barber_packet = EO_EncodeNumber(player->player_id, 2);
            barber_packet.Insert(EO_EncodeNumber(2, 1), barber_packet.Length() + 1);
            barber_packet.Insert(EO_EncodeNumber(0, 1), barber_packet.Length() + 1);
            barber_packet.Insert(EO_EncodeNumber(style, 1), barber_packet.Length() + 1);
            barber_packet.Insert(EO_EncodeNumber(color, 1), barber_packet.Length() + 1);
            this->Server_BroadcastNearby(player, AC_UPDATE, OP_PLAYER, barber_packet);
            barber_packet.Insert(EO_EncodeNumber(player->item_change_remaining, 4), 1);
            this->Client_SendEncoded(player, AC_UPDATE, OP_BARBER, barber_packet);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int barber_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord barber_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, barber_npc_index);
            if (barber_coords.x < 0 || barber_coords.y < 0)
                return false;
            if (!this->Server_InViewRange(barber_coords.x, barber_coords.y, player->x, player->y))
                return true;
            int barber_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, barber_npc_index);
            MapCoord type_info = GUI->npc_values->GetType(barber_npc_id);
            // npc type 10, the barber
            if (type_info.x != 10)
                return true;
            player->session_token = random(10000) + 200001;
            this->Client_SendEncoded(player, AC_OPEN, OP_BARBER,
                EO_EncodeNumber(player->session_token, 4));
            return true;
        }
    }
    // .. Citizen: an inn's sleep, giving up the citizenship, the three answers, and Open
    if (family == OP_INN)
    {
        if (action == AC_REQUEST || action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int inn_sleep_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            int sleep_inn_index = this->EO_DecodeNumber(data.SubString(3, 2));
            if (player->session_id != inn_sleep_npc_index)
                return true;
            if (player->session_token != sleep_inn_index)
                return false;
            if (player->hp >= player->max_hp)
                return true;
            // a night costs the hp and tp they are missing
            int inn_cost = player->max_hp - player->hp + (player->max_tp - player->tp);
            if (action == AC_REQUEST)
            {
                this->Client_SendEncoded(player, AC_REQUEST, OP_INN,
                    EO_EncodeNumber(inn_cost, 4));
                return true;
            }
            else
            {
                int sleep_map = GUI->inn_values->GetSleepMap(sleep_inn_index);
                if (sleep_map < 1)
                    return true;
                if (!this->players->Player_RemoveItem(player, 1, inn_cost))
                    return true;
                MapCoord sleep_pos;
                sleep_pos.x = GUI->inn_values->GetSleepX(sleep_inn_index);
                sleep_pos.y = GUI->inn_values->GetSleepY(sleep_inn_index);
                this->Client_SendEncoded(player, AC_CONFIRM, OP_INN,
                    EO_EncodeNumber(player->item_change_remaining, 4));
                player->hp = player->max_hp;
                player->tp = player->max_tp;
                if (player->in_party)
                {
                    String inn_msg = EO_EncodeNumber(player->player_id, 2);
                    inn_msg.Insert(EO_EncodeNumber(player->HpPercent(), 1), inn_msg.Length() + 1);
                    this->Server_BroadcastToParty(player, AC_UPDATE, OP_GROUP, inn_msg);
                }
                this->Player_Warp(player, sleep_map, sleep_pos, WarpEffect_None, 0);
                return true;
            }
        }
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int inn_id = this->EO_DecodeNumber(data.SubString(1, 2));
            if (player->session_token != inn_id)
                return false;
            if (player->home_id != inn_id)
            {
                this->Client_SendEncoded(player, AC_DELETE, OP_INN, EO_EncodeNumber(0, 1));
                return true;
            }
            if (!player->home_id)
            {
                this->Client_SendEncoded(player, AC_DELETE, OP_INN, EO_EncodeNumber(0, 1));
                return true;
            }
            player->home_id = 0;
            player->home_name = GUI->inn_values->GetName(0);
            this->Client_SendEncoded(player, AC_DELETE, OP_INN, EO_EncodeNumber(1, 1));
            return true;
        }
        if (action == AC_RESULT)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            int inn_answer_npc_index = this->EO_DecodeNumber(PacketReader_GetBreakString());
            int answer_inn_index = this->EO_DecodeNumber(PacketReader_GetBreakString());
            String answer1 = PacketReader_GetBreakString();
            String answer2 = PacketReader_GetBreakString();
            String answer3 = PacketReader_GetBreakString();
            if (player->session_id != inn_answer_npc_index)
                return true;
            if (player->session_token != answer_inn_index)
                return false;
            int inn_count = 0;
            if (LowerCase(answer1) != LowerCase(GUI->inn_values->GetAnswer(answer_inn_index, 0)))
                inn_count++;
            if (LowerCase(answer2) != LowerCase(GUI->inn_values->GetAnswer(answer_inn_index, 1)))
                inn_count++;
            if (LowerCase(answer3) != LowerCase(GUI->inn_values->GetAnswer(answer_inn_index, 2)))
                inn_count++;
            // all three answers right: they become a citizen of the inn
            if (!inn_count)
            {
                player->home_id = answer_inn_index;
                player->home_name = GUI->inn_values->GetName(answer_inn_index);
            }
            this->Client_SendEncoded(player, AC_RESULT, OP_INN, EO_EncodeNumber(inn_count, 1));
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int inn_open_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord inn_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, inn_open_npc_index);
            if (inn_coords.x < 0 || inn_coords.y < 0)
                return false;
            if (!this->Server_InViewRange(inn_coords.x, inn_coords.y, player->x, player->y))
                return true;
            int inn_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, inn_open_npc_index);
            MapCoord inn_type_info = GUI->npc_values->GetType(inn_npc_id);
            if (inn_type_info.type != NpcType_Inn)
                return true;
            player->session_token = inn_type_info.behavior_id - 1;
            String inn_reply = EO_EncodeNumber(player->session_token, 3);
            inn_reply.Insert(EO_EncodeNumber(player->home_id, 1), inn_reply.Length() + 1);
            inn_reply.Insert(EO_EncodeNumber(player->session_id, 2), inn_reply.Length() + 1);
            inn_reply.Insert(GUI->inn_values->GetQuestion(inn_type_info.behavior_id - 1), inn_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_OPEN, OP_INN, inn_reply);
            return true;
        }
    }
    // .. Bank: Add deposits gold, Take withdraws it, Open at a banker in view
    if (family == OP_BANK)
    {
        if (action == AC_ADD)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 7)
                return false;
            unsigned int bank_deposit_amount = this->EO_DecodeNumber(data.SubString(1, 4));
            int deposit_session = this->EO_DecodeNumber(data.SubString(5, 3));
            if (deposit_session < 100000 || deposit_session > 200000)
                return false;
            if (player->session_token != deposit_session)
                return false;
            if (player->money_bank > 100000000000)
                return true;
            if (!this->players->Player_RemoveItem(player, 1, bank_deposit_amount))
                return true;
            player->money_bank += bank_deposit_amount;
            String bank_deposit_reply = EO_EncodeNumber(player->item_change_remaining, 4);
            bank_deposit_reply.Insert(EO_EncodeNumber(player->money_bank, 4), bank_deposit_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_BANK, bank_deposit_reply);
            return true;
        }
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 7)
                return false;
            unsigned int bank_withdraw_amount = this->EO_DecodeNumber(data.SubString(1, 4));
            int withdraw_session = this->EO_DecodeNumber(data.SubString(5, 3));
            if (player->session_token != withdraw_session)
                return true;
            if ((unsigned int)player->money_bank < bank_withdraw_amount)
                return true;
            player->money_bank -= bank_withdraw_amount;
            this->players->Player_AddItem(player, 1, bank_withdraw_amount);
            String bank_withdraw_reply = EO_EncodeNumber(player->item_change_amount, 4);
            bank_withdraw_reply.Insert(EO_EncodeNumber(player->money_bank, 4), bank_withdraw_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_BANK, bank_withdraw_reply);
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int bank_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord bank_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, bank_npc_index);
            if (bank_coords.x < 0 || bank_coords.y < 0)
                return false;
            if (!this->Server_InViewRange(bank_coords.x, bank_coords.y, player->x, player->y))
                return true;
            int bank_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, bank_npc_index);
            MapCoord type_info = GUI->npc_values->GetType(bank_npc_id);
            // npc type 9, the banker
            if (type_info.x != 9)
                return true;
            player->session_token = random(10000) + 100001;
            String bank_open_reply = EO_EncodeNumber(player->money_bank, 4);
            bank_open_reply.Insert(EO_EncodeNumber(player->session_token, 3), bank_open_reply.Length() + 1);
            bank_open_reply.Insert(EO_EncodeNumber(player->locker_bank, 1), bank_open_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_OPEN, OP_BANK, bank_open_reply);
            return true;
        }
    }
    // .. Jukebox: Open lists the recent tracks, Msg plays one for 25 gold, Use plays an instrument
    if (family == OP_MUSIC)
    {
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            String tracks = GUI->jukebox_control->BuildOpenData(player->map_id);
            if (tracks.Length() > 0)
                this->Client_SendEncoded(player, AC_OPEN, OP_MUSIC, tracks);
            return true;
        }
        if (action == AC_PLAY)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 4)
                return false;
            int requested_track = this->EO_DecodeNumber(data.SubString(3, 2)) + 1;
            if (requested_track < 1)
                return false;
            if (GUI->jukebox_control->TryPlayTrack(player->map_id, player->name))
            {
                if (!this->players->Player_RemoveItem(player, 1, 25))
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_MUSIC, EO_EncodeNumber(1, 2));
                    return true;
                }
                this->Client_SendEncoded(player, AC_UPDATE, OP_MUSIC,
                    EO_EncodeNumber(player->item_change_remaining, 4));
                this->Server_BroadcastToMap(player->map_id, AC_DO, OP_MUSIC,
                    EO_EncodeNumber(requested_track, 2));
                return true;
            }
            return true;
        }
        if (action == AC_DO)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int instrument_id = this->EO_DecodeNumber(String(data[1]));
            int note = this->EO_DecodeNumber(String(data[2]));
            // the two instruments, held as the weapon; notes 1 to 36
            if (instrument_id != 49 && instrument_id != 50)
                return false;
            if (note < 1 || note > 36)
                return false;
            if (player->weapon_graphic_id != instrument_id)
                return false;
            String jukebox_packet = EO_EncodeNumber(player->player_id, 2);
            jukebox_packet.Insert(EO_EncodeNumber(player->direction, 1), jukebox_packet.Length() + 1);
            jukebox_packet.Insert(EO_EncodeNumber(instrument_id, 1), jukebox_packet.Length() + 1);
            jukebox_packet.Insert(EO_EncodeNumber(note, 1), jukebox_packet.Length() + 1);
            this->Server_BroadcastNearby(player, AC_PLAY, OP_MUSIC, jukebox_packet);
            return true;
        }
    }
    // .. Trade: Request asks a player, Accept opens it, Add and Remove change an offer, Agree accepts, Close ends it
    if (family == OP_TRADE)
    {
        if (action == AC_ADD)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int trade_add_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            unsigned int trade_amount = this->EO_DecodeNumber(data.SubString(3, 4));
            unsigned int add_have = this->players->GetItemAmount(player, trade_add_item_id);
            if (GUI->item_values->GetSpecial(trade_add_item_id) == ItemSpecial_Lore)
                return true;
            if (trade_amount > add_have || trade_amount < 1)
                return true;
            if (player->trade_partner_id < 1)
                return false;
            // offering more than they have: a 200 second ban
            if ((unsigned int)this->players->GetItemAmount(player, trade_add_item_id) < trade_amount)
            {
                this->banned->AddBan(player->remote_ip, player->hdid, (char)0, 200);
                return false;
            }
            Player *trade_add_target = this->players->by_id[player->trade_partner_id];
            if (!trade_add_target)
                return false;
            if (player->map_id != trade_add_target->map_id)
                return true;
            if (trade_add_target->trade_partner_id != player->player_id)
            {
                player->trade_accepted = false;
                return false;
            }
            if (!this->players->Player_AddTradeItem(player, trade_add_item_id, trade_amount))
                return true;
            player->trade_accepted = false;
            // any change undoes both accepts
            player->trade_value = 100;
            trade_add_target->trade_accepted = false;
            trade_add_target->trade_value = 100;
            String trade_add_packet = EO_EncodeNumber(player->player_id, 2);
            for (vector<PlayerInventory>::iterator add_offer = player->trade_items.begin();
                add_offer != player->trade_items.end(); add_offer++)
            {
                trade_add_packet.Insert(EO_EncodeNumber(add_offer->item_id, 2), trade_add_packet.Length() + 1);
                trade_add_packet.Insert(EO_EncodeNumber(add_offer->amount, 4), trade_add_packet.Length() + 1);
            }
            trade_add_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_add_packet.Length() + 1);
            trade_add_packet.Insert(EO_EncodeNumber(trade_add_target->player_id, 2), trade_add_packet.Length() + 1);
            vector<PlayerInventory>::iterator add_target_offer;
            for (add_target_offer = trade_add_target->trade_items.begin();
                add_target_offer != trade_add_target->trade_items.end(); add_target_offer++)
            {
                trade_add_packet.Insert(EO_EncodeNumber(add_target_offer->item_id, 2), trade_add_packet.Length() + 1);
                trade_add_packet.Insert(EO_EncodeNumber(add_target_offer->amount, 4), trade_add_packet.Length() + 1);
            }
            trade_add_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_add_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_TRADE, trade_add_packet);
            this->Client_SendEncoded(trade_add_target, AC_RESULT, OP_TRADE, trade_add_packet);
            return true;
        }
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            Player *trade_remove_target = this->players->by_id[player->trade_partner_id];
            if (!trade_remove_target)
                return false;
            if (player->map_id != trade_remove_target->map_id)
                return false;
            if (trade_remove_target->trade_partner_id != player->player_id)
            {
                player->trade_accepted = false;
                return false;
            }
            int trade_remove_item_id = this->EO_DecodeNumber(data.SubString(1, 2));
            if (!this->players->Player_RemoveTradeItem(player, trade_remove_item_id))
                return true;
            player->trade_accepted = false;
            player->trade_value = 100;
            trade_remove_target->trade_accepted = false;
            trade_remove_target->trade_value = 100;
            String trade_remove_packet = EO_EncodeNumber(player->player_id, 2);
            for (vector<PlayerInventory>::iterator remove_offer = player->trade_items.begin();
                remove_offer != player->trade_items.end(); remove_offer++)
            {
                trade_remove_packet.Insert(EO_EncodeNumber(remove_offer->item_id, 2), trade_remove_packet.Length() + 1);
                trade_remove_packet.Insert(EO_EncodeNumber(remove_offer->amount, 4), trade_remove_packet.Length() + 1);
            }
            trade_remove_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_remove_packet.Length() + 1);
            trade_remove_packet.Insert(EO_EncodeNumber(trade_remove_target->player_id, 2),
                trade_remove_packet.Length() + 1);
            vector<PlayerInventory>::iterator remove_target_offer;
            for (remove_target_offer = trade_remove_target->trade_items.begin();
                remove_target_offer != trade_remove_target->trade_items.end(); remove_target_offer++)
            {
                trade_remove_packet.Insert(EO_EncodeNumber(remove_target_offer->item_id, 2),
                    trade_remove_packet.Length() + 1);
                trade_remove_packet.Insert(EO_EncodeNumber(remove_target_offer->amount, 4),
                    trade_remove_packet.Length() + 1);
            }
            trade_remove_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_remove_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_RESULT, OP_TRADE, trade_remove_packet);
            this->Client_SendEncoded(trade_remove_target, AC_RESULT, OP_TRADE, trade_remove_packet);
            return true;
        }
        if (action == AC_UPDATE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 1)
                return false;
            int trade_arg = this->EO_DecodeNumber(String(data[1]));
            Player *trade_agree_target = this->players->by_id[player->trade_partner_id];
            if (!trade_agree_target)
                return false;
            if (player->map_id != trade_agree_target->map_id)
                return false;
            if (trade_agree_target->trade_partner_id != player->player_id)
            {
                player->trade_accepted = false;
                return false;
            }
            // .. one side accepts, or takes it back; the other is told
            if (!trade_agree_target->trade_accepted || trade_arg != 1)
            {
                if (!trade_arg)
                {
                    player->trade_accepted = false;
                    player->trade_value = 100;
                }
                if (trade_arg == 1)
                {
                    player->trade_accepted = true;
                    player->trade_value = trade_agree_target->trade_items.size();
                }
                String trade_pending_packet = EO_EncodeNumber(player->player_id, 2);
                trade_pending_packet.Insert(EO_EncodeNumber(trade_arg, 1), trade_pending_packet.Length() + 1);
                this->Client_SendEncoded(trade_agree_target, AC_UPDATE, OP_TRADE,
                    trade_pending_packet);
                this->Client_SendEncoded(player, AC_HIT, OP_TRADE, EO_EncodeNumber(trade_arg, 1));
                return true;
            }
            // the offer changed since the other side accepted: both start over
            if (player->trade_items.size() != (unsigned int)trade_agree_target->trade_value)
            {
                player->trade_accepted = false;
                player->trade_value = 100;
                trade_agree_target->trade_accepted = false;
                trade_agree_target->trade_value = 100;
                String trade_changed_packet = EO_EncodeNumber(player->player_id, 2);
                for (vector<PlayerInventory>::iterator agree_offer = player->trade_items.begin();
                    agree_offer != player->trade_items.end(); agree_offer++)
                {
                    trade_changed_packet.Insert(EO_EncodeNumber(agree_offer->item_id, 2),
                        trade_changed_packet.Length() + 1);
                    trade_changed_packet.Insert(EO_EncodeNumber(agree_offer->amount, 4),
                        trade_changed_packet.Length() + 1);
                }
                trade_changed_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE),
                    trade_changed_packet.Length() + 1);
                trade_changed_packet.Insert(EO_EncodeNumber(trade_agree_target->player_id, 2),
                    trade_changed_packet.Length() + 1);
                vector<PlayerInventory>::iterator agree_target_offer;
                for (agree_target_offer = trade_agree_target->trade_items.begin();
                    agree_target_offer != trade_agree_target->trade_items.end(); agree_target_offer++)
                {
                    trade_changed_packet.Insert(EO_EncodeNumber(agree_target_offer->item_id, 2),
                        trade_changed_packet.Length() + 1);
                    trade_changed_packet.Insert(EO_EncodeNumber(agree_target_offer->amount, 4),
                        trade_changed_packet.Length() + 1);
                }
                trade_changed_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE),
                    trade_changed_packet.Length() + 1);
                this->Client_SendEncoded(player, AC_CAST, OP_TRADE, trade_changed_packet);
                this->Client_SendEncoded(trade_agree_target, AC_CAST, OP_TRADE,
                    trade_changed_packet);
                return true;
            }
            // .. both accepted: each must still have what they offer, and offer something
            vector<PlayerInventory>::iterator given_item;
            for (given_item = player->trade_items.begin(); given_item != player->trade_items.end(); given_item++)
            {
                int item_id = given_item->item_id;
                int have;
                have = this->players->GetItemAmount(player, item_id);
                if (have < 0)
                    have = 0;
                if (given_item->amount > (unsigned int)have || given_item->amount < 1)
                {
                    player->trade_items.clear();
                    player->trade_accepted = false;
                    trade_agree_target->trade_accepted = false;
                    this->Client_SendEncoded(trade_agree_target, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(player->player_id, 2));
                    this->Client_SendEncoded(player, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(trade_agree_target->player_id, 2));
                    return true;
                }
            }
            vector<PlayerInventory>::iterator received_item;
            for (received_item = trade_agree_target->trade_items.begin();
                received_item != trade_agree_target->trade_items.end(); received_item++)
            {
                int item_id = received_item->item_id;
                int have;
                have = this->players->GetItemAmount(trade_agree_target, item_id);
                if (have < 0)
                    have = 0;
                if (received_item->amount > (unsigned int)have || received_item->amount < 1)
                {
                    trade_agree_target->trade_items.clear();
                    trade_agree_target->trade_accepted = false;
                    player->trade_accepted = false;
                    this->Client_SendEncoded(trade_agree_target, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(player->player_id, 2));
                    this->Client_SendEncoded(player, AC_CLOSE, OP_TRADE,
                        EO_EncodeNumber(trade_agree_target->player_id, 2));
                    return true;
                }
            }
            if (player->trade_items.size() < 1)
            {
                this->Client_SendEncoded(trade_agree_target, AC_CLOSE, OP_TRADE,
                    EO_EncodeNumber(player->player_id, 2));
                this->Client_SendEncoded(player, AC_CLOSE, OP_TRADE,
                    EO_EncodeNumber(trade_agree_target->player_id, 2));
                return true;
            }
            if (trade_agree_target->trade_items.size() < 1)
            {
                this->Client_SendEncoded(trade_agree_target, AC_CLOSE, OP_TRADE,
                    EO_EncodeNumber(player->player_id, 2));
                this->Client_SendEncoded(player, AC_CLOSE, OP_TRADE,
                    EO_EncodeNumber(trade_agree_target->player_id, 2));
                return true;
            }
            // .. the items change hands
            String trade_result = EO_EncodeNumber(player->player_id, 2);
            for (given_item = player->trade_items.begin(); given_item != player->trade_items.end(); given_item++)
            {
                this->players->Player_RemoveItem(player, given_item->item_id, given_item->amount);
                this->players->Player_AddItem(trade_agree_target, given_item->item_id, given_item->amount);
                trade_result.Insert(EO_EncodeNumber(given_item->item_id, 2), trade_result.Length() + 1);
                trade_result.Insert(EO_EncodeNumber(given_item->amount, 4), trade_result.Length() + 1);
                player->weight_current -= given_item->amount * GUI->item_values->GetWeight(given_item->item_id);
                trade_agree_target->weight_current += given_item->amount *
                    GUI->item_values->GetWeight(given_item->item_id);
                if (player->weight_current < 0)
                    player->weight_current = 0;
            }
            trade_result.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_result.Length() + 1);
            trade_result.Insert(EO_EncodeNumber(trade_agree_target->player_id, 2), trade_result.Length() + 1);
            for (received_item = trade_agree_target->trade_items.begin();
                received_item != trade_agree_target->trade_items.end(); received_item++)
            {
                this->players->Player_RemoveItem(trade_agree_target, received_item->item_id, received_item->amount);
                this->players->Player_AddItem(player, received_item->item_id, received_item->amount);
                trade_result.Insert(EO_EncodeNumber(received_item->item_id, 2), trade_result.Length() + 1);
                trade_result.Insert(EO_EncodeNumber(received_item->amount, 4), trade_result.Length() + 1);
                trade_agree_target->weight_current -= received_item->amount *
                    GUI->item_values->GetWeight(received_item->item_id);
                player->weight_current += received_item->amount * GUI->item_values->GetWeight(received_item->item_id);
                if (player->weight_current < 0)
                    player->weight_current = 0;
            }
            trade_result.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_result.Length() + 1);
            player->trade_items.clear();
            player->trade_partner_id = -1;
            player->trade_accepted = false;
            player->trade_value = 100;
            trade_agree_target->trade_items.clear();
            trade_agree_target->trade_partner_id = -1;
            trade_agree_target->trade_accepted = false;
            trade_agree_target->trade_value = 100;
            this->Client_SendEncoded(player, AC_DO, OP_TRADE, trade_result);
            this->Client_SendEncoded(trade_agree_target, AC_DO, OP_TRADE, trade_result);
            String emote_reply = EO_EncodeNumber(player->player_id, 2);
            emote_reply.Insert(EO_EncodeNumber(12, 1), emote_reply.Length() + 1);
            this->Server_BroadcastNearby(player, AC_SET, OP_EMOTE, emote_reply);
            emote_reply = EO_EncodeNumber(trade_agree_target->player_id, 2);
            emote_reply.Insert(EO_EncodeNumber(12, 1), emote_reply.Length() + 1);
            this->Server_BroadcastNearby(trade_agree_target, AC_SET, OP_EMOTE, emote_reply);
            this->Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
            this->Player_FireQuestTriggers(trade_agree_target, QuestCondition_AnyEvent, 0);
            return true;
        }
        if (action == AC_CLOSE)
        {
            if (!player->logged_in)
                return false;
            Player *trade_close_target = this->players->by_id[player->trade_partner_id];
            if (player->trade_partner_id < 1)
                return true;
            if (!trade_close_target)
                return true;
            player->trade_items.clear();
            trade_close_target->trade_items.clear();
            player->trade_partner_id = -1;
            trade_close_target->trade_partner_id = -1;
            player->trade_accepted = false;
            player->trade_value = 100;
            this->Client_SendEncoded(trade_close_target, AC_CLOSE, OP_TRADE,
                EO_EncodeNumber(player->player_id, 2));
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int trade_request_type = this->EO_DecodeNumber(String(data[1]));
            int trade_request_target_id = this->EO_DecodeNumber(data.SubString(2, 2));
            // a trade request carries 138
            if (trade_request_type != 138)
                return true;
            Player *trade_request_target = this->players->by_id[trade_request_target_id];
            if (!trade_request_target)
                return true;
            if (this->settings->GetJailMap() == player->map_id)
                return false;
            if (player->map_id != trade_request_target->map_id)
                return true;
            if (trade_request_target->trade_accepted)
            {
                // asking again a player who already accepted: a 4 hour ban
                if (trade_request_target->trade_partner_id == player->player_id)
                {
                    this->banned->AddBan(player->remote_ip, player->hdid, false, 14400);
                    return false;
                }
                return true;
            }
            player->trade_partner_id = trade_request_target_id;
            player->trade_accepted = false;
            player->trade_value = 100;
            player->trade_items.clear();
            String trade_request_packet = EO_EncodeNumber(trade_request_type, 1);
            trade_request_packet.Insert(EO_EncodeNumber(player->player_id, 2), trade_request_packet.Length() + 1);
            trade_request_packet.Insert(player->name, trade_request_packet.Length() + 1);
            this->Client_SendEncoded(trade_request_target, AC_REQUEST, OP_TRADE,
                trade_request_packet);
            return true;
        }
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int trade_accept_type = this->EO_DecodeNumber(String(data[1]));
            int trade_accept_target_id = this->EO_DecodeNumber(data.SubString(2, 2));
            Player *trade_accept_target = this->players->by_id[trade_accept_target_id];
            if (!trade_accept_target)
                return true;
            if (player->map_id != trade_accept_target->map_id)
                return true;
            if (trade_accept_type)
                return true;
            if (trade_accept_target->trade_partner_id != player->player_id)
                return true;
            player->trade_partner_id = trade_accept_target->player_id;
            player->trade_accepted = false;
            trade_accept_target->trade_accepted = false;
            player->trade_value = 100;
            player->trade_items.clear();
            trade_accept_target->trade_items.clear();
            String trade_accept_packet = EO_EncodeNumber(player->player_id, 2);
            trade_accept_packet.Insert(player->name, trade_accept_packet.Length() + 1);
            trade_accept_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_accept_packet.Length() + 1);
            trade_accept_packet.Insert(EO_EncodeNumber(trade_accept_target->player_id, 2),
                trade_accept_packet.Length() + 1);
            trade_accept_packet.Insert(trade_accept_target->name, trade_accept_packet.Length() + 1);
            trade_accept_packet.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), trade_accept_packet.Length() + 1);
            this->Client_SendEncoded(player, AC_OPEN, OP_TRADE, trade_accept_packet);
            this->Client_SendEncoded(trade_accept_target, AC_OPEN, OP_TRADE, trade_accept_packet);
            return true;
        }
    }
    // .. Party: Take refreshes the list, Request asks or invites, Remove takes a member out, Accept joins
    if (family == OP_GROUP)
    {
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 1)
                return false;
            if (!player->in_party)
            {
                this->Client_SendEncoded(player, AC_CLOSE, OP_GROUP,
                    String(this->EO_GetBreakByte(EO_BREAK_BYTE)));
                return true;
            }
            unsigned int party_count = player->CountPartyMembers();
            // the list only when the client's count is wrong
            if (this->EO_DecodeNumber(String(data[1])) != party_count)
                this->Client_SendEncoded(player, AC_LIST, OP_GROUP, Party_EncodeMemberList(player));
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int party_request_type = this->EO_DecodeNumber(String(data[1]));
            int party_request_id = this->EO_DecodeNumber(data.SubString(2, 2));
            Player *party_request_target = this->players->GetById(party_request_id);
            if (!party_request_target)
                return true;
            // .. 0: ask to join their party
            if (!party_request_type)
            {
                if (player->in_party && party_request_target->in_party &&
                    player->party_leader_id == party_request_target->party_leader_id)
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_GROUP,
                        EO_EncodeNumber(PartyReplyCode_AlreadyInYourParty, 1) + party_request_target->name);
                    return true;
                }
                if (party_request_target->in_party &&
                    party_request_target->CountPartyMembers() + 1 >= this->settings->GetGroupMax())
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_GROUP,
                        EO_EncodeNumber(PartyReplyCode_PartyIsFull, 1));
                    return true;
                }
                player->party_requested_id = party_request_target->player_id;
                String msg = EO_EncodeNumber(party_request_type, 1);
                msg.Insert(EO_EncodeNumber(player->player_id, 2), msg.Length() + 1);
                msg.Insert(player->name, msg.Length() + 1);
                this->Client_SendEncoded(party_request_target, AC_REQUEST, OP_GROUP, msg);
                return true;
            }
            // .. 1: invite them to yours
            if (party_request_type == 1)
            {
                if (party_request_target->in_party)
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_GROUP,
                        EO_EncodeNumber(PartyReplyCode_AlreadyInAnotherParty, 1) + party_request_target->name);
                    return true;
                }
                if (player->in_party && player->CountPartyMembers() + 1 >= this->settings->GetGroupMax())
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_GROUP,
                        EO_EncodeNumber(PartyReplyCode_PartyIsFull, 1));
                    return true;
                }
                player->party_requested_id = party_request_target->player_id;
                String msg = EO_EncodeNumber(party_request_type, 1);
                msg.Insert(EO_EncodeNumber(player->player_id, 2), msg.Length() + 1);
                msg.Insert(player->name, msg.Length() + 1);
                this->Client_SendEncoded(party_request_target, AC_REQUEST, OP_GROUP, msg);
                return true;
            }
            return false;
        }
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (!player->in_party)
            {
                this->Client_SendEncoded(player, AC_CLOSE, OP_GROUP,
                    String(this->EO_GetBreakByte(EO_BREAK_BYTE)));
                return true;
            }
            if (data.Length() < 2)
                return false;
            Player *party_remove_target = this->players->GetById(this->EO_DecodeNumber(data.SubString(1, 2)));
            if (!party_remove_target)
                return false;
            if (!party_remove_target->in_party)
                return false;
            // only from the same party
            if (player->party_leader_id != party_remove_target->party_leader_id)
                return false;
            this->Server_BroadcastToParty(player, AC_DELETE, OP_GROUP,
                EO_EncodeNumber(party_remove_target->player_id, 2));
            this->players->Player_LeaveParty(party_remove_target);
            return true;
        }
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 3)
                return false;
            int party_accept_type = this->EO_DecodeNumber(String(data[1]));
            int party_accept_id = this->EO_DecodeNumber(data.SubString(2, 2));
            Player *party_accept_target = this->players->GetById(party_accept_id);
            if (!party_accept_target)
                return true;
            if (party_accept_target->party_requested_id != player->player_id)
                return true;
            // .. 0: they join your party, leaving their own; a new party when you have none
            if (!party_accept_type)
            {
                if (party_accept_target->in_party)
                {
                    this->Server_BroadcastToPartyExceptSelf(party_accept_target, AC_DELETE,
                        OP_GROUP, EO_EncodeNumber(party_accept_target->player_id, 2));
                    this->players->Player_LeaveParty(party_accept_target);
                }
                if (player->in_party)
                {
                    String msg = EO_EncodeNumber(party_accept_target->player_id, 2);
                    msg.Insert(EO_EncodeNumber(0, 1), msg.Length() + 1);
                    msg.Insert(EO_EncodeNumber(party_accept_target->level, 1), msg.Length() + 1);
                    msg.Insert(EO_EncodeNumber(party_accept_target->HpPercent(), 1), msg.Length() + 1);
                    msg.Insert(party_accept_target->name, msg.Length() + 1);
                    this->Server_BroadcastToParty(player, AC_ADD, OP_GROUP, msg);
                    this->players->Party_AddNewMember(player, party_accept_target);
                    this->Client_SendEncoded(party_accept_target, AC_NEW, OP_GROUP,
                        Party_EncodeMemberList(party_accept_target));
                    return true;
                }
                else
                {
                    player->AddPartyMember(player->player_id);
                    player->AddPartyMember(party_accept_target->player_id);
                    party_accept_target->AddPartyMember(player->player_id);
                    party_accept_target->AddPartyMember(party_accept_target->player_id);
                    player->party_leader_id = player->player_id;
                    party_accept_target->party_leader_id = player->player_id;
                    player->in_party = true;
                    party_accept_target->in_party = true;
                    this->Server_BroadcastToParty(player, AC_NEW, OP_GROUP,
                        Party_EncodeMemberList(player));
                    return true;
                }
            }
            // .. 1: you join their party, leaving your own; a new party when they have none
            if (party_accept_type == 1)
            {
                if (player->in_party)
                {
                    this->Server_BroadcastToPartyExceptSelf(player, AC_DELETE, OP_GROUP,
                        EO_EncodeNumber(player->player_id, 2));
                    this->players->Player_LeaveParty(player);
                }
                if (party_accept_target->in_party)
                {
                    String msg = EO_EncodeNumber(player->player_id, 2);
                    msg.Insert(EO_EncodeNumber(0, 1), msg.Length() + 1);
                    msg.Insert(EO_EncodeNumber(player->level, 1), msg.Length() + 1);
                    msg.Insert(EO_EncodeNumber(player->HpPercent(), 1), msg.Length() + 1);
                    msg.Insert(player->name, msg.Length() + 1);
                    this->Server_BroadcastToParty(party_accept_target, AC_ADD, OP_GROUP, msg);
                    this->players->Party_AddNewMember(party_accept_target, player);
                    this->Client_SendEncoded(player, AC_NEW, OP_GROUP,
                        Party_EncodeMemberList(player));
                    return true;
                }
                else
                {
                    player->AddPartyMember(party_accept_target->player_id);
                    player->AddPartyMember(player->player_id);
                    party_accept_target->AddPartyMember(party_accept_target->player_id);
                    party_accept_target->AddPartyMember(player->player_id);
                    player->party_leader_id = party_accept_target->player_id;
                    party_accept_target->party_leader_id = party_accept_target->player_id;
                    player->in_party = true;
                    party_accept_target->in_party = true;
                    this->Server_BroadcastToParty(party_accept_target, AC_NEW, OP_GROUP,
                        Party_EncodeMemberList(party_accept_target));
                    return true;
                }
            }
            return false;
        }
    }
    // .. Guild: everything at a guild master, with the session it handed out; one request a server each 5 seconds
    if (family == OP_GUILD)
    {
        // .. Guild Buy: gold into the guild's bank
        if (action == AC_BUY)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 8)
                return false;
            int deposit_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            int guild_gold = this->EO_DecodeNumber(data.SubString(5, 4));
            if (player->session_token != deposit_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            if (!this->players->Player_RemoveItem(player, 1, guild_gold))
                return true;
            this->mysql_controls->Mysql_SubmitExec(0,
                "UPDATE endl_guilds SET money = money + " + IntToStr(player->item_change_count) + " WHERE tag = '" +
                player->guild_tag + "'");
            this->Client_SendEncoded(player, AC_BUY, OP_GUILD,
                EO_EncodeNumber(player->item_change_remaining, 4));
            return true;
        }
        if (action == AC_RANK)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 9)
                return false;
            int rank_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            int rank = this->EO_DecodeNumber(data.SubString(5, 1));
            String rank_member_name = data.SubString(6, data.Length() - 5);
            if (rank < 1 || rank > 9)
                return true;
            if (player->session_token != rank_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            // rank 1 is the leader
            if (player->guild_rank_id != 1)
                return true;
            String rank_field = "rank" + IntToStr(rank);
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildRankName, player->player_id, player->query_id,
                data, "SELECT " + rank_field + " FROM endl_guilds WHERE tag = '" + player->guild_tag + "' LIMIT 1");
            return true;
        }
        // .. Guild Kick: the leader removes a member; an offline one goes through the database
        if (action == AC_REMOVE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 7)
                return false;
            int kick_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            String kick_member_name = data.SubString(5, data.Length() - 4);
            if (player->session_token != kick_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            if (player->guild_rank_id != 1)
                return true;
            Player *member = this->players->FindByName(kick_member_name);
            if (!member)
            {
                this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildKickMember, player->player_id,
                    player->query_id, data,
                    "SELECT ident_guild, ident_rank FROM endl_characters WHERE name = '" + kick_member_name +
                    "' AND ident_guild = '" + player->guild_tag + "' LIMIT 1");
                return true;
            }
            if (member->guild_tag != player->guild_tag)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RemoveNotMember, 2));
                return true;
            }
            if (member->guild_rank_id == 1)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RemoveLeader, 2));
                return true;
            }
            member->guild_rank_id = GUILD_RANK_NEW_MEMBER;
            member->guild_tag = "0";
            member->guild_name = "";
            member->guild_rank_name = "";
            this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_Removed, 2));
            return true;
        }
        if (action == AC_JUNK)
            return true;
        // .. Guild Agree: the leader changes the description or the 9 rank names
        if (action == AC_UPDATE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 7)
                return false;
            int update_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            int update_info_type = this->EO_DecodeNumber(data.SubString(5, 2));
            String rest = data.SubString(7, data.Length() - 6);
            if (player->session_token != update_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            if (player->guild_rank_id != 1)
                return true;
            if (update_info_type == GuildInfoType_Description)
            {
                this->mysql_controls->Mysql_SubmitExec(0,
                    "UPDATE endl_guilds SET description = '" + this->mysql_controls->Mysql_SanitizeString(rest, 0) +
                    "' WHERE tag = '" + player->guild_tag + "'");
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Updated, 2));
                return true;
            }
            if (update_info_type == GuildInfoType_Ranks)
            {
                this->PacketReader_Init(rest, this->EO_GetBreakByte(EO_BREAK_BYTE));
                String out = "UPDATE endl_guilds SET ";
                out.Insert("rank1 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank2 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank3 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank4 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank5 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank6 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank7 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank8 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "',", out.Length() + 1);
                out.Insert("rank9 = '" + this->mysql_controls->Mysql_SanitizeString(PacketReader_GetBreakString(), 0) +
                    "' ", out.Length() + 1);
                out.Insert("WHERE tag = '" + player->guild_tag + "'", out.Length() + 1);
                this->mysql_controls->Mysql_SubmitExec(0, out);
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RanksUpdated, 2));
                return true;
            }
        }
        // .. Guild Take: the description, the ranks or the bank
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int view_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            int view_info_type = this->EO_DecodeNumber(data.SubString(5, 2));
            if (player->session_token != view_session_id)
                return true;
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            if (view_info_type == GuildInfoType_Description)
            {
                if (player->guild_rank_id != 1)
                    return true;
                this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildDescription, player->player_id,
                    player->query_id, data,
                    "SELECT description FROM endl_guilds WHERE tag = '" + player->guild_tag + "' LIMIT 1");
                return true;
            }
            if (view_info_type == GuildInfoType_Ranks)
            {
                this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildRanks, player->player_id, player->query_id,
                    data,
                    "SELECT rank1,rank2,rank3,rank4,rank5,rank6,rank7,rank8,rank9 FROM endl_guilds WHERE tag = '" +
                    player->guild_tag + "' LIMIT 1");
                return true;
            }
            if (view_info_type == GuildInfoType_Bank)
            {
                this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildBank, player->player_id, player->query_id,
                    data, "SELECT money FROM endl_guilds WHERE tag = '" + player->guild_tag + "' LIMIT 1");
                return true;
            }
        }
        // .. Guild Tell: a guild's member list, by its tag
        if (action == AC_PRIVATE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int members_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != members_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            String info_guild = this->mysql_controls->Db_SanitizeString(data.SubString(5, data.Length() - 4));
            if (info_guild.Length() < 2)
                return true;
            if (info_guild.Length() > 3)
                return true;
            String info_query = "SELECT ident_rank, name, rank FROM endl_characters WHERE ident_guild = '" +
                info_guild + "' ORDER BY ident_rank, name LIMIT 100";
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildMemberList, player->player_id, player->query_id,
                data, info_query);
            return true;
        }
        // .. Guild Report: a guild's info, by its tag
        if (action == AC_PUBLIC)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int leave_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != leave_session_id)
                return true;
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            String info_guild = this->mysql_controls->Db_SanitizeString(data.SubString(5, data.Length() - 4));
            if (info_guild.Length() < 2)
                return true;
            if (info_guild.Length() > 3)
                return true;
            String info_query = "SELECT * FROM endl_guilds WHERE tag = '" + info_guild + "' LIMIT 1";
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildInfo, player->player_id, player->query_id, data,
                info_query);
            return true;
        }
        // .. Guild Remove: they leave their guild
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int join_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != join_session_id)
                return true;
            player->guild_tag = "";
            player->guild_name = "";
            player->guild_rank_id = GUILD_RANK_NEW_MEMBER;
            player->guild_rank_name = "";
            player->guild_inviter_id = -1;
            return true;
        }
        // .. Guild Player: asks to join, through a recruiter on the map
        if (action == AC_SET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            int join_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != join_session_id)
                return true;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String join_guild = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            String recruiter = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            if (player->guild_tag.Length() > 1)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_AlreadyMember, 2));
                return true;
            }
            Player *guild_join_target = this->players->FindByName(recruiter);
            if (!guild_join_target)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RecruiterOffline, 2));
                return true;
            }
            if (guild_join_target->map_id != player->map_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RecruiterNotHere, 2));
                return true;
            }
            if (guild_join_target->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RecruiterWrongGuild, 2));
                return true;
            }
            if (LowerCase(guild_join_target->guild_tag) != LowerCase(join_guild))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_RecruiterWrongGuild, 2));
                return true;
            }
            // ranks 1 and 2 recruit
            if (guild_join_target->guild_rank_id > 2)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NotRecruiter, 2));
                return true;
            }
            player->guild_recruiter_name = guild_join_target->name;
            String guild_msg = EO_EncodeNumber(GuildReply_JoinRequest, 2);
            guild_msg.Insert(EO_EncodeNumber(player->player_id, 2), guild_msg.Length() + 1);
            guild_msg.Insert(player->name, guild_msg.Length() + 1);
            this->Client_SendEncoded(guild_join_target, AC_RESULT, OP_GUILD, guild_msg);
            return true;
        }
        // .. Guild Create: makes the guild once enough players agreed to be in it
        if (action == AC_NEW)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 13)
                return false;
            int create_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != create_session_id)
                return true;
            if (create_session_id < 300000 || create_session_id > 400000)
                return true;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String tag_upper = UpperCase(this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString()));
            String guild_create_name = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            String description = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            guild_create_name = this->mysql_controls->Mysql_SanitizeString(guild_create_name, 0);
            tag_upper = this->mysql_controls->Mysql_SanitizeString(tag_upper, 1);
            if (tag_upper.Length() < 2 || tag_upper.Length() > 3)
                return true;
            if (guild_create_name.Length() < 4 || guild_create_name.Length() > 24)
                return true;
            if (player->guild_inviter_id != player->player_id)
                return true;
            player->guild_inviter_id = -1;
            if (player->guild_tag.Length() > 1)
                return true;
            // tags no guild can have
            if (tag_upper == "GM" || tag_upper == "HGM" || tag_upper == "GOD" || tag_upper == "ADM" ||
                tag_upper == "SUK" || tag_upper == "SUX" || tag_upper == "ASS" || tag_upper == "FUK" ||
                tag_upper == "BRA" || tag_upper == "FUC" || tag_upper == "SEX" || tag_upper == "CUM" ||
                tag_upper == "HOE" || tag_upper == "TIT" || tag_upper == "HO" || tag_upper == "FU" ||
                tag_upper == "KKK" || tag_upper == "XXX")
                return true;
            if (!this->PassesWordFilter(guild_create_name))
                return true;
            String create_tag_first = tag_upper[1];
            String create_name_first = guild_create_name[1];
            // the tag starts with the name's first letter
            if (LowerCase(create_tag_first) != LowerCase(create_name_first))
                return true;
            if (tag_upper[1] == ' ' || tag_upper[2] == ' ')
                return true;
            if (!this->mysql_controls->IsAlphabeticText(tag_upper))
            {
                this->banned->AddBan(player->remote_ip, player->hdid, false, 14400);
                return false;
            }
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildRequestCheck, player->player_id, player->query_id,
                data,
                "SELECT name FROM endl_guilds WHERE name = '" + guild_create_name + "' OR tag = '" + tag_upper + "'");
            return true;
        }
        // .. Guild Use: a recruiter takes in the player who asked them
        if (action == AC_DO)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 2)
                return false;
            if (player->guild_tag.Length() < GUILD_TAG_MIN_LENGTH)
                return true;
            if (player->guild_rank_id > 2)
                return true;
            Player *guild_recruit_target = this->players->GetById(this->EO_DecodeNumber(data.SubString(1, 2)));
            if (!guild_recruit_target)
                return true;
            if (guild_recruit_target->guild_tag.Length() > 1)
                return true;
            if (guild_recruit_target->map_id != player->map_id)
                return true;
            if (player->name != guild_recruit_target->guild_recruiter_name)
                return true;
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildJoin, player->player_id, player->query_id, data,
                "SELECT * FROM endl_guilds WHERE tag = '" + player->guild_tag + "' LIMIT 1");
            return true;
        }
        // .. Guild Accept: a player agrees to be in a guild that is being made
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 6)
                return false;
            int accept_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            int inviter_id = this->EO_DecodeNumber(data.SubString(5, 2));
            if (accept_session_id != 20202)
                return false;
            if (player->guild_tag.Length() > 1)
                return true;
            Player *inviter = this->players->GetById(inviter_id);
            if (!inviter)
                return true;
            if (inviter->player_id == player->player_id)
                return true;
            if (inviter->map_id != player->map_id)
                return true;
            int invites = this->players->CountGuildInvites(inviter);
            if (invites < 10)
            {
                player->guild_inviter_id = inviter_id;
                // the tenth tells the founder the guild can be made
                if (invites == 9)
                {
                    this->Client_SendEncoded(inviter, AC_RESULT, OP_GUILD,
                        EO_EncodeNumber(GuildReply_CreateAddConfirm, 2) + player->name);
                    return true;
                }
                this->Client_SendEncoded(inviter, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_CreateAdd, 2) + player->name);
            }
            return true;
        }
        // .. Guild Request: starts a new guild; 10 players with no guild must be on the map
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 13)
                return false;
            int request_session_id = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != request_session_id)
                return true;
            if (request_session_id < 300000 || request_session_id > 400000)
                return true;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String tag = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            String guild_request_name = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            if (tag.Length() < GUILD_TAG_MIN_LENGTH || tag.Length() > 3)
                return true;
            if (guild_request_name.Length() < 4 || guild_request_name.Length() > 24)
                return true;
            if (player->guild_tag.Length() > 1)
                return true;
            if (tag == "GM" || tag == "HGM" || tag == "GOD" || tag == "ADM" || tag == "SUK" || tag == "SUX" ||
                tag == "ASS" || tag == "FUK" || tag == "BRA" || tag == "FUC" || tag == "SEX" || tag == "CUM" ||
                tag == "HOE" || tag == "TIT" || tag == "HO" || tag == "FU" || tag == "KKK" || tag == "XXX")
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NotApproved, 2));
                return true;
            }
            if (!this->PassesWordFilter(guild_request_name))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NotApproved, 2));
                return true;
            }
            String request_tag_first = tag[1];
            String request_name_first = guild_request_name[1];
            if (LowerCase(request_tag_first) != LowerCase(request_name_first))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NotApproved, 2));
                return true;
            }
            if (tag[1] == ' ' || tag[2] == ' ')
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NotApproved, 2));
                return true;
            }
            if (this->players->CountGuildlessOnMap(player) < 10)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_NoCandidates, 2));
                return true;
            }
            if (!this->Server_TickOncePerFiveSeconds())
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                    EO_EncodeNumber(GuildReply_Busy, 2));
                return true;
            }
            this->mysql_controls->Mysql_SubmitQuery(DbQueryType_GuildCreateCheck, player->player_id, player->query_id,
                data,
                "SELECT name FROM endl_guilds WHERE name = '" + guild_request_name + "' OR tag = '" + tag +
                "' LIMIT 1");
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int guild_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord guild_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, guild_npc_index);
            if (guild_coords.x < 0 || guild_coords.y < 0)
                return true;
            if (!this->Server_InViewRange(guild_coords.x, guild_coords.y, player->x, player->y))
                return true;
            int guild_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, guild_npc_index);
            MapCoord type_info = GUI->npc_values->GetType(guild_npc_id);
            // npc type 11, the guild master
            if (type_info.x != 11)
                return true;
            player->session_token = random(10000) + 300001;
            this->Client_SendEncoded(player, AC_OPEN, OP_GUILD,
                EO_EncodeNumber(player->session_token, 3));
            return true;
        }
    }
    // .. Quest: List shows the quests, Accept answers a dialog, Use talks to a quest npc
    if (family == OP_QUEST)
    {
        if (action == AC_LIST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() != 1)
                return false;
            int quest_list_type = this->EO_DecodeNumber(data);
            // .. 1: the quests in progress, each with its step and its count so far
            if (quest_list_type == 1)
            {
                String active_reply = EO_EncodeNumber(QuestPage_Progress, 1);
                active_reply.Insert(EO_EncodeNumber((int)player->quest_trackers.size(), 2), active_reply.Length() + 1);
                vector<PlayerQuest>::iterator list_active_quest;
                for (list_active_quest = player->quest_trackers.begin();
                    list_active_quest != player->quest_trackers.end(); list_active_quest++)
                {
                    QuestState *
                        state = this->quest_engine->GetState(list_active_quest->quest_id,
                        list_active_quest->state_index);
                    if (state)
                    {
                        active_reply.Insert(this->quest_engine->GetQuestName(list_active_quest->quest_id),
                            active_reply.Length() + 1);
                        active_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), active_reply.Length() + 1);
                        active_reply.Insert(state->description, active_reply.Length() + 1);
                        active_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), active_reply.Length() + 1);
                        int cond = state->progress_condition_type;
                        int goal = 0;
                        int progress = 0;
                        if (state->progress_rule_index < 5 && state->progress_condition_type > 0 &&
                            state->progress_condition_type < 10)
                        {
                            progress = list_active_quest->counters[state->progress_rule_index];
                            goal = state->rules[state->progress_rule_index]->args[1];
                            if (state->progress_condition_type == 9)
                                goal = state->rules[state->progress_rule_index]->args[0];
                            if (goal < 1)
                                goal = 1;
                        }
                        active_reply.Insert(EO_EncodeNumber(cond, 2), active_reply.Length() + 1);
                        active_reply.Insert(EO_EncodeNumber(progress, 2), active_reply.Length() + 1);
                        active_reply.Insert(EO_EncodeNumber(goal, 2), active_reply.Length() + 1);
                        active_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), active_reply.Length() + 1);
                    }
                }
                this->Client_SendEncoded(player, AC_LIST, OP_QUEST, active_reply);
                return true;
            }
            // .. 2: the quests done
            if (quest_list_type == 2)
            {
                String finished_reply = EO_EncodeNumber(QuestPage_History, 1);
                finished_reply.Insert(EO_EncodeNumber((int)player->quest_trackers.size(), 2),
                    finished_reply.Length() + 1);
                vector<PlayerQuest>::iterator list_finished_quest;
                for (list_finished_quest = player->quest_history.begin();
                    list_finished_quest != player->quest_history.end(); list_finished_quest++)
                {
                    finished_reply.Insert(this->quest_engine->GetQuestName(list_finished_quest->quest_id),
                        finished_reply.Length() + 1);
                    finished_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), finished_reply.Length() + 1);
                }
                this->Client_SendEncoded(player, AC_LIST, OP_QUEST, finished_reply);
                return true;
            }
            return true;
        }
        // .. Quest Accept: a dialog answer moves the quest on to the state its rule names
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            int dialog_session_id = this->EO_DecodeNumber(data.SubString(1, 2));
            int dialog_token = this->EO_DecodeNumber(data.SubString(3, 2));
            int quest_id = this->EO_DecodeNumber(data.SubString(5, 2));
            int accept_behavior_id = this->EO_DecodeNumber(data.SubString(7, 2));
            int quest_dialog_type = this->EO_DecodeNumber(data.SubString(9, 1));
            int quest_arg = this->EO_DecodeNumber(data.SubString(10, 1));
            int result = -1;
            if (player->session_id != dialog_session_id)
                return false;
            if (player->session_token != dialog_token)
                return false;
            if (player->dialog_behavior_id != accept_behavior_id)
                return false;
            player->session_token = random(10000);
            for (vector<PlayerQuest>::iterator dialog_active_quest = player->quest_trackers.begin();
                dialog_active_quest != player->quest_trackers.end(); dialog_active_quest++)
            {
                if (dialog_active_quest->quest_id != quest_id)
                    continue;
                int value = -1;
                // 1: a link of the dialog, 2: an answer typed in
                if (quest_dialog_type == 1)
                    value = this->quest_engine->GetRuleValue(dialog_active_quest->quest_id,
                        dialog_active_quest->state_index, accept_behavior_id);
                if (quest_dialog_type == 2)
                    value = this->quest_engine->GetRuleValue2(dialog_active_quest->quest_id,
                        dialog_active_quest->state_index, quest_arg);
                if (value >= 0)
                {
                    dialog_active_quest->state_index = value;
                    this->Player_ApplyQuestActions(player, dialog_active_quest, 1);
                    // a finished quest leaves the list; else the next dialog follows
                    if (dialog_active_quest->done)
                        player->quest_trackers.erase(dialog_active_quest);
                    else
                    {
                        if (player->map_id > 0)
                        {
                            String quest_dialog_reply = this->quest_engine->GetActionData2(dialog_active_quest->quest_id,
                                dialog_active_quest->state_index, accept_behavior_id);
                            if (quest_dialog_reply.Length() < 1)
                                break;
                            player->session_id = random(50000) + 10000;
                            player->session_token = random(30000) + 10000;
                            player->dialog_behavior_id = accept_behavior_id;
                            quest_dialog_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), 1);
                            quest_dialog_reply.Insert(this->quest_engine->GetQuestName(dialog_active_quest->quest_id),
                                1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(dialog_active_quest->quest_id, 2), 1);
                            quest_dialog_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), 1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(player->session_token, 2), 1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(player->session_id, 2), 1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(dialog_active_quest->quest_id, 2), 1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(accept_behavior_id, 2), 1);
                            quest_dialog_reply.Insert(EO_EncodeNumber(1, 1), 1);
                            this->Client_SendEncoded(player, AC_TALK, OP_QUEST,
                                quest_dialog_reply);
                        }
                    }
                    break;
                }
            }
            return true;
        }
        // .. Quest Use: talking to a quest npc in view
        if (action == AC_DO)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int quest_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            int quest_filter = this->EO_DecodeNumber(data.SubString(3, 2));
            MapCoord quest_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, quest_npc_index);
            if (quest_coords.x < 0 || quest_coords.y < 0)
                return true;
            if (!this->Server_InViewRange(quest_coords.x, quest_coords.y, player->x, player->y))
                return true;
            int quest_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, quest_npc_index);
            MapCoord quest_type_info = GUI->npc_values->GetType(quest_npc_id);
            if (quest_type_info.type != NpcType_Quest)
                return true;
            if (this->quest_engine->GetQuestLoaded(quest_type_info.behavior_id))
            {
                char quest_found = 1;
                vector<PlayerQuest>::iterator talk_finished_quest;
                for (talk_finished_quest = player->quest_history.begin();
                    talk_finished_quest != player->quest_history.end(); talk_finished_quest++)
                {
                    if (talk_finished_quest->quest_id == quest_type_info.behavior_id)
                    {
                        quest_found = 0;
                        break;
                    }
                }
                for (talk_finished_quest = player->quest_trackers.begin();
                    talk_finished_quest != player->quest_trackers.end(); talk_finished_quest++)
                {
                    if (talk_finished_quest->quest_id == quest_type_info.behavior_id)
                    {
                        quest_found = 0;
                        break;
                    }
                }
                // the npc's own quest starts, unless done or in progress; 10 at once at most
                if (quest_found && player->quest_trackers.size() < 10)
                {
                    int quest_version = this->quest_engine->GetQuestVersion(quest_type_info.behavior_id);
                    PlayerQuest tracker(quest_type_info.behavior_id, 0, quest_version);
                    player->quest_trackers.insert(player->quest_trackers.end(), tracker);
                }
            }
            String quest_talk_reply;
            String quest_name;
            String payload;
            int quest_count = 0;
            for (vector<PlayerQuest>::iterator talk_active_quest = player->quest_trackers.begin();
                talk_active_quest != player->quest_trackers.end(); talk_active_quest++)
            {
                if (!(talk_active_quest->quest_id == quest_filter || !quest_filter))
                    continue;
                if (!quest_name.Length())
                {
                    // the npc's chat line, at most once each 10 ticks a map
                    if (this->map_control->TryTakeQuestCooldown(player->map_id))
                    {
                        quest_name = this->quest_engine->GetActionData(talk_active_quest->quest_id,
                            talk_active_quest->state_index, quest_type_info.behavior_id);
                        if (quest_name.Length() > 0)
                        {
                            quest_name.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), 1);
                            quest_name.Insert(EO_EncodeNumber(quest_npc_index, 2), 1);
                            this->Server_BroadcastNearTile(-1, player->map_id, quest_coords, AC_PUBLIC,
                                OP_QUEST, quest_name);
                        }
                    }
                }
                if (!quest_talk_reply.Length())
                {
                    quest_talk_reply = this->quest_engine->GetActionData2(talk_active_quest->quest_id,
                        talk_active_quest->state_index, quest_type_info.behavior_id);
                    if (quest_talk_reply.Length() > 0)
                    {
                        player->session_id = random(50000) + 10000;
                        player->session_token = random(30000) + 10000;
                        player->dialog_behavior_id = quest_type_info.behavior_id;
                        payload = EO_EncodeNumber(quest_type_info.behavior_id, 2);
                        payload.Insert(EO_EncodeNumber(talk_active_quest->quest_id, 2), payload.Length() + 1);
                        payload.Insert(EO_EncodeNumber(player->session_id, 2), payload.Length() + 1);
                        payload.Insert(EO_EncodeNumber(player->session_token, 2), payload.Length() + 1);
                        payload.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), payload.Length() + 1);
                        payload.Insert(EO_EncodeNumber(talk_active_quest->quest_id, 2), payload.Length() + 1);
                        payload.Insert(this->quest_engine->GetQuestName(talk_active_quest->quest_id),
                            payload.Length() + 1);
                        payload.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), payload.Length() + 1);
                        quest_count++;
                    }
                }
                else if (!quest_filter)
                {
                    if (this->quest_engine->GetActionData2(talk_active_quest->quest_id, talk_active_quest->state_index,
                        quest_type_info.behavior_id) != "")
                    {
                        payload.Insert(EO_EncodeNumber(talk_active_quest->quest_id, 2), payload.Length() + 1);
                        payload.Insert(this->quest_engine->GetQuestName(talk_active_quest->quest_id),
                            payload.Length() + 1);
                        payload.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), payload.Length() + 1);
                        quest_count++;
                    }
                }
            }
            if (quest_talk_reply.Length() > 0)
            {
                payload.Insert(EO_EncodeNumber(quest_count, 1), 1);
                quest_talk_reply.Insert(payload, 1);
                this->Client_SendEncoded(player, AC_TALK, OP_QUEST, quest_talk_reply);
            }
            return true;
        }
    }
    // .. StatSkill: Take learns a spell, Junk resets, Remove forgets, Open at a master, Add raises
    if (family == OP_SKILL)
    {
        if (action == AC_GET)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int learn_session_token = this->EO_DecodeNumber(data.SubString(1, 4));
            int learn_spell_id = this->EO_DecodeNumber(data.SubString(5, 2));
            if (player->session_token != learn_session_token)
                return true;
            if (!GUI->learn_values->HasSkill(learn_session_token, learn_spell_id))
                return true;
            if (this->players->Player_HasSpellId(player, learn_spell_id))
                return true;
            LearnItemVal skill = GUI->learn_values->GetSkill(learn_session_token, learn_spell_id);
            if (!GUI->class_values->ClassMatches(player->class_id, skill.class_requirement))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_SKILL,
                    EO_EncodeNumber(SkillMasterReply_WrongClass, 2) + EO_EncodeNumber(player->class_id, 1));
                return true;
            }
            if (player->level < skill.level_requirement)
                return true;
            if (player->adj_strength < skill.str_requirement)
                return true;
            if (player->adj_intelligence < skill.wis_requirement)
                return true;
            if (player->adj_wisdom < skill.int_requirement)
                return true;
            if (player->adj_agility < skill.agi_requirement)
                return true;
            if (player->adj_constitution < skill.con_requirement)
                return true;
            if (player->adj_charisma < skill.cha_requirement)
                return true;
            if (skill.skill_requirement_1 > 0 && !this->players->Player_HasSpellId(player, skill.skill_requirement_1))
                return true;
            if (!(skill.skill_requirement_2 <= 0 ||
                this->players->Player_HasSpellId(player, skill.skill_requirement_2)))
                return true;
            if (!(skill.skill_requirement_3 <= 0 ||
                this->players->Player_HasSpellId(player, skill.skill_requirement_3)))
                return true;
            if (!(skill.skill_requirement_4 <= 0 ||
                this->players->Player_HasSpellId(player, skill.skill_requirement_4)))
                return true;
            // the master's price in gold
            if (!this->players->Player_RemoveItem(player, 1, skill.price))
                return true;
            this->players->Player_AddSpell(player, learn_spell_id);
            String skill_learn_reply = EO_EncodeNumber(learn_spell_id, 2);
            skill_learn_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), skill_learn_reply.Length() + 1);
            this->Client_SendEncoded(player, AC_GET, OP_SKILL, skill_learn_reply);
            return true;
        }
        if (action == AC_JUNK)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int reset_session_token = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != reset_session_token)
                return true;
            // .. a reset needs nothing equipped; the stats and spells are given back as points
            if (player->boots_graphic_id || player->accessory_graphic_id ||
                (player->gloves_graphic_id || player->armor_graphic_id) ||
                (player->belt_graphic_id || player->necklace_graphic_id) ||
                (player->hat_graphic_id || player->shield_graphic_id) ||
                (player->weapon_graphic_id || player->ring1_graphic_id) ||
                (player->ring2_graphic_id || player->armlet1_graphic_id) ||
                (player->armlet2_graphic_id || player->bracer1_graphic_id) || player->bracer2_graphic_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_SKILL,
                    EO_EncodeNumber(SkillMasterReply_RemoveItems, 2));
                return true;
            }
            player->base_strength = 0;
            player->base_wisdom = 0;
            player->base_intelligence = 0;
            player->base_agility = 0;
            player->base_constitution = 0;
            player->base_charisma = 0;
            this->players->Player_ClearSpells(player);
            player->stat_points = 3 * player->level;
            player->skill_points = 4 * player->level;
            if (player->hp > player->max_hp)
                player->hp = player->max_hp;
            if (player->tp > player->max_tp)
                player->tp = player->max_tp;
            player->CalculateAdjStats();
            this->Player_CalculateStats(player);
            player->CalculateHP_TP_SP();
            String skill_reset_reply = EO_EncodeNumber(player->stat_points, 2);
            skill_reset_reply.Insert(EO_EncodeNumber(player->skill_points, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->hp, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->max_hp, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->tp, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->max_tp, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->max_sp, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_strength, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_intelligence, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_wisdom, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_agility, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_constitution, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->adj_charisma, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->min_damage, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->max_damage, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->accuracy, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->evasion, 2), skill_reset_reply.Length() + 1);
            skill_reset_reply.Insert(EO_EncodeNumber(player->armor, 2), skill_reset_reply.Length() + 1);
            player->base_stats_dirty = true;
            this->Client_SendEncoded(player, AC_JUNK, OP_SKILL, skill_reset_reply);
            return true;
        }
        if (action == AC_DELETE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 6)
                return false;
            int forget_session_token = this->EO_DecodeNumber(data.SubString(1, 4));
            int forget_spell_id = this->EO_DecodeNumber(data.SubString(5, 2));
            if (player->session_token != forget_session_token)
                return true;
            if (this->players->Player_RemoveSpell(player, forget_spell_id))
                this->Client_SendEncoded(player, AC_DELETE, OP_SKILL,
                    EO_EncodeNumber(forget_spell_id, 2));
            return true;
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int skill_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord skill_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, skill_npc_index);
            if (skill_coords.x < 0 || skill_coords.y < 0)
                return true;
            if (!this->Server_InViewRange(skill_coords.x, skill_coords.y, player->x, player->y))
                return true;
            int skill_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, skill_npc_index);
            MapCoord skill_type_info = GUI->npc_values->GetType(skill_npc_id);
            if (skill_type_info.type != NpcType_Trainer)
                return true;
            int behavior_id = skill_type_info.behavior_id;
            player->session_token = skill_type_info.behavior_id;
            this->Client_SendEncoded(player, AC_OPEN, OP_SKILL,
                GUI->learn_values->BuildOpenData(skill_type_info.behavior_id));
            return true;
        }
        if (action == AC_ADD)
        {
            if (data.Length() != 3)
                return false;
            int action_type = this->EO_DecodeNumber(data.SubString(1, 1));
            int stat_or_spell_id = this->EO_DecodeNumber(data.SubString(2, 2));
            // .. 1: a stat point into a stat
            if (action_type == 1)
            {
                if (player->stat_points < 1)
                    return true;
                if (stat_or_spell_id < StatId_Str || stat_or_spell_id > StatId_Cha)
                    return true;
                player->stat_points--;
                if (stat_or_spell_id == StatId_Str)
                    player->base_strength++;
                if (stat_or_spell_id == StatId_Int)
                    player->base_intelligence++;
                if (stat_or_spell_id == StatId_Wis)
                    player->base_wisdom++;
                if (stat_or_spell_id == StatId_Agi)
                    player->base_agility++;
                if (stat_or_spell_id == StatId_Con)
                    player->base_constitution++;
                if (stat_or_spell_id == StatId_Cha)
                    player->base_charisma++;
                player->CalculateAdjStats();
                this->Player_CalculateStats(player);
                player->CalculateHP_TP_SP();
                String stat_reply = EO_EncodeNumber(player->stat_points, 2);
                stat_reply.Insert(EO_EncodeNumber(player->adj_strength, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->adj_intelligence, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->adj_wisdom, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->adj_agility, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->adj_constitution, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->adj_charisma, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->max_hp, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->max_tp, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->max_sp, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->weight_max, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->min_damage, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->max_damage, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->accuracy, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->evasion, 2), stat_reply.Length() + 1);
                stat_reply.Insert(EO_EncodeNumber(player->armor, 2), stat_reply.Length() + 1);
                player->base_stats_dirty = true;
                this->Client_SendEncoded(player, AC_SET, OP_SKILL, stat_reply);
                return true;
            }
            // .. 2: a skill point into a spell, up to level 100
            if (action_type == 2)
            {
                if (player->skill_points < 1)
                    return true;
                int spell_level = this->players->Player_GetSpellLevel(player, stat_or_spell_id);
                if (spell_level < 0 || spell_level > 99)
                    return true;
                player->skill_points--;
                int new_level = this->players->Player_LevelUpSpell(player, stat_or_spell_id);
                String spell_reply = EO_EncodeNumber(player->skill_points, 2);
                spell_reply.Insert(EO_EncodeNumber(stat_or_spell_id, 2), spell_reply.Length() + 1);
                spell_reply.Insert(EO_EncodeNumber(new_level, 2), spell_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_CONFIRM, OP_SKILL, spell_reply);
                return true;
            }
        }
    }
    // .. Marriage: at the marriage npc, 1 approves a partner for 500 gold, 2 divorces for 10000
    if (family == OP_LAW)
    {
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 10)
                return false;
            int subtype = this->EO_DecodeNumber(data.SubString(1, 1));
            int marriage_token = this->EO_DecodeNumber(data.SubString(2, 4));
            if (player->session_token != marriage_token)
                return true;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String marriage_name = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            marriage_name = LowerCase(marriage_name);
            marriage_name = this->mysql_controls->Mysql_SanitizeString(marriage_name, 0);
            if (marriage_name.Length() < 4)
                return false;
            if (subtype == 1)
            {
                if (player->partner_name.Length() > 3)
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                        EO_EncodeNumber(MarriageReply_AlreadyMarried, 2));
                    return true;
                }
                if (!this->players->Player_RemoveItem(player, 1, 500))
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                        EO_EncodeNumber(MarriageReply_NotEnoughGold, 2));
                    return true;
                }
                // the approval keeps the partner's first three letters
                player->partner_name = marriage_name.SubString(1, 3);
                String approval_msg = EO_EncodeNumber(MarriageReply_Success, 2);
                approval_msg.Insert(EO_EncodeNumber(player->item_change_remaining, 4), approval_msg.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_LAW, approval_msg);
                return true;
            }
            if (subtype == 2)
            {
                if (player->partner_name.Length() < 4)
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                        EO_EncodeNumber(MarriageReply_NotMarried, 2));
                    return true;
                }
                if (LowerCase(player->partner_name) != LowerCase(marriage_name))
                {
                    this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                        EO_EncodeNumber(MarriageReply_WrongName, 2));
                    return true;
                }
                Player *divorce_target = this->players->FindByName(marriage_name);
                if (divorce_target)
                {
                    if (!this->players->Player_RemoveItem(player, 1, 10000))
                    {
                        this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                            EO_EncodeNumber(MarriageReply_NotEnoughGold, 2));
                        return true;
                    }
                    this->Client_SendEncoded(divorce_target, AC_RESULT, OP_LAW,
                        EO_EncodeNumber(MarriageReply_DivorceNotification, 2));
                    divorce_target->partner_name = "";
                }
                else
                {
                    if (!this->Server_TickOncePerFiveSeconds())
                    {
                        this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                            EO_EncodeNumber(MarriageReply_ServiceBusy, 2));
                        return true;
                    }
                    if (!this->players->Player_RemoveItem(player, 1, 10000))
                    {
                        this->Client_SendEncoded(player, AC_RESULT, OP_LAW,
                            EO_EncodeNumber(MarriageReply_NotEnoughGold, 2));
                        return true;
                    }
                    // an offline partner is marked DV- in the database
                    this->mysql_controls->Mysql_SubmitExec(player->account_ident,
                        "UPDATE endl_characters SET partner = 'DV-' WHERE name = '" + marriage_name + "'");
                }
                player->partner_name = "";
                String divorce_msg = EO_EncodeNumber(MarriageReply_Success, 2);
                divorce_msg.Insert(EO_EncodeNumber(player->item_change_remaining, 4), divorce_msg.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_LAW, divorce_msg);
                return true;
            }
        }
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int marriage_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord marriage_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, marriage_npc_index);
            if (marriage_coords.x < 0 || marriage_coords.y < 0)
                return true;
            if (!this->Server_InViewRange(marriage_coords.x, marriage_coords.y, player->x, player->y))
                return true;
            int marriage_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, marriage_npc_index);
            MapCoord type_info = GUI->npc_values->GetType(marriage_npc_id);
            // npc type 13, the marriage npc
            if (type_info.x != 13)
                return true;
            player->session_token = random(10000) + 900001;
            this->Client_SendEncoded(player, AC_OPEN, OP_LAW,
                EO_EncodeNumber(player->session_token, 3));
            return true;
        }
    }
    // .. Priest: Open at a free priest, Request asks the partner, Accept starts the wedding, Use says yes
    if (family == OP_PRIEST)
    {
        if (action == AC_OPEN)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            if (player->partner_name.Length() > 3)
                return true;
            int priest_npc_index = this->EO_DecodeNumber(data.SubString(1, 2));
            MapCoord priest_coords = this->map_control->GetNpcCoordsByIndex(player->map_id, priest_npc_index);
            if (priest_coords.x < 0 || priest_coords.y < 0)
                return false;
            if (!this->Server_InViewRange(priest_coords.x, priest_coords.y, player->x, player->y))
                return true;
            if (GUI->weddings->Has(player->map_id, priest_npc_index))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_Busy, 2));
                return true;
            }
            int priest_open_npc_id = this->map_control->GetNpcIdByIndex(player->map_id, priest_npc_index);
            MapCoord type_info = GUI->npc_values->GetType(priest_open_npc_id);
            // npc type 12, the priest
            if (type_info.x != 12)
                return true;
            else if (player->level < PROTECTED_MIN_LEVEL)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_LowLevel, 2));
                return true;
            }
            // the wedding clothes: armor 21 for gender 1, armor 2 for gender 0
            if (player->gender == 1 && player->armor_graphic_id != 21)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_NotDressed, 2));
                return true;
            }
            if (!player->gender && player->armor_graphic_id != 2)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_NotDressed, 2));
                return true;
            }
            player->session_token = random(10000) + 800001;
            player->npc_index = priest_npc_index;
            this->Client_SendEncoded(player, AC_OPEN, OP_PRIEST,
                EO_EncodeNumber(player->session_token, 4));
            return true;
        }
        if (action == AC_REQUEST)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            int priest_token = this->EO_DecodeNumber(data.SubString(1, 4));
            if (player->session_token != priest_token)
                return true;
            if (priest_token < 800000 || priest_token > 900000)
                return true;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            PacketReader_GetBreakString();
            String priest_name = this->mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
            if (priest_name.Length() < 4 || priest_name.Length() > 24)
                return true;
            Player *priest_request_target = this->players->FindByName(priest_name);
            if (!priest_request_target)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerNotPresent, 2));
                return true;
            }
            if (priest_request_target->player_id == player->player_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerNotPresent, 2));
                return true;
            }
            if (priest_request_target->map_id != player->map_id)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerNotPresent, 2));
                return true;
            }
            if (priest_request_target->partner_name.Length() > 3)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerAlreadyMarried, 2));
                return true;
            }
            // both must have approved each other
            if (LowerCase(priest_name.SubString(1, 3)) != LowerCase(player->partner_name))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_NoPermission, 2));
                return true;
            }
            if (LowerCase(priest_request_target->partner_name) != LowerCase(player->name.SubString(1, 3)))
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_NoPermission, 2));
                return true;
            }
            if (priest_request_target->gender == 1 && priest_request_target->armor_graphic_id != 21)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerNotDressed, 2));
                return true;
            }
            if (!priest_request_target->gender && priest_request_target->armor_graphic_id != 2)
            {
                this->Client_SendEncoded(player, AC_RESULT, OP_PRIEST,
                    EO_EncodeNumber(PriestReply_PartnerNotDressed, 2));
                return true;
            }
            player->wedding_request_id = priest_request_target->player_id;
            String priest_msg = EO_EncodeNumber(player->player_id, 2);
            priest_msg.Insert(player->name, priest_msg.Length() + 1);
            this->Client_SendEncoded(priest_request_target, AC_REQUEST, OP_PRIEST, priest_msg);
            return true;
        }
        if (action == AC_CONFIRM)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 2)
                return false;
            int priest_id = this->EO_DecodeNumber(data.SubString(1, 2));
            Player *priest_accept_target = this->players->GetById(priest_id);
            if (!priest_accept_target)
                return true;
            if (priest_accept_target->map_id != player->map_id)
                return true;
            if (priest_accept_target->wedding_request_id != player->player_id)
                return true;
            if (GUI->weddings->Has(priest_accept_target->map_id, priest_accept_target->npc_index))
                return true;
            int priest_accept_npc_id = this->map_control->GetNpcIdByIndex(priest_accept_target->map_id,
                priest_accept_target->npc_index);
            MapCoord priest_accept_type_info = GUI->npc_values->GetType(priest_accept_npc_id);
            if (priest_accept_type_info.type != NpcType_Priest)
                return true;
            player->npc_index = priest_accept_target->npc_index;
            player->session_token = priest_accept_target->session_token;
            GUI->weddings->Add(priest_accept_target->map_id, priest_accept_target->npc_index, player->player_id,
                player->name, priest_accept_target->player_id, priest_accept_target->name);
            return true;
        }
        if (action == AC_DO)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 4)
                return false;
            if (player->partner_name.Length() > 3)
                return true;
            GUI->weddings->Confirm(player->map_id, player->npc_index, player->player_id);
            return true;
        }
    }
    // .. AdminInteract: a message or a report to the admins
    if (family == OP_ADMIN)
    {
        if (action == AC_PRIVATE)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 1)
                return false;
            String admin_tell_msg = EO_EncodeNumber(AdminMessageType_Message, 2);
            admin_tell_msg.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_tell_msg.Length() + 1);
            admin_tell_msg.Insert(player->name, admin_tell_msg.Length() + 1);
            admin_tell_msg.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_tell_msg.Length() + 1);
            admin_tell_msg.Insert(data, admin_tell_msg.Length() + 1);
            this->Admin_BroadcastToAll(AC_RESULT, OP_ADMIN, admin_tell_msg);
            return true;
        }
        if (action == AC_PUBLIC)
        {
            if (!player->logged_in)
                return false;
            if (data.Length() < 1)
                return false;
            this->PacketReader_Init(data, this->EO_GetBreakByte(EO_BREAK_BYTE));
            String reported_name = PacketReader_GetBreakString();
            String report_text = PacketReader_GetBreakString();
            String admin_report_msg = EO_EncodeNumber(AdminMessageType_Report, 2);
            admin_report_msg.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_report_msg.Length() + 1);
            admin_report_msg.Insert(player->name, admin_report_msg.Length() + 1);
            admin_report_msg.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_report_msg.Length() + 1);
            admin_report_msg.Insert(report_text, admin_report_msg.Length() + 1);
            admin_report_msg.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), admin_report_msg.Length() + 1);
            admin_report_msg.Insert(reported_name, admin_report_msg.Length() + 1);
            this->Admin_BroadcastToAll(AC_RESULT, OP_ADMIN, admin_report_msg);
            return true;
        }
    }
    // .. Message: Request S, P or G for the server's numbers, the top players or the top guilds
    if (family == OP_SYSTEM)
    {
        if (action == AC_REQUEST)
        {
            if (data.Length() != 1)
                return false;
            if (data == "S")
            {
                String server_reply = "S";
                server_reply.Insert(EO_EncodeNumber(GUI->server->Socket->ActiveConnections, 2),
                    server_reply.Length() + 1);
                server_reply.Insert(EO_EncodeNumber(this->players->GetOnlineCount(), 2), server_reply.Length() + 1);
                server_reply.Insert(EO_EncodeNumber(this->players->GetPeakOnline(), 2), server_reply.Length() + 1);
                TTimeStamp now = DateTimeToTimeStamp(Now());
                int date_delta = now.Date - this->mysql_controls->Server_GetStartTime().Date;
                int time_delta = now.Time - this->mysql_controls->Server_GetStartTime().Time;
                // the uptime in minutes
                int minutes = 1440 * date_delta + time_delta / MS_PER_MINUTE;
                server_reply.Insert(EO_EncodeNumber(minutes, 4), server_reply.Length() + 1);
                server_reply.Insert(EO_EncodeNumber(this->mysql_controls->file_cache->accounts_count, 4),
                    server_reply.Length() + 1);
                server_reply.Insert(EO_EncodeNumber(this->mysql_controls->file_cache->characters_count, 4),
                    server_reply.Length() + 1);
                server_reply.Insert(EO_EncodeNumber(this->mysql_controls->file_cache->guilds_count, 4),
                    server_reply.Length() + 1);
                server_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), server_reply.Length() + 1);
                server_reply.Insert(Server_FormatSentTraffic(), server_reply.Length() + 1);
                server_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), server_reply.Length() + 1);
                server_reply.Insert(Server_FormatReceivedTraffic(), server_reply.Length() + 1);
                server_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), server_reply.Length() + 1);
                this->Client_SendEncoded(player, AC_RESULT, OP_SYSTEM, server_reply);
                return true;
            }
            if (data == "P")
            {
                String players_reply = "P";
                players_reply.Insert(EO_EncodeNumber((int)this->mysql_controls->file_cache->top_players.size(), 2),
                    players_reply.Length() + 1);
                vector<TopPlayer *>::iterator top_player;
                for (top_player = this->mysql_controls->file_cache->top_players.begin();
                    top_player != this->mysql_controls->file_cache->top_players.end(); top_player++)
                {
                    players_reply.Insert((*top_player)->name, players_reply.Length() + 1);
                    players_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), players_reply.Length() + 1);
                    players_reply.Insert((*top_player)->title, players_reply.Length() + 1);
                    players_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), players_reply.Length() + 1);
                    players_reply.Insert(EO_EncodeNumber((*top_player)->level, 1), players_reply.Length() + 1);
                    players_reply.Insert(EO_EncodeNumber((*top_player)->experience, 4), players_reply.Length() + 1);
                    players_reply.Insert(EO_EncodeNumber((*top_player)->gender, 1), players_reply.Length() + 1);
                    players_reply.Insert(EO_EncodeNumber((*top_player)->privilege, 1), players_reply.Length() + 1);
                    players_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), players_reply.Length() + 1);
                }
                this->Client_SendEncoded(player, AC_RESULT, OP_SYSTEM, players_reply);
                return true;
            }
            if (data == "G")
            {
                String guilds_reply = "G";
                guilds_reply.Insert(EO_EncodeNumber((int)this->mysql_controls->file_cache->top_guilds.size(), 2),
                    guilds_reply.Length() + 1);
                vector<TopGuild *>::iterator top_guild;
                for (top_guild = this->mysql_controls->file_cache->top_guilds.begin();
                    top_guild != this->mysql_controls->file_cache->top_guilds.end(); top_guild++)
                {
                    guilds_reply.Insert((*top_guild)->ident_guild, guilds_reply.Length() + 1);
                    guilds_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), guilds_reply.Length() + 1);
                    guilds_reply.Insert((*top_guild)->guild, guilds_reply.Length() + 1);
                    guilds_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), guilds_reply.Length() + 1);
                    guilds_reply.Insert(EO_EncodeNumber((*top_guild)->exptotal, 4), guilds_reply.Length() + 1);
                    guilds_reply.Insert(EO_EncodeNumber((*top_guild)->exphigh, 1), guilds_reply.Length() + 1);
                    guilds_reply.Insert(EO_EncodeNumber((*top_guild)->members, 2), guilds_reply.Length() + 1);
                    guilds_reply.Insert((char)this->EO_GetBreakByte(EO_BREAK_BYTE), guilds_reply.Length() + 1);
                }
                this->Client_SendEncoded(player, AC_RESULT, OP_SYSTEM, guilds_reply);
                return true;
            }
        }
        if (action == AC_LIST)
        {
            this->Client_SendRaw(player, Message_BuildPlayerList(), 12);
            return true;
        }
        if (action == AC_SYN1)
        {
            if (data.Length() != 2)
                return false;
            this->Client_SendEncoded(player, AC_SYN2, OP_SYSTEM, EO_EncodeNumber(2, 2));
            return true;
        }
    }
    // .. Global: Open and Close the world chat; Player and Remove turn the whisper block on and off
    if (family == OP_PCHAT)
    {
        if (!player->logged_in)
            return false;
        if (action == AC_OPEN)
        {
            player->global_chat = true;
            String global_names = this->global_history[0];
            int i;
            for (i = 1; i < GLOBAL_HISTORY_SIZE; i++)
                global_names.Insert(this->global_history[i], global_names.Length() + 1);
            if (global_names != "")
                this->Client_SendEncoded(player, AC_LIST, OP_CHAT, global_names);
            return true;
        }
        if (action == AC_CLOSE)
        {
            player->global_chat = false;
            return true;
        }
        if (action == AC_SET)
        {
            player->whispers_disabled = true;
            return true;
        }
        if (action == AC_DELETE)
        {
            player->whispers_disabled = false;
            return true;
        }
    }
    return false;
}

// A query's result came back from the database thread: it goes on with the request that asked, by its type.
void Packets::MysqlCallback_Dispatch(mySQLtask *query_result)
{
    Player *player = players->GetById(query_result->player_id);
    if (!player)
        return;
    // an answer for an earlier connection on the same socket
    if (player->query_id != query_result->expected_query_id)
        return;
    // .. Login: the password, the ban, a login still pending or online; then their characters
    if (query_result->query_type == DbQueryType_Login)
    {
        PacketReader_Init(query_result->data, EO_GetBreakByte(EO_BREAK_BYTE));
        String login_account = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String login_password = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        if (GUI->myquery->RecordCount < 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_WrongUser, 2) + "NO");
            return;
        }
        if (login_password != Account_DecodePassword(mysql_controls->Db_GetString("password")))
        {
            Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_WrongUserPassword, 2) + "NO");
            return;
        }
        if (mysql_controls->Db_GetInt("banned") > 0)
        {
            Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_Banned, 2) + "NO");
            player->removing = true;
            return;
        }
        int ident = mysql_controls->Db_GetInt("ident");
        String account_name = mysql_controls->Db_GetString("account");
        String account_type = mysql_controls->Db_GetString("type");
        if (mysql_controls->IsTaskPending(ident))
        {
            Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_LoggedIn, 2) + "NO");
            return;
        }
        if (players->IsAccountIdentOnline(ident))
        {
            Client_SendEncoded(player, AC_RESULT, OP_LOGIN,
                EO_EncodeNumber(LoginReply_LoggedIn, 2) + "NO");
            return;
        }
        // VIP and DEV accounts keep their name reserved for their ip
        if (account_type == "VIP" || account_type == "DEV")
        {
            logins->SetReservedIp(login_account, player->socket->RemoteAddress);
            player->remove_timer = -1;
        }
        if (player->remove_timer > 0)
        {
            player->removing = true;
            return;
        }
        player->account_ident = ident;
        player->account_name = account_name;
        player->account_type = account_type;
        // a second connection on the account: a 2 minute ban
        if (players->IsAccountNameTaken(player->account_name, player->player_id))
        {
            banned->AddBan(player->remote_ip, player->hdid, (char)0, 120);
            player->removing = true;
            return;
        }
        player->account_logged_in = true;
        TDateTime login_now = Now();
        mysql_controls->Mysql_SubmitExec_FromCallback(player->account_ident,
            "UPDATE endl_accounts SET lastvisit = '" + login_now.DateString() + "' WHERE ident = " + IntToStr(ident));
        mysql_controls->Mysql_SubmitQuery_FromCallback(DbQueryType_LoginCharacters, player->player_id, player->query_id,
            "",
            "SELECT * FROM endl_characters WHERE ident_account = " + IntToStr((DWORD)player->account_ident) +
            " ORDER BY level DESC LIMIT 3");
        return;
    }
    if (query_result->query_type == DbQueryType_LoginCharacters)
    {
        Login_SendCharacterList(player, AC_RESULT, OP_LOGIN, EO_EncodeNumber(LoginReply_Ok, 2));
        return;
    }
    // .. CharacterCreate: a new character on the start map, when the name is free
    if (query_result->query_type == DbQueryType_CharacterCreate)
    {
        if (GUI->myquery->RecordCount > 0)
        {
            Client_SendEncoded(player, AC_RESULT, OP_CHARACTER,
                EO_EncodeNumber(CharacterReply_Exists, 2) + "NO");
            return;
        }
        int gender = EO_DecodeNumber(query_result->data.SubString(3, 2));
        int hair_modal = EO_DecodeNumber(query_result->data.SubString(5, 2));
        int hair_color = EO_DecodeNumber(query_result->data.SubString(7, 2));
        int skin_color = EO_DecodeNumber(query_result->data.SubString(9, 2));
        String character_create_name = mysql_controls->Db_SanitizeString(PacketReader_GetBreakStringAt(2,
            query_result->data, EO_GetBreakByte(EO_BREAK_BYTE)));
        TDateTime character_create_now = Now();
        String character_create_sql = "INSERT INTO endl_characters (ident_account, ident_guild, ident_class, name, "
            "signup, gender, hairmodal, haircolor, skincolor, nav_map, nav_x, nav_y, hp_max, hp_now, mp_max, mp_now, "
            "sp_max, clientusge, money_bank ) VALUES (";
        character_create_sql = character_create_sql + IntToStr((DWORD)player->account_ident) + ",";
        character_create_sql = character_create_sql + "'0',1,'";
        character_create_sql = character_create_sql + character_create_name + "',";
        character_create_sql = character_create_sql + "'" + character_create_now.DateString() + "',";
        character_create_sql = character_create_sql + IntToStr(gender) + ",";
        character_create_sql = character_create_sql + IntToStr(hair_modal) + ",";
        character_create_sql = character_create_sql + IntToStr(hair_color) + ",";
        character_create_sql = character_create_sql + IntToStr(skin_color) + ",";
        character_create_sql = character_create_sql + IntToStr(settings->GetStartMap()) + ",";
        character_create_sql = character_create_sql + IntToStr(settings->GetStartX()) + ",";
        character_create_sql = character_create_sql + IntToStr(settings->GetStartY()) + ",";
        // hp 10, tp 10, sp 20
        character_create_sql = character_create_sql + "10,10,10,10,20,0,0)";
        mysql_controls->Mysql_SubmitExec_FromCallback(player->account_ident, character_create_sql);
        mysql_controls->Mysql_SubmitQuery_FromCallback(DbQueryType_CharacterList, player->player_id, player->query_id,
            "",
            "SELECT * FROM endl_characters WHERE ident_account = " + IntToStr((DWORD)player->account_ident) +
            " ORDER BY level DESC LIMIT 3");
        mysql_controls->file_cache->characters_count++;
        return;
    }
    if (query_result->query_type == DbQueryType_CharacterList)
    {
        Login_SendCharacterList(player, AC_RESULT, OP_CHARACTER,
            EO_EncodeNumber(CharacterReply_Ok, 2));
        player->pending_char_name = "";
        player->session_id = random(50000) + 10000;
        return;
    }
    // .. AccountCheck: the name is free; the reply carries the session for Create
    if (query_result->query_type == DbQueryType_AccountCheck)
    {
        if (GUI->myquery->RecordCount > 0)
        {
            Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
                EO_EncodeNumber(AccountReply_Exists, 2) + "NO");
            return;
        }
        String reply = EO_EncodeNumber(player->session_id, 2);
        reply.Insert(EO_EncodeNumber(ping_history[0], 1), reply.Length() + 1);
        reply.Insert("OK", reply.Length() + 1);
        Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT, reply);
        return;
    }
    // .. AccountCreate: the new account, with its password hidden and its ip
    if (query_result->query_type == DbQueryType_AccountCreate)
    {
        if (GUI->myquery->RecordCount > 0)
        {
            player->removing = true;
            return;
        }
        PacketReader_Init(query_result->data, EO_GetBreakByte(EO_BREAK_BYTE));
        PacketReader_GetBreakString();
        int code = EO_DecodeNumber(query_result->data.SubString(1, 2));
        String create_account = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        if (player->session_id != code)
        {
            player->removing = true;
            return;
        }
        if (player->account_create_cooldown > 4)
            return;
        player->account_create_cooldown = 6;
        String account_create_password = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String realname = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String location = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String email = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String serial_c = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String serial_h = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        TDateTime account_create_now = Now();
        String account_create_sql = "INSERT INTO endl_accounts (account, password, realname, location, email,  signup, "
            "lastvisit, serial_c, serial_h , ipaddress, banned) VALUES (";
        account_create_sql = account_create_sql + "'" + create_account + "',";
        account_create_sql = account_create_sql + "ENCODE('" + Account_EncodePassword(account_create_password) +
            "','eoeokeyendl'),";
        account_create_sql = account_create_sql + "'" + realname + "',";
        account_create_sql = account_create_sql + "'" + location + "',";
        account_create_sql = account_create_sql + "'" + email + "',";
        account_create_sql = account_create_sql + "'" + account_create_now.DateString() + "',";
        account_create_sql = account_create_sql + "'" + account_create_now.DateString() + "',";
        account_create_sql = account_create_sql + "'" + serial_c + "',";
        account_create_sql = account_create_sql + "'" + serial_h + "',";
        account_create_sql = account_create_sql + "'" + player->socket->RemoteAddress + "',";
        account_create_sql = account_create_sql + "0)";
        mysql_controls->Mysql_SubmitExec(0, account_create_sql);
        Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
            EO_EncodeNumber(AccountReply_Created, 2) + "GO");
        mysql_controls->file_cache->accounts_count++;
        player->session_id = random(50000) + 10000;
        return;
    }
    // .. AccountChangePassword: the old password must match
    if (query_result->query_type == DbQueryType_AccountChangePassword)
    {
        PacketReader_Init(query_result->data, EO_GetBreakByte(EO_BREAK_BYTE));
        String password_change_account = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String old_password = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String new_password = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        if (GUI->myquery->RecordCount < 1)
        {
            player->removing = true;
            return;
        }
        if (Account_DecodePassword(mysql_controls->Db_GetString("password")) != old_password ||
            mysql_controls->Db_GetString("account") != password_change_account)
        {
            Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
                EO_EncodeNumber(AccountReply_ChangeFailed, 2) + "NO");
            return;
        }
        Client_SendEncoded(player, AC_RESULT, OP_ACCOUNT,
            EO_EncodeNumber(AccountReply_Changed, 2) + "OK");
        mysql_controls->Mysql_SubmitExec(player->account_ident,
            "UPDATE endl_accounts SET password = ENCODE('" + Account_EncodePassword(new_password) +
            "','eoeokeyendl') WHERE ident = " + IntToStr((DWORD)player->account_ident));
        return;
    }
    // .. GuildRankName: the rank's name, then the member gets the rank, online or not
    if (query_result->query_type == DbQueryType_GuildRankName)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        int rank_lookup_token = EO_DecodeNumber(query_result->data.SubString(5, 1));
        String rank_lookup_char_name = query_result->data.SubString(6, query_result->data.Length() - 5);
        String rank_label = "rank" + IntToStr(rank_lookup_token);
        String rank_value = mysql_controls->Db_GetString(rank_label);
        Player *target = players->FindByName(rank_lookup_char_name);
        rank_value = mysql_controls->Mysql_SanitizeString(rank_value, 0);
        if (!target)
        {
            player->guild_query_text = rank_value;
            mysql_controls->Mysql_SubmitQuery_FromCallback(DbQueryType_GuildRankTarget, player->player_id,
                player->query_id, query_result->data,
                "SELECT ident_guild, ident_rank FROM endl_characters WHERE name = '" + rank_lookup_char_name +
                "' AND ident_guild = '" + player->guild_tag + "' LIMIT 1");
            return;
        }
        if (target->guild_tag != player->guild_tag)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RankingNotMember, 2));
            return;
        }
        if (target->guild_rank_id == 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RankingLeader, 2));
            return;
        }
        target->guild_rank_name = rank_value;
        target->guild_rank_id = rank_lookup_token;
        Client_SendEncoded(target, AC_CONFIRM, OP_GUILD,
            EO_EncodeNumber(rank_lookup_token, 1) + rank_value);
        Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_Updated, 2));
        return;
    }
    if (query_result->query_type == DbQueryType_GuildRankTarget)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        int rank_change_token = EO_DecodeNumber(query_result->data.SubString(5, 1));
        String rank_change_char_name = query_result->data.SubString(6, query_result->data.Length() - 5);
        if (LowerCase(mysql_controls->Db_GetString("ident_guild")) != LowerCase(player->guild_tag))
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RankingNotMember, 2));
            return;
        }
        if (mysql_controls->Db_GetInt("ident_rank") == 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RankingLeader, 2));
            return;
        }
        mysql_controls->Mysql_SubmitExec_FromCallback(player->account_ident,
            "UPDATE endl_characters SET ident_rank = " + IntToStr(rank_change_token) + ", rank = '" +
            player->guild_query_text + "' WHERE name = '" + rank_change_char_name + "' AND ident_guild = '" +
            player->guild_tag + "'");
        Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_Updated, 2));
        return;
    }
    // .. GuildKickMember: an offline member leaves the guild in the database
    if (query_result->query_type == DbQueryType_GuildKickMember)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        if (LowerCase(mysql_controls->Db_GetString("ident_guild")) != LowerCase(player->guild_tag))
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RemoveNotMember, 2));
            return;
        }
        if (mysql_controls->Db_GetInt("ident_rank") == 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_RemoveLeader, 2));
            return;
        }
        String guild_kick_char_name = query_result->data.SubString(5, query_result->data.Length() - 4);
        mysql_controls->Mysql_SubmitExec_FromCallback(player->account_ident,
            "UPDATE endl_characters SET ident_guild = '0', ident_rank = 9, guild = '', rank = '' WHERE name = '" +
            guild_kick_char_name + "' AND ident_guild = '" + player->guild_tag + "'");
        Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_Removed, 2));
        return;
    }
    if (query_result->query_type == DbQueryType_GuildDescription)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        String description = mysql_controls->Db_GetString("description");
        if (!description.Length())
            description = " ";
        Client_SendEncoded(player, AC_GET, OP_GUILD, description);
        return;
    }
    if (query_result->query_type == DbQueryType_GuildRanks)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        String ranks = mysql_controls->Db_GetString("rank1");
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank2"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank3"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank4"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank5"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank6"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank7"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank8"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        ranks.Insert(mysql_controls->Db_GetString("rank9"), ranks.Length() + 1);
        ranks.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), ranks.Length() + 1);
        Client_SendEncoded(player, AC_RANK, OP_GUILD, ranks);
        return;
    }
    if (query_result->query_type == DbQueryType_GuildBank)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        Client_SendEncoded(player, AC_SELL, OP_GUILD,
            EO_EncodeNumber(mysql_controls->Db_GetInt("money"), 4));
        return;
    }
    if (query_result->query_type == DbQueryType_GuildMemberList)
    {
        if (GUI->myquery->RecordCount < 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_NotFound, 2));
            return;
        }
        String list = EO_EncodeNumber(GUI->myquery->RecordCount, 2);
        list.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
        while (!GUI->myquery->Eof)
        {
            list.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("ident_rank"), 1), list.Length() + 1);
            list.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            list.Insert(mysql_controls->Db_GetString("name"), list.Length() + 1);
            list.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            list.Insert(mysql_controls->Db_GetString("rank"), list.Length() + 1);
            list.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            GUI->myquery->Next();
        }
        Client_SendEncoded(player, AC_PRIVATE, OP_GUILD, list);
        return;
    }
    // .. GuildInfo: the guild's name, tag, description, wealth, ranks, then its leaders
    if (query_result->query_type == DbQueryType_GuildInfo)
    {
        if (GUI->myquery->RecordCount < 1)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_NotFound, 2));
            return;
        }
        // the wealth by the bank: 2000 poor, 10000 normal, 50000 wealthy, 100000 very wealthy
        String type = "bankrupt";
        int guild_info_money = mysql_controls->Db_GetInt("money");
        String guild_info_tag = mysql_controls->Db_GetString("tag");
        if (guild_info_money >= 2000)
            type = "poor";
        if (guild_info_money >= 10000)
            type = "normal";
        if (guild_info_money >= 50000)
            type = "wealthy";
        if (guild_info_money >= 100000)
            type = "very wealthy";
        player->guild_query_text = mysql_controls->Db_GetString("name");
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("tag"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("signup"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("description"),
            player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(type, player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank1"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank2"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank3"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank4"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank5"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank6"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank7"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank8"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert(mysql_controls->Db_GetString("rank9"), player->guild_query_text.Length() + 1);
        player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), player->guild_query_text.Length() + 1);
        mysql_controls->Mysql_SubmitQuery_FromCallback(DbQueryType_GuildRankedMembers, player->player_id,
            player->query_id, "",
            "SELECT ident_rank, name FROM endl_characters WHERE ident_rank < 3 AND ident_guild = '" + guild_info_tag +
            "' ORDER by ident_rank asc LIMIT 20");
        return;
    }
    if (query_result->query_type == DbQueryType_GuildRankedMembers)
    {
        bool has_rows = true;
        if (GUI->myquery->RecordCount < 1)
            has_rows = 0;
        if (has_rows)
        {
            player->guild_query_text.Insert(EO_EncodeNumber(mysql_controls->GetResultCount(), 2),
                player->guild_query_text.Length() + 1);
            player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE),
                player->guild_query_text.Length() + 1);
            while (!mysql_controls->ResultAtEnd())
            {
                player->guild_query_text.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("ident_rank"), 1),
                    player->guild_query_text.Length() + 1);
                player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE),
                    player->guild_query_text.Length() + 1);
                player->guild_query_text.Insert(mysql_controls->Db_GetString("name"),
                    player->guild_query_text.Length() + 1);
                player->guild_query_text.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE),
                    player->guild_query_text.Length() + 1);
                mysql_controls->NextResultRecord();
            }
        }
        Client_SendEncoded(player, AC_PUBLIC, OP_GUILD, player->guild_query_text);
        return;
    }
    // .. GuildCreateCheck: the name and tag are free; the players on the map are asked to join
    if (query_result->query_type == DbQueryType_GuildCreateCheck)
    {
        if (mysql_controls->GetResultCount() > 0)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_Exists, 2));
            return;
        }
        PacketReader_Init(query_result->data, EO_GetBreakByte(EO_BREAK_BYTE));
        PacketReader_GetBreakString();
        String guild_request_tag = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String guild_request_name = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        player->guild_inviter_id = player->player_id;
        Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_CreateBegin, 2));
        String guild_request_msg = EO_EncodeNumber(player->player_id, 2);
        guild_request_msg.Insert(guild_request_name + " (" + guild_request_tag + ")", guild_request_msg.Length() + 1);
        Server_BroadcastToMap(player->map_id, AC_REQUEST, OP_GUILD, guild_request_msg);
        return;
    }
    // .. GuildRequestCheck: 10 agreed and 50000 gold: the guild is made, its bank starts at 10000
    if (query_result->query_type == DbQueryType_GuildRequestCheck)
    {
        if (GUI->myquery->RecordCount > 0)
            return;
        if (players->CountGuildInvites(player) < 10)
        {
            Client_SendEncoded(player, AC_RESULT, OP_GUILD,
                EO_EncodeNumber(GuildReply_NoCandidates, 2));
            return;
        }
        if (!players->Player_RemoveItem(player, 1, 50000))
            return;
        PacketReader_Init(query_result->data, EO_GetBreakByte(EO_BREAK_BYTE));
        PacketReader_GetBreakString();
        String guild_create_tag = UpperCase(mysql_controls->Db_SanitizeString(PacketReader_GetBreakString()));
        String guild_create_name = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        String guild_create_description = mysql_controls->Db_SanitizeString(PacketReader_GetBreakString());
        player->guild_tag = guild_create_tag;
        player->guild_name = guild_create_name;
        player->guild_rank_name = "Leader";
        player->guild_inviter_id = -1;
        player->guild_rank_id = 1;
        players->GuildSetMemberInfo(player, guild_create_name, guild_create_tag);
        TDateTime guild_create_now = Now();
        guild_create_tag = mysql_controls->Db_SanitizeString(guild_create_tag);
        guild_create_name = mysql_controls->Db_SanitizeString(guild_create_name);
        guild_create_description = mysql_controls->Db_SanitizeString(guild_create_description);
        String guild_create_sql = "INSERT INTO endl_guilds (tag, name, description, money, signup, rank1, rank2 ) "
            "VALUES (";
        guild_create_sql = guild_create_sql + "'" + UpperCase(guild_create_tag) + "',";
        guild_create_sql = guild_create_sql + "'" + guild_create_name + "',";
        guild_create_sql = guild_create_sql + "'" + guild_create_description + "',";
        guild_create_sql = guild_create_sql + "10000,";
        guild_create_sql = guild_create_sql + "'" + guild_create_now.DateString() + "',";
        guild_create_sql = guild_create_sql + "'Leader',";
        guild_create_sql = guild_create_sql + "'Recruiter')";
        mysql_controls->Mysql_SubmitExec_FromCallback(0, guild_create_sql);
        if (guild_create_tag.Length() == GUILD_TAG_MIN_LENGTH)
            guild_create_tag = guild_create_tag + " ";
        String guild_create_msg = EO_EncodeNumber(player->player_id, 2);
        guild_create_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_create_msg.Length() + 1);
        guild_create_msg.Insert(guild_create_tag, guild_create_msg.Length() + 1);
        guild_create_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_create_msg.Length() + 1);
        guild_create_msg.Insert(guild_create_name, guild_create_msg.Length() + 1);
        guild_create_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_create_msg.Length() + 1);
        guild_create_msg.Insert("Leader", guild_create_msg.Length() + 1);
        guild_create_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_create_msg.Length() + 1);
        Guild_BroadcastToAll(player, AC_NEW, OP_GUILD, guild_create_msg);
        guild_create_msg.Insert(EO_EncodeNumber(player->item_change_remaining, 4), guild_create_msg.Length() + 1);
        guild_create_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_create_msg.Length() + 1);
        Client_SendEncoded(player, AC_NEW, OP_GUILD, guild_create_msg);
        mysql_controls->file_cache->guilds_count++;
        return;
    }
    // .. GuildJoin: each new member costs the guild's bank 1000
    if (query_result->query_type == DbQueryType_GuildJoin)
    {
        if (GUI->myquery->RecordCount < 1)
            return;
        Player *other = players->GetById(EO_DecodeNumber(query_result->data.SubString(1, 2)));
        if (!other)
            return;
        if (other->guild_tag.Length() > 1)
            return;
        if (other->map_id != player->map_id)
            return;
        if (LowerCase(mysql_controls->Db_GetString("tag")) == LowerCase(other->guild_tag))
            return;
        if (mysql_controls->Db_GetInt("money") < 1000)
            return;
        String guild_join_tag = mysql_controls->Db_GetString("tag");
        String guild_join_name = mysql_controls->Db_GetString("name");
        String new_member_rank_name = mysql_controls->Db_GetString("rank9");
        int guild_join_money = mysql_controls->Db_GetInt("money") - 1000;
        mysql_controls->Mysql_SubmitExec_FromCallback(0,
            "UPDATE endl_guilds SET money = " + IntToStr(guild_join_money) + " WHERE tag = '" + player->guild_tag +
            "'");
        other->guild_tag = guild_join_tag;
        other->guild_name = guild_join_name;
        other->guild_rank_name = new_member_rank_name;
        other->guild_rank_id = GUILD_RANK_NEW_MEMBER;
        other->guild_inviter_id = -1;
        player->guild_inviter_id = -1;
        String guild_join_msg = EO_EncodeNumber(player->player_id, 2);
        guild_join_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_join_msg.Length() + 1);
        guild_join_msg.Insert(guild_join_tag, guild_join_msg.Length() + 1);
        guild_join_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_join_msg.Length() + 1);
        guild_join_msg.Insert(guild_join_name, guild_join_msg.Length() + 1);
        guild_join_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_join_msg.Length() + 1);
        guild_join_msg.Insert(new_member_rank_name, guild_join_msg.Length() + 1);
        guild_join_msg.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), guild_join_msg.Length() + 1);
        Client_SendEncoded(other, AC_UPDATE, OP_GUILD, guild_join_msg);
        Client_SendEncoded(player, AC_RESULT, OP_GUILD, EO_EncodeNumber(GuildReply_Accepted, 2));
        return;
    }
    // .. GuildTopList: the top guilds again
    if (query_result->query_type == DbQueryType_GuildTopList)
    {
        for (vector<TopGuild *>::iterator it = mysql_controls->file_cache->top_guilds.begin();
            it != mysql_controls->file_cache->top_guilds.end();)
        {
            TopGuild *entry = *it;
            it = mysql_controls->file_cache->top_guilds.erase(it);
            delete entry;
        }
        mysql_controls->LoadCachedGuilds();
    }
}

// Something happened to the player ( an event and its value ): each of their quests checks its rules; done ones leave.
void Packets::Player_FireQuestTriggers(Player *player, int event, int value)
{
    for (vector<PlayerQuest>::iterator iter = player->quest_trackers.begin(); iter != player->quest_trackers.end(); )
    {
        QuestState *state = quest_engine->GetState(iter->quest_id, iter->state_index);
        if (!state)
            iter = player->quest_trackers.erase(iter);
        else
        {
            Player_EvaluateQuestRules(player, iter, state, event, value);
            if (iter->done)
                iter = player->quest_trackers.erase(iter);
            else
                iter++;
        }
    }
}

// The rules of the quest's state against an event: the first that holds moves the quest to its goto state.
void Packets::Player_EvaluateQuestRules(Player *player, PlayerQuest *tracker, QuestState *state, int event, int arg)
{
    vector<QuestRule *>::iterator iter;
    int index = -1;
    for (iter = state->rules.begin(); iter != state->rules.end(); iter++)
    {
        index++;
        if ((*iter)->rule != event && event != QuestCondition_AnyEvent)
            continue;
        // done the quest its number of times today
        if ((*iter)->rule == QuestCondition_DoneDaily &&
            quest_counters->GetCompletionCount(player->name, tracker->quest_id) >= (*iter)->args[0])
        {
            tracker->state_index = (*iter)->goto_state_index;
            Player_ApplyQuestActions(player, tracker, 1);
            return;
        }
        // .. any event: Always, GotItems ( the count so far is kept ) and LostItems
        if (event == QuestCondition_AnyEvent)
        {
            int item_id = (*iter)->args[0];
            int item_goal = (*iter)->args[1];
            if ((*iter)->rule == QuestCondition_Always)
            {
                tracker->state_index = (*iter)->goto_state_index;
                Player_ApplyQuestActions(player, tracker, 1);
            }
            if ((*iter)->rule == QuestCondition_GotItems)
            {
                int amount = players->GetItemAmount(player, item_id);
                if (amount >= 0)
                    tracker->counters[index] = amount;
                else
                    tracker->counters[index] = 0;
                if (amount >= item_goal)
                {
                    tracker->state_index = (*iter)->goto_state_index;
                    Player_ApplyQuestActions(player, tracker, 1);
                    return;
                }
                continue;
            }
            if ((*iter)->rule == QuestCondition_LostItems)
            {
                if (players->GetItemAmount(player, item_id) < item_goal)
                {
                    tracker->state_index = (*iter)->goto_state_index;
                    Player_ApplyQuestActions(player, tracker, 1);
                    return;
                }
                continue;
            }
        }
        // .. a kill counts in the rule's own counter; the first 5 rules have one
        if ((*iter)->rule == QuestCondition_KilledNpcs)
        {
            if (index > 4)
                continue;
            int npc_id = (*iter)->args[0];
            int npc_kill_goal = (*iter)->args[1];
            if (arg != npc_id)
                continue;
            tracker->counters[index]++;
            if (tracker->counters[index] >= npc_kill_goal)
            {
                tracker->state_index = (*iter)->goto_state_index;
                Player_ApplyQuestActions(player, tracker, 1);
                return;
            }
        }
        if ((*iter)->rule == QuestCondition_KilledPlayers)
        {
            if (index > 4)
                continue;
            int player_kill_goal = (*iter)->args[0];
            tracker->counters[index]++;
            if (tracker->counters[index] >= player_kill_goal)
            {
                tracker->state_index = (*iter)->goto_state_index;
                Player_ApplyQuestActions(player, tracker, 1);
                return;
            }
        }
        // .. standing on a tile, entering a map, leaving one
        if ((*iter)->rule == QuestCondition_EnterCoord)
        {
            int goal_map = (*iter)->args[0];
            int goal_x = (*iter)->args[1];
            int goal_y = (*iter)->args[2];
            if (player->map_id == goal_map && player->x == goal_x && player->y == goal_y)
            {
                tracker->state_index = (*iter)->goto_state_index;
                Player_ApplyQuestActions(player, tracker, 1);
                return;
            }
        }
        if ((*iter)->rule == QuestCondition_EnterMap && player->map_id == (*iter)->args[0])
        {
            tracker->state_index = (*iter)->goto_state_index;
            Player_ApplyQuestActions(player, tracker, 1);
            return;
        }
        if ((*iter)->rule == QuestCondition_LeaveMap && (*iter)->args[0] == arg)
        {
            tracker->state_index = (*iter)->goto_state_index;
            Player_ApplyQuestActions(player, tracker, 1);
            return;
        }
    }
}

// With run_actions the quest's state begins: its counters start over, its actions run and its rules are checked again.
void Packets::Player_ApplyQuestActions(Player *player, PlayerQuest *tracker, char run_actions)
{
    if (run_actions)
    {
        for (int i = 0; i < QUEST_COUNTER_SLOTS; i++)
            tracker->counters[i] = 0;
        QuestState *state = quest_engine->GetState(tracker->quest_id, tracker->state_index);
        if (!state)
        {
            tracker->done = 1;
            return;
        }
        for (vector<QuestAction *>::iterator iter = state->actions.begin(); iter != state->actions.end(); iter++)
        {
            if ((*iter)->action == QuestActionId_SetMap)
            {
                int target_map = (*iter)->args[0];
                MapCoord coords;
                coords.x = (*iter)->args[1];
                coords.y = (*iter)->args[2];
                Player_Warp(player, target_map, coords, WarpEffect_None, 1);
            }
            // .. GiveItem and RemoveItem: one when the file gives no amount
            if ((*iter)->action == QuestActionId_GiveItem)
            {
                unsigned int give_item = (*iter)->args[0];
                int give_amount = (*iter)->args[1];
                if ((*iter)->data[1] == "")
                    give_amount = 1;
                if (give_amount > 0)
                {
                    players->Player_AddItem(player, give_item, give_amount);
                    player->weight_current += give_amount * GUI->item_values->GetWeight(give_item);
                    if (player->weight_current < 0)
                        player->weight_current = 0;
                    int give_weight = player->weight_current;
                    if (give_weight > WEIGHT_DISPLAY_MAX)
                        give_weight = WEIGHT_DISPLAY_MAX;
                    String give_reply = EO_EncodeNumber(give_item, 2);
                    give_reply.Insert(EO_EncodeNumber(give_amount, 3), give_reply.Length() + 1);
                    give_reply.Insert(EO_EncodeNumber(give_weight, 1), give_reply.Length() + 1);
                    Client_SendEncoded(player, AC_GIVE, OP_ITEM, give_reply);
                }
            }
            if ((*iter)->action == QuestActionId_RemoveItem)
            {
                unsigned int take_item = (*iter)->args[0];
                int take_amount = (*iter)->args[1];
                if ((*iter)->data[1] == "")
                    take_amount = 1;
                if (take_amount > 0)
                {
                    players->Player_RemoveItemNoQuestRules(player, take_item, take_amount);
                    player->weight_current -= player->item_change_count * GUI->item_values->GetWeight(take_item);
                    if (player->weight_current < 0)
                        player->weight_current = 0;
                    int take_weight = player->weight_current;
                    if (take_weight > WEIGHT_DISPLAY_MAX)
                        take_weight = WEIGHT_DISPLAY_MAX;
                    String take_reply = EO_EncodeNumber(take_item, 2);
                    take_reply.Insert(EO_EncodeNumber(player->item_change_remaining, 4), take_reply.Length() + 1);
                    take_reply.Insert(EO_EncodeNumber(take_weight, 1), take_reply.Length() + 1);
                    Client_SendEncoded(player, AC_REMOVE, OP_ITEM, take_reply);
                }
            }
            // counts a completion for today and ends the quest
            if ((*iter)->action == QuestActionId_ResetDaily)
            {
                quest_counters->RecordCompletion(player->name, tracker->quest_id);
                tracker->done = 1;
                return;
            }
            // End also puts it in the player's history once; Reset only ends it
            if ((*iter)->action == QuestActionId_End || (*iter)->action == QuestActionId_Reset)
            {
                if ((*iter)->action == QuestActionId_End)
                {
                    char is_new = 1;
                    for (vector<PlayerQuest>::iterator it = player->quest_history.begin();
                        it != player->quest_history.end(); it++)
                    {
                        if (it->quest_id == tracker->quest_id)
                            is_new = 0;
                    }
                    if (is_new)
                        player->quest_history.insert(player->quest_history.end(), *tracker);
                }
                tracker->done = 1;
                return;
            }
            if ((*iter)->action == QuestActionId_SetClass)
            {
                int class_id = (*iter)->args[0];
                player->class_id = class_id;
                player->CalculateAdjStats();
                Player_CalculateStats(player);
                player->CalculateHP_TP_SP();
                String class_reply = EO_EncodeNumber(player->class_id, 2);
                class_reply.Insert(EO_EncodeNumber(player->adj_strength, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->adj_intelligence, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->adj_wisdom, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->adj_agility, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->adj_constitution, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->adj_charisma, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->max_hp, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->max_tp, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->max_sp, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->weight_max, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->min_damage, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->max_damage, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->accuracy, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->evasion, 2), class_reply.Length() + 1);
                class_reply.Insert(EO_EncodeNumber(player->armor, 2), class_reply.Length() + 1);
                Client_SendEncoded(player, AC_LIST, OP_GAMEBAR, class_reply);
            }
            if ((*iter)->action == QuestActionId_PlayMusic)
                Client_SendEncoded(player, AC_SET, OP_MUSIC,
                    EO_EncodeNumber((*iter)->args[0], 1));
            if ((*iter)->action == QuestActionId_PlaySound)
                Client_SendEncoded(player, AC_SET, OP_SOUND,
                    EO_EncodeNumber((*iter)->args[0], 1));
            if ((*iter)->action == QuestActionId_ShowHint)
                Client_SendEncoded(player, AC_OPEN, OP_SYSTEM, (*iter)->data[0]);
            if ((*iter)->action == QuestActionId_GiveExp)
            {
                player->experience += (*iter)->args[0];
                int level = players->Player_TryLevelUp(player);
                if (level > 0)
                    Server_BroadcastNearby(player, AC_CONFIRM, OP_ITEM,
                        EO_EncodeNumber(player->player_id, 2));
                String exp_gain_reply = EO_EncodeNumber(player->experience, 4);
                exp_gain_reply.Insert(EO_EncodeNumber(player->karma + 1000, 2), exp_gain_reply.Length() + 1);
                exp_gain_reply.Insert(EO_EncodeNumber(level, 1), exp_gain_reply.Length() + 1);
                exp_gain_reply.Insert(EO_EncodeNumber(player->stat_points, 2), exp_gain_reply.Length() + 1);
                exp_gain_reply.Insert(EO_EncodeNumber(player->skill_points, 2), exp_gain_reply.Length() + 1);
                Client_SendEncoded(player, AC_RESULT, OP_GAMEBAR, exp_gain_reply);
            }
            if ((*iter)->action == QuestActionId_RemoveExp)
            {
                player->experience -= (*iter)->args[0];
                String exp_loss_reply = EO_EncodeNumber(player->experience, 4);
                exp_loss_reply.Insert(EO_EncodeNumber(player->karma + 1000, 2), exp_loss_reply.Length() + 1);
                Client_SendEncoded(player, AC_RESULT, OP_GAMEBAR, exp_loss_reply);
            }
            // karma stays within -1000 and 1000; the reply carries the experience
            if ((*iter)->action == QuestActionId_GiveKarma)
            {
                player->karma += (*iter)->args[0];
                if (player->karma > 1000)
                    player->karma = 1000;
                String karma_gain_reply = EO_EncodeNumber(player->experience, 4);
                karma_gain_reply.Insert(EO_EncodeNumber(player->karma + 1000, 2), karma_gain_reply.Length() + 1);
                Client_SendEncoded(player, AC_RESULT, OP_GAMEBAR, karma_gain_reply);
            }
            if ((*iter)->action == QuestActionId_RemoveKarma)
            {
                player->karma -= (*iter)->args[0];
                if (player->karma < -1000)
                    player->karma = -1000;
                String karma_loss_reply = EO_EncodeNumber(player->experience, 4);
                karma_loss_reply.Insert(EO_EncodeNumber(player->karma + 1000, 2), karma_loss_reply.Length() + 1);
                Client_SendEncoded(player, AC_RESULT, OP_GAMEBAR, karma_loss_reply);
            }
            if ((*iter)->action == QuestActionId_Quake)
            {
                String quake_packet = EO_EncodeNumber(1, 1);
                quake_packet.Insert(EO_EncodeNumber((*iter)->args[0], 1), quake_packet.Length() + 1);
                Server_BroadcastToMap(player->map_id, AC_DO, OP_EFFECT, quake_packet);
            }
            if ((*iter)->action == QuestActionId_EffectOnPlayer)
            {
                String effect_packet = EO_EncodeNumber(player->player_id, 2);
                effect_packet.Insert(EO_EncodeNumber((*iter)->args[0], 3), effect_packet.Length() + 1);
                Server_BroadcastNearby(player, AC_SET, OP_EFFECT, effect_packet);
                Client_SendEncoded(player, AC_SET, OP_EFFECT, effect_packet);
            }
        }
    }
    if (run_actions)
        Player_FireQuestTriggers(player, QuestCondition_AnyEvent, 0);
    return;
}

// Builds the account's characters ( 3 at most ) from the query's rows, with their stats, then sends the reply.
void Packets::Login_SendCharacterList(Player *player, unsigned char action, unsigned char family, String data)
{
    GUI->myquery->First();
    data.Insert(EO_EncodeNumber(mysql_controls->GetResultCount(), 1), data.Length() + 1);
    data.Insert(EO_EncodeNumber(0, 1), data.Length() + 1);
    data.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
    for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
    {
        if (!player->character_slots[i])
            continue;
        Player *slot = player->character_slots[i];
        player->character_slots[i] = 0;
        delete slot;
    }
    int i = 0;
    while (!GUI->myquery->Eof && i < CHARACTER_SLOT_COUNT)
    {
        Player *newplayer = new Player(player->socket);
        newplayer->player_id = player->socket->SocketHandle;
        newplayer->character_id = mysql_controls->Db_GetInt("ident");
        newplayer->account_id = mysql_controls->Db_GetInt("ident_account");
        newplayer->class_id = mysql_controls->Db_GetInt("ident_class");
        newplayer->guild_rank_id = mysql_controls->Db_GetInt("ident_rank");
        newplayer->guild_tag = mysql_controls->Db_GetString("ident_guild");
        newplayer->home_id = mysql_controls->Db_GetInt("citizenship");
        newplayer->name = mysql_controls->Db_GetString("name");
        newplayer->partner_name = mysql_controls->Db_GetString("partner");
        newplayer->title = mysql_controls->Db_GetString("title");
        newplayer->guild_name = mysql_controls->Db_GetString("guild");
        newplayer->guild_rank_name = mysql_controls->Db_GetString("rank");
        newplayer->experience = mysql_controls->Db_GetInt("experience");
        newplayer->level = mysql_controls->Db_GetInt("level");
        newplayer->signup = mysql_controls->Db_GetString("signup");
        newplayer->gender = mysql_controls->Db_GetInt("gender");
        newplayer->hair_style = mysql_controls->Db_GetInt("hairmodal");
        newplayer->hair_color = mysql_controls->Db_GetInt("haircolor");
        newplayer->skin = mysql_controls->Db_GetInt("skincolor");
        newplayer->map_id = mysql_controls->Db_GetInt("nav_map");
        newplayer->x = mysql_controls->Db_GetInt("nav_x");
        newplayer->y = mysql_controls->Db_GetInt("nav_y");
        newplayer->direction = mysql_controls->Db_GetInt("nav_direction");
        newplayer->base_hp = mysql_controls->Db_GetInt("hp_max");
        newplayer->hp = mysql_controls->Db_GetInt("hp_now");
        newplayer->base_tp = mysql_controls->Db_GetInt("mp_max");
        newplayer->tp = mysql_controls->Db_GetInt("mp_now");
        newplayer->base_sp = mysql_controls->Db_GetInt("sp_max");
        newplayer->usage = mysql_controls->Db_GetInt("clientusge");
        newplayer->money_bank = mysql_controls->Db_GetInt("money_bank");
        newplayer->locker_bank = mysql_controls->Db_GetInt("locker_bank");
        newplayer->stat_points = mysql_controls->Db_GetInt("stat_points");
        newplayer->skill_points = mysql_controls->Db_GetInt("skill_points");
        newplayer->karma = mysql_controls->Db_GetInt("alignment_good");
        newplayer->base_strength = mysql_controls->Db_GetInt("stat_strenght");
        newplayer->base_wisdom = mysql_controls->Db_GetInt("stat_wisdom");
        newplayer->base_intelligence = mysql_controls->Db_GetInt("stat_intelligence");
        newplayer->base_agility = mysql_controls->Db_GetInt("stat_agility");
        newplayer->base_constitution = mysql_controls->Db_GetInt("stat_constitution");
        newplayer->base_charisma = mysql_controls->Db_GetInt("stat_charisma");
        newplayer->boots_item_id = mysql_controls->Db_GetInt("eq_boots");
        newplayer->accessory_item_id = mysql_controls->Db_GetInt("eq_pants");
        newplayer->gloves_item_id = mysql_controls->Db_GetInt("eq_gloves");
        newplayer->armor_item_id = mysql_controls->Db_GetInt("eq_armor");
        newplayer->belt_item_id = mysql_controls->Db_GetInt("eq_belt");
        newplayer->necklace_item_id = mysql_controls->Db_GetInt("eq_necklage");
        newplayer->hat_item_id = mysql_controls->Db_GetInt("eq_hat");
        newplayer->shield_item_id = mysql_controls->Db_GetInt("eq_shield");
        newplayer->weapon_item_id = mysql_controls->Db_GetInt("eq_weapon");
        newplayer->ring1_item_id = mysql_controls->Db_GetInt("eq_ring_l");
        newplayer->ring2_item_id = mysql_controls->Db_GetInt("eq_ring_r");
        newplayer->armlet1_item_id = mysql_controls->Db_GetInt("eq_armlet_l");
        newplayer->armlet2_item_id = mysql_controls->Db_GetInt("eq_armlet_r");
        newplayer->bracer1_item_id = mysql_controls->Db_GetInt("eq_bracer_l");
        newplayer->bracer2_item_id = mysql_controls->Db_GetInt("eq_bracer_r");
        for (int k = 0; k < ELEMENT_COUNT; k++)
            newplayer->element_resistances[k] = 0;
        newplayer->weight_current = 0;
        newplayer->min_damage = 0;
        newplayer->max_damage = 0;
        newplayer->accuracy = 0;
        newplayer->evasion = 0;
        newplayer->armor = 0;
        newplayer->class_min_damage = 0;
        newplayer->class_max_damage = 0;
        newplayer->class_accuracy = 0;
        newplayer->class_evasion = 0;
        newplayer->class_armor = 0;
        Player_ApplyEquipmentBonuses(newplayer);
        newplayer->CalculateAdjStats();
        Player_CalculateStats(newplayer);
        newplayer->CalculateHP_TP_SP();
        newplayer->on_chair = 0;
        newplayer->sitting = 0;
        // 1 = on a chair, 2 = on the floor
        if (mysql_controls->Db_GetInt("sitting") == 1)
            newplayer->on_chair = 1;
        if (mysql_controls->Db_GetInt("sitting") == 2)
            newplayer->sitting = 1;
        newplayer->admin_level = mysql_controls->Db_GetInt("privilege");
        newplayer->quest_cache = mysql_controls->Db_GetString("questcache");
        newplayer->quest_blob = mysql_controls->Db_GetString("questblob") + mysql_controls->Db_GetString("questblob2");
        // each list and its second column, joined again
        newplayer->invblob1 = mysql_controls->Db_GetString("invblob") + mysql_controls->Db_GetString("invblob2");
        newplayer->bankblob = mysql_controls->Db_GetString("invblob3") + mysql_controls->Db_GetString("invblob4");
        newplayer->skillblob = mysql_controls->Db_GetString("skillblob") + mysql_controls->Db_GetString("skillblob2");
        if (i < CHARACTER_SLOT_COUNT)
            player->character_slots[i++] = newplayer;
        data.Insert(mysql_controls->Db_GetString("name"), data.Length() + 1);
        data.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("ident"), 4), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("level"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("gender"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("hairmodal"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("haircolor"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("skincolor"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(mysql_controls->Db_GetInt("privilege"), 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(GUI->item_values->GetSpec1ForTypes(mysql_controls->Db_GetInt("eq_boots")), 2),
            data.Length() + 1);
        data.Insert(EO_EncodeNumber(GUI->item_values->GetSpec1ForTypes(mysql_controls->Db_GetInt("eq_armor")), 2),
            data.Length() + 1);
        data.Insert(EO_EncodeNumber(GUI->item_values->GetSpec1ForTypes(mysql_controls->Db_GetInt("eq_hat")), 2),
            data.Length() + 1);
        data.Insert(EO_EncodeNumber(GUI->item_values->GetSpec1ForTypes(mysql_controls->Db_GetInt("eq_shield")), 2),
            data.Length() + 1);
        data.Insert(EO_EncodeNumber(GUI->item_values->GetSpec1ForTypes(mysql_controls->Db_GetInt("eq_weapon")), 2),
            data.Length() + 1);
        data.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        GUI->myquery->Next();
    }
    Client_SendEncoded(player, action, family, data);
}

// The party as the client gets it: each member's id, whether they lead, their level, hp percent and name.
String Packets::Party_EncodeMemberList(Player *player)
{
    int i;
    if (!player->in_party)
        return "";
    String member_list = "";
    for (i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        Player *member = players->GetById(player->party_ids[i]);
        if (member)
        {
            int is_leader = 0;
            if (player->party_leader_id == member->player_id)
                is_leader = 1;
            member_list.Insert(EO_EncodeNumber(member->player_id, 2), member_list.Length() + 1);
            member_list.Insert(EO_EncodeNumber(is_leader, 1), member_list.Length() + 1);
            member_list.Insert(EO_EncodeNumber(member->level, 1), member_list.Length() + 1);
            member_list.Insert(EO_EncodeNumber(member->HpPercent(), 1), member_list.Length() + 1);
            member_list.Insert(member->name, member_list.Length() + 1);
            member_list.Insert(EO_GetBreakByte(EO_BREAK_BYTE), member_list.Length() + 1);
        }
    }
    return member_list;
}

// What comes into view after a step: the players and npcs at the edge of their view, and the items there.
String Packets::Walk_BuildReply(Player *player)
{
    vector<Player *>::iterator iter;
    vector<Npc *>::iterator niter;
    String reply = "";
    try
    {
        for (iter = players->players.begin(); iter != players->players.end(); iter++)
        {
            if ((*iter)->map_id == player->map_id && Server_InViewRing(player->x, player->y, (*iter)->x, (*iter)->y))
                reply.Insert(EO_EncodeNumber((*iter)->player_id, 2), reply.Length() + 1);
        }
        reply.Insert(EO_GetBreakByte(EO_BREAK_BYTE), reply.Length() + 1);
        if (player->map_id > 0 && (int)map_control->maps.size() >= player->map_id)
        {
            for (niter = map_control->maps[player->map_id - 1].npc_list.begin();
                niter != map_control->maps[player->map_id - 1].npc_list.end(); niter++)
            {
                if (Server_InViewRing(player->x, player->y, (*niter)->x, (*niter)->y))
                    reply.Insert(EO_EncodeNumber((*niter)->index, 1), reply.Length() + 1);
            }
        }
        reply.Insert(EO_GetBreakByte(EO_BREAK_BYTE), reply.Length() + 1);
        if (player->map_id > 0 && (int)map_control->maps.size() >= player->map_id)
        {
            vector<ItemObj *>::iterator it;
            for (it = map_control->maps[player->map_id - 1].ground_items.begin();
                it != map_control->maps[player->map_id - 1].ground_items.end(); it++)
            {
                if (!Server_InItemViewRing(player->x, player->y, (*it)->x, (*it)->y))
                    continue;
                reply.Insert(EO_EncodeNumber((*it)->index, 2), reply.Length() + 1);
                reply.Insert(EO_EncodeNumber((*it)->item_id, 2), reply.Length() + 1);
                reply.Insert(EO_EncodeNumber((*it)->x, 1), reply.Length() + 1);
                reply.Insert(EO_EncodeNumber((*it)->y, 1), reply.Length() + 1);
                reply.Insert(EO_EncodeNumber((*it)->amount, 3), reply.Length() + 1);
            }
        }
    }
    catch (...)
    {
        reply = "";
    }
    return reply;
}

// Everything in view: the players, the living npcs and the items on the ground.
String Packets::Refresh_BuildReply(Player *player)
{
    unsigned int count;
    String data = EO_GetBreakByte(EO_BREAK_BYTE);
    count = 0;
    try
    {
        vector<Player *>::iterator it;
        for (it = players->players.begin(); it != players->players.end(); it++)
        {
            if ((*it)->map_id != player->map_id)
                continue;
            if (!Server_InViewRange(player->x, player->y, (*it)->x, (*it)->y))
                continue;
            count++;
            data.Insert((*it)->name, data.Length() + 1);
            data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->player_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->map_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->x, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->y, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->direction, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->class_id, 1), data.Length() + 1);
            data.Insert((*it)->guild_tag, data.Length() + 1);
            if ((*it)->guild_tag.Length() == 2)
                data.Insert(" ", data.Length() + 1);
            if ((*it)->guild_tag.Length() == 1)
                data.Insert("  ", data.Length() + 1);
            if (!(*it)->guild_tag.Length())
                data.Insert("   ", data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->level, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->gender, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->hair_style, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->hair_color, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->skin, 1), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->max_hp, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->hp, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->max_tp, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->tp, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->boots_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->accessory_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->gloves_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->belt_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->armor_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->necklace_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->hat_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->shield_graphic_id, 2), data.Length() + 1);
            data.Insert(EO_EncodeNumber((*it)->weapon_graphic_id, 2), data.Length() + 1);
            unsigned int sit_state = 0;
            if ((*it)->on_chair)
                sit_state = 1;
            if ((*it)->sitting)
                sit_state = SIT_STATE_FLOOR;
            data.Insert(EO_EncodeNumber(sit_state, 1), data.Length() + 1);
            if ((*it)->hidden)
                data.Insert(EO_EncodeNumber(1, 1), data.Length() + 1);
            else
                data.Insert(EO_EncodeNumber(0, 1), data.Length() + 1);
            data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        }
        data.Insert(EO_EncodeNumber(count, 1), 1);
        if (player->map_id > 0 && (int)map_control->maps.size() >= player->map_id)
        {
            vector<Npc *>::iterator npc;
            for (npc = map_control->maps[player->map_id - 1].npc_list.begin();
                npc != map_control->maps[player->map_id - 1].npc_list.end(); npc++)
            {
                if (!(*npc)->alive)
                    continue;
                if (!Server_InViewRange(player->x, player->y, (*npc)->x, (*npc)->y))
                    continue;
                data.Insert(EO_EncodeNumber((*npc)->index, 1), data.Length() + 1);
                data.Insert(EO_EncodeNumber((*npc)->id, 2), data.Length() + 1);
                // a flagged bot sees each npc up to a tile off, a new offset each minute
                if (!player->cheater_flag)
                {
                    data.Insert(EO_EncodeNumber((*npc)->x, 1), data.Length() + 1);
                    data.Insert(EO_EncodeNumber((*npc)->y, 1), data.Length() + 1);
                }
                else
                {
                    int cheat_x = (*npc)->x + cheat_offset_x - 1;
                    int cheat_y = (*npc)->y + cheat_offset_y - 1;
                    if (cheat_x < 1)
                        cheat_x = 0;
                    if (cheat_y < 1)
                        cheat_y = 0;
                    data.Insert(EO_EncodeNumber(cheat_x, 1), data.Length() + 1);
                    data.Insert(EO_EncodeNumber(cheat_y, 1), data.Length() + 1);
                }
                data.Insert(EO_EncodeNumber((*npc)->direction, 1), data.Length() + 1);
            }
        }
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        if (player->map_id > 0 && (int)map_control->maps.size() >= player->map_id)
        {
            vector<ItemObj *>::iterator ground_item;
            for (ground_item = map_control->maps[player->map_id - 1].ground_items.begin();
                ground_item != map_control->maps[player->map_id - 1].ground_items.end(); ground_item++)
            {
                if (!Server_InViewRange(player->x, player->y, (*ground_item)->x, (*ground_item)->y))
                    continue;
                data.Insert(EO_EncodeNumber((*ground_item)->index, 2), data.Length() + 1);
                data.Insert(EO_EncodeNumber((*ground_item)->item_id, 2), data.Length() + 1);
                data.Insert(EO_EncodeNumber((*ground_item)->x, 1), data.Length() + 1);
                data.Insert(EO_EncodeNumber((*ground_item)->y, 1), data.Length() + 1);
                data.Insert(EO_EncodeNumber((*ground_item)->amount, 3), data.Length() + 1);
            }
        }
    }
    catch (...)
    {
        data = "NO";
    }
    return data;
}

// A player as the others see them: name, id, map spot, looks, guild tag, sitting, hidden, and the warp effect.
String Packets::Player_SerializeAvatar(Player *player, signed int warp_state)
{
    String out = player->name;
    out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
    try
    {
        out.Insert(EO_EncodeNumber(player->player_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->map_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->x, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->y, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->direction, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->class_id, 1), out.Length() + 1);
        out.Insert(player->guild_tag, out.Length() + 1);
        if (player->guild_tag.Length() == 2)
            out.Insert(" ", out.Length() + 1);
        if (player->guild_tag.Length() == 1)
            out.Insert("  ", out.Length() + 1);
        if (!player->guild_tag.Length())
            out.Insert("   ", out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->level, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->gender, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->hair_style, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->hair_color, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->skin, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->max_hp, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->hp, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->max_tp, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->tp, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->boots_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->accessory_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->gloves_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->belt_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->armor_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->necklace_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->hat_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->shield_graphic_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->weapon_graphic_id, 2), out.Length() + 1);
        unsigned int sit_state = 0;
        if (player->on_chair)
            sit_state = 1;
        if (player->sitting)
            sit_state = SIT_STATE_FLOOR;
        out.Insert(EO_EncodeNumber(sit_state, 1), out.Length() + 1);
        if (player->hidden)
            out.Insert(EO_EncodeNumber(1, 1), out.Length() + 1);
        else
            out.Insert(EO_EncodeNumber(0, 1), out.Length() + 1);
        if (warp_state < 0)
            out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        else
        {
            out.Insert(EO_EncodeNumber(warp_state, 1), out.Length() + 1);
            out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        }
    }
    catch (...)
    {
    }
    return out;
}

// A player's book: name, home, partner, title, guild and rank, their icon, then the quests they have done.
String Packets::Book_BuildReply(Player *player)
{
    String out = "";
    try
    {
        out.Insert(player->name, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(player->home_name, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(player->partner_name, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(player->title, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(player->guild_name, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(player->guild_rank_name, out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->player_id, 2), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->class_id, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->gender, 1), out.Length() + 1);
        out.Insert(EO_EncodeNumber(player->admin_level, 1), out.Length() + 1);
        // .. the icon: 1 = player, 4 = admin, 5 = high game master
        // 6, 9 and 10 the same in a party
        if (player->in_party)
        {
            if (player->admin_level > AdminLevel_LightGuide)
            {
                if (player->admin_level < AdminLevel_HighGameMaster)
                    out.Insert(EO_EncodeNumber(9, 1), out.Length() + 1);
                else
                    out.Insert(EO_EncodeNumber(10, 1), out.Length() + 1);
            }
            else
                out.Insert(EO_EncodeNumber(6, 1), out.Length() + 1);
        }
        else if (player->admin_level > AdminLevel_LightGuide)
        {
            if (player->admin_level < AdminLevel_HighGameMaster)
                out.Insert(EO_EncodeNumber(4, 1), out.Length() + 1);
            else
                out.Insert(EO_EncodeNumber(5, 1), out.Length() + 1);
        }
        else
            out.Insert(EO_EncodeNumber(1, 1), out.Length() + 1);
        out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        vector<PlayerQuest>::iterator it;
        for (it = player->quest_history.begin(); it != player->quest_history.end(); it++)
        {
            out.Insert(quest_engine->GetQuestName(it->quest_id), out.Length() + 1);
            out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
        }
    }
    catch (...)
    {
    }
    return out;
}

// A player's paperdoll: the same as their book, with their equipment and their icon.
String Packets::Paperdoll_BuildReply(Player *player)
{
    String data = "";
    try
    {
        data.Insert(player->name, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(player->home_name, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(player->partner_name, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(player->title, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(player->guild_name, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(player->guild_rank_name, data.Length() + 1);
        data.Insert(EO_GetBreakByte(EO_BREAK_BYTE), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->player_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->class_id, 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->gender, 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->admin_level, 1), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->boots_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->accessory_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->gloves_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->belt_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->armor_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->necklace_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->hat_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->shield_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->weapon_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->ring1_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->ring2_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->armlet1_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->armlet2_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->bracer1_item_id, 2), data.Length() + 1);
        data.Insert(EO_EncodeNumber(player->bracer2_item_id, 2), data.Length() + 1);
        // .. the icon: 1 = player, 4 = admin, 5 = high game master
        // 6, 9 and 10 the same in a party
        if (player->in_party)
        {
            if (player->admin_level > AdminLevel_LightGuide)
            {
                if (player->admin_level < AdminLevel_HighGameMaster)
                    data.Insert(EO_EncodeNumber(9, 1), data.Length() + 1);
                else
                    data.Insert(EO_EncodeNumber(10, 1), data.Length() + 1);
            }
            else
                data.Insert(EO_EncodeNumber(6, 1), data.Length() + 1);
        }
        else if (player->admin_level > AdminLevel_LightGuide)
        {
            if (player->admin_level < AdminLevel_HighGameMaster)
                data.Insert(EO_EncodeNumber(4, 1), data.Length() + 1);
            else
                data.Insert(EO_EncodeNumber(5, 1), data.Length() + 1);
        }
        else
            data.Insert(EO_EncodeNumber(1, 1), data.Length() + 1);
    }
    catch (...)
    {
    }
    return data;
}

// The names of the players online ( not the hidden ); with 25 or more the list is kept for 40 seconds.
String Packets::Server_BuildOnlineNames()
{
    int count;
    if (online_names_ttl < 1)
    {
        String names = EO_GetBreakByte(EO_BREAK_BYTE);
        count = 0;
        for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
        {
            if (!(*it)->logged_in)
                continue;
            if ((*it)->hide_online)
                continue;
            names.Insert((*it)->name, names.Length() + 1);
            names.Insert(EO_GetBreakByte(EO_BREAK_BYTE), names.Length() + 1);
            count++;
        }
        names.Insert(EO_EncodeNumber(count, 2), 1);
        if (count > 24)
        {
            online_names_cache = names;
            online_names_ttl = 4;
        }
        return names;
    }
    else
        return online_names_cache;
}

// The players online with their title, level, experience, gender and admin level.
String Packets::Message_BuildPlayerList()
{
    unsigned int count;
    String names = EO_GetBreakByte(EO_BREAK_BYTE);
    count = 0;
    for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
    {
        if (!(*it)->logged_in)
            continue;
        if ((*it)->hide_online)
            continue;
        names.Insert((*it)->name, names.Length() + 1);
        names.Insert(EO_GetBreakByte(EO_BREAK_BYTE), names.Length() + 1);
        names.Insert((*it)->title, names.Length() + 1);
        names.Insert(EO_GetBreakByte(EO_BREAK_BYTE), names.Length() + 1);
        names.Insert(EO_EncodeNumber((*it)->level, 1), names.Length() + 1);
        names.Insert(EO_EncodeNumber((*it)->experience, 4), names.Length() + 1);
        names.Insert(EO_EncodeNumber((*it)->gender, 1), names.Length() + 1);
        names.Insert(EO_EncodeNumber((*it)->admin_level, 1), names.Length() + 1);
        names.Insert(EO_GetBreakByte(EO_BREAK_BYTE), names.Length() + 1);
        count++;
    }
    names.Insert(EO_EncodeNumber(count, 2), 1);
    return names;
}

// The online list: each player's name, title, level, icon, class and guild tag; with 25 or more it is kept 40 seconds.
String Packets::Server_BuildOnlineList()
{
    int count;
    if (online_list_ttl < 1)
    {
        String list = EO_GetBreakByte(EO_BREAK_BYTE);
        count = 0;
        for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
        {
            if (!(*it)->logged_in)
                continue;
            if ((*it)->hide_online)
                continue;
            list.Insert((*it)->name, list.Length() + 1);
            list.Insert(EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            list.Insert((*it)->title, list.Length() + 1);
            list.Insert(EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            list.Insert(EO_EncodeNumber((*it)->level, 1), list.Length() + 1);
            if ((*it)->in_party)
            {
                if ((*it)->admin_level > AdminLevel_LightGuide)
                {
                    if ((*it)->admin_level < AdminLevel_HighGameMaster)
                        list.Insert(EO_EncodeNumber(9, 1), list.Length() + 1);
                    else
                        list.Insert(EO_EncodeNumber(10, 1), list.Length() + 1);
                }
                else
                    list.Insert(EO_EncodeNumber(6, 1), list.Length() + 1);
            }
            else if ((*it)->admin_level > AdminLevel_LightGuide)
            {
                if ((*it)->admin_level < AdminLevel_HighGameMaster)
                    list.Insert(EO_EncodeNumber(4, 1), list.Length() + 1);
                else
                    list.Insert(EO_EncodeNumber(5, 1), list.Length() + 1);
            }
            else
                list.Insert(EO_EncodeNumber(1, 1), list.Length() + 1);
            list.Insert(EO_EncodeNumber((*it)->class_id, 1), list.Length() + 1);
            list.Insert((*it)->guild_tag, list.Length() + 1);
            if ((*it)->guild_tag.Length() == 2)
                list.Insert(" ", list.Length() + 1);
            if ((*it)->guild_tag.Length() == 1)
                list.Insert("  ", list.Length() + 1);
            if (!(*it)->guild_tag.Length())
                list.Insert("   ", list.Length() + 1);
            list.Insert(EO_GetBreakByte(EO_BREAK_BYTE), list.Length() + 1);
            count++;
        }
        list.Insert(EO_EncodeNumber(count, 2), 1);
        if (count >= 25)
        {
            online_list_cache = list;
            online_list_ttl = 4;
        }
        return list;
    }
    else
        return online_list_cache;
}

// A living npc by its index on the player's map: its index, id, spot and direction, or "".
String Packets::NpcRange_Lookup(Player *player, int npc_index)
{
    vector<Npc *>::iterator iter;
    String fragment = "";
    try
    {
        if (player->map_id > 0 && (int)map_control->maps.size() >= player->map_id)
        {
            for (iter = map_control->maps[player->map_id - 1].npc_list.begin();
                iter != map_control->maps[player->map_id - 1].npc_list.end(); iter++)
            {
                if ((*iter)->index == npc_index && (*iter)->alive)
                {
                    fragment.Insert(EO_EncodeNumber((*iter)->index, 1), fragment.Length() + 1);
                    fragment.Insert(EO_EncodeNumber((*iter)->id, 2), fragment.Length() + 1);
                    // a flagged bot sees it up to a tile off
                    if (!player->cheater_flag)
                    {
                        fragment.Insert(EO_EncodeNumber((*iter)->x, 1), fragment.Length() + 1);
                        fragment.Insert(EO_EncodeNumber((*iter)->y, 1), fragment.Length() + 1);
                    }
                    else
                    {
                        int cheat_x = (*iter)->x + cheat_offset_x - 1;
                        int cheat_y = (*iter)->y + cheat_offset_y - 1;
                        if (cheat_x < 1)
                            cheat_x = 0;
                        if (cheat_y < 1)
                            cheat_y = 0;
                        fragment.Insert(EO_EncodeNumber(cheat_x, 1), fragment.Length() + 1);
                        fragment.Insert(EO_EncodeNumber(cheat_y, 1), fragment.Length() + 1);
                    }
                    fragment.Insert(EO_EncodeNumber((*iter)->direction, 1), fragment.Length() + 1);
                    break;
                }
            }
        }
    }
    catch (...)
    {
    }
    return fragment;
}

// Answers a new connection's init packet: the hdid, the bans, then the client's version.
// true  = accepted ( the init ok reply is sent )
// false = refused ( ban or version reply ), or not an init packet
bool Packets::Server_HandleInit(Player *player, String data)
{
    if (data.Length() > 8)
    {
        if (data.Length() < 42)
        {
            // .. the init packet: 255,255, the challenge ( 3 ), the version ( 3 ), 112, the hdid's length and the hdid
            if (EO_DecodeByte((unsigned char)data[1]) == 255)
            {
                if (EO_DecodeByte((unsigned char)data[2]) == 255)
                {
                    if (data.Length() > 10)
                    {
                        int hdid_len = EO_DecodeNumber(String(data[10]));
                        player->hdid = data.SubString(11, data.Length() - 10);
                        if (player->hdid.Length() != hdid_len)
                        {
                            player->socket->SendText(Server_BuildInitBanReply());
                            return false;
                        }
                        if (banned->IsBanned(player->remote_ip, player->hdid))
                        {
                            player->socket->SendText(Server_BuildInitBanReply());
                            return false;
                        }
                    }
                    int challenge_response = Server_HashChallenge(EO_DecodeNumber(data.SubString(3, 3)));
                    // .. the client's version ( major, minor, patch ): each part at least the server's, then 112
                    int major = EO_DecodeNumber(String(data[6]));
                    int minor = EO_DecodeNumber(String(data[7]));
                    int patch = EO_DecodeNumber(String(data[8]));
                    int magic = EO_DecodeNumber(String(data[9]));
                    if (major >= version_major && minor >= version_minor && patch >= version_patch && magic == 112)
                    {
                        if (major < 10)
                        {
                            // a server over its maxconnections drops the client after the reply
                            if (GUI->server->Socket->ActiveConnections > settings->GetMaxConnections())
                                player->remove_timer = 7;
                            else
                                player->remove_timer = -1;
                            player->challenge_response = challenge_response;
                            player->initialized = true;
                            player->socket->SendText(Server_BuildInitOkReply(player));
                            return true;
                        }
                    }
                    else
                    {
                        player->socket->SendText(Server_BuildInitVersionReply());
                        return false;
                    }
                }
            }
        }
    }
    // anything else puts the ip on the login block list
    logins->AddLogin(player->remote_ip);
    return false;
}

// The init reply that accepts a client: code 2, the sequence start, the multiples, the id and the challenge answer.
String Packets::Server_BuildInitOkReply(Player *player)
{
    int total = ping_history[0] + 13;
    int seq1 = total / 7;
    int seq2 = total % 7;
    String out = EO_GetBreakByte(EO_BREAK_BYTE);
    out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
    out.Insert(EO_GetBreakByte(2), out.Length() + 1);
    out.Insert(EO_GetBreakByte(seq1), out.Length() + 1);
    out.Insert(EO_GetBreakByte(seq2), out.Length() + 1);
    out.Insert(EO_GetBreakByte(player->server_encryption_multiple), out.Length() + 1);
    out.Insert(EO_GetBreakByte(player->client_encryption_multiple), out.Length() + 1);
    out.Insert(EO_EncodeNumber(player->player_id, 2), out.Length() + 1);
    out.Insert(EO_EncodeNumber(player->challenge_response, 3), out.Length() + 1);
    out.Insert(EO_EncodeNumber(out.Length(), 2), 1);
    return out;
}

// The init reply for a client of the wrong version: code 1 and the version the server wants.
String Packets::Server_BuildInitVersionReply()
{
    String out = EO_GetBreakByte(EO_BREAK_BYTE);
    out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
    out.Insert(EO_GetBreakByte(1), out.Length() + 1);
    out.Insert(EO_EncodeNumber(version_major, 1), out.Length() + 1);
    out.Insert(EO_EncodeNumber(version_minor, 1), out.Length() + 1);
    out.Insert(EO_EncodeNumber(version_patch, 1), out.Length() + 1);
    out.Insert(EO_EncodeNumber(out.Length(), 2), 1);
    return out;
}

// The init reply for a banned client: code 3, the ban's type and time.
String Packets::Server_BuildInitBanReply()
{
    String out = EO_GetBreakByte(EO_BREAK_BYTE);
    out.Insert(EO_GetBreakByte(EO_BREAK_BYTE), out.Length() + 1);
    out.Insert(EO_GetBreakByte(3), out.Length() + 1);
    out.Insert(EO_GetBreakByte(banned->GetBanType()), out.Length() + 1);
    out.Insert(EO_GetBreakByte(banned->GetBanTime()), out.Length() + 1);
    out.Insert(EO_EncodeNumber(out.Length(), 2), 1);
    return out;
}

// Sends data unencoded ( a file, a list ) after 255, 255 and its reply code.
// One past 62000 bytes is logged to error.log.
void Packets::Client_SendRaw(Player *client, String data, int reply_code)
{
    FILE *fp;
    if (data.Length() > 62000)
    {
        String msg = DateToStr(Now());
        msg.Insert(" ", msg.Length() + 1);
        msg.Insert(TimeToStr(Now()), msg.Length() + 1);
        msg.Insert(" EndlServ ", msg.Length() + 1);
        msg.Insert("Too large uncoded packet dropped: " + IntToStr(reply_code), msg.Length() + 1);
        msg.Insert("\n", msg.Length() + 1);
        fp = fopen("error.log", "a");
        fprintf(fp, "%s", msg.c_str());
        fclose(fp);
    }
    if (client->removing)
        return;
    if (!client->connected)
        return;
    String built = (char)EO_GetBreakByte(EO_BREAK_BYTE);
    built.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), built.Length() + 1);
    built.Insert((char)EO_GetBreakByte(reply_code), built.Length() + 1);
    built.Insert(data, built.Length() + 1);
    built.Insert(EO_EncodeNumber(built.Length(), 2), 1);
    client->socket->SendText(built);
}

// Sends unencoded data to every player on the map.
void Packets::Server_SendRawToMap(int map_id, String data, int reply_code)
{
    for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
    {
        if ((*it)->map_id != map_id)
            continue;
        if (!(*it)->logged_in)
            continue;
        if ((*it)->removing)
            continue;
        String packet = (char)EO_GetBreakByte(EO_BREAK_BYTE);
        packet.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), packet.Length() + 1);
        packet.Insert((char)EO_GetBreakByte(reply_code), packet.Length() + 1);
        packet.Insert(data, packet.Length() + 1);
        packet.Insert(EO_EncodeNumber(packet.Length(), 2), 1);
        (*it)->socket->SendText(packet);
    }
}

// To the player's party, not the player.
void Packets::Server_BroadcastToPartyExceptSelf(Player *player, unsigned char action, unsigned char family, String data)
{
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        if (player->party_ids[i] == player->player_id)
            continue;
        Player *member = players->GetById(player->party_ids[i]);
        if (member)
        {
            if (member->logged_in)
                Client_SendEncoded(member, action, family, data);
        }
    }
}

// To the player's whole party.
void Packets::Server_BroadcastToParty(Player *player, unsigned char action, unsigned char family, String data)
{
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        Player *member = players->GetById(player->party_ids[i]);
        if (member && member->logged_in)
            Client_SendEncoded(member, action, family, data);
    }
}

// To the members of the player's party on their map.
void Packets::Server_BroadcastToPartyOnMap(Player *player, unsigned char action, unsigned char family, String data)
{
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        Player *member = players->GetById(player->party_ids[i]);
        if (member)
        {
            if (player->map_id == member->map_id)
            {
                if (member->logged_in)
                    Client_SendEncoded(member, action, family, data);
            }
        }
    }
}

// To the other members of the player's guild.
void Packets::Guild_BroadcastToAll(Player *player, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->guild_tag == player->guild_tag)
        {
            if ((*player_iter)->player_id != player->player_id)
            {
                if ((*player_iter)->logged_in)
                    Client_SendEncoded(*player_iter, action, family, data);
            }
        }
    }
}

// To the other players next to a tile.
void Packets::Server_BroadcastAdjacent(Player *player, MapCoord coords, unsigned char action, unsigned char family,
    String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_id != player->map_id)
            continue;
        if (Coords_IsAdjacent(coords.x, coords.y, (*player_iter)->x, (*player_iter)->y))
        {
            if ((*player_iter)->logged_in)
            {
                if ((*player_iter)->player_id != player->player_id)
                    Client_SendEncoded(*player_iter, action, family, data);
            }
        }
    }
}

// To the players on the map, and to the admins anywhere.
void Packets::Server_BroadcastToMapAndAdmins(int map_id, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_id != map_id)
        {
            if ((*player_iter)->admin_level <= AdminLevel_Player)
                continue;
        }
        if ((*player_iter)->logged_in)
            Client_SendEncoded(*player_iter, action, family, data);
    }
}

// To the players on the map.
void Packets::Server_BroadcastToMap(int map_id, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_id == map_id)
        {
            if ((*player_iter)->logged_in)
                Client_SendEncoded(*player_iter, action, family, data);
        }
    }
}

// To the players who see a tile, but skip_id.
void Packets::Server_BroadcastNearTile(int skip_id, int map_id, MapCoord coord, unsigned char action,
    unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_id == map_id && (*player_iter)->player_id != skip_id &&
            Server_InViewRange(coord.x, coord.y, (*player_iter)->x, (*player_iter)->y))
            Client_SendEncoded(*player_iter, action, family, data);
    }
}

// To the other admins.
void Packets::Admin_BroadcastToOtherAdmins(Player *player, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->admin_level <= AdminLevel_Player)
            continue;
        if ((*player_iter)->logged_in)
        {
            if ((*player_iter)->player_id != player->player_id)
                Client_SendEncoded(*player_iter, action, family, data);
        }
    }
}

// To every admin.
void Packets::Admin_BroadcastToAll(unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->admin_level > AdminLevel_Player)
        {
            if ((*player_iter)->logged_in)
                Client_SendEncoded(*player_iter, action, family, data);
        }
    }
}

// To every player in the game.
void Packets::Server_BroadcastToAll(unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->logged_in)
            Client_SendEncoded(*player_iter, action, family, data);
    }
}

// To the other players with the world chat open.
void Packets::Server_BroadcastToGlobalChat(Player *player, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if (!(*player_iter)->global_chat)
            continue;
        if ((*player_iter)->logged_in)
        {
            if ((*player_iter)->player_id != player->player_id)
                Client_SendEncoded(*player_iter, action, family, data);
        }
    }
}

// To every player in the game but the sender.
void Packets::Server_BroadcastToAllExceptSelf(Player *player, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->logged_in)
        {
            if ((*player_iter)->player_id != player->player_id)
                Client_SendEncoded(*player_iter, action, family, data);
        }
    }
}

// To the other players who see the player.
void Packets::Server_BroadcastNearby(Player *player, unsigned char action, unsigned char family, String data)
{
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_id != player->map_id)
            continue;
        if (Server_InViewRangeReverse(player->x, player->y, (*player_iter)->x, (*player_iter)->y))
        {
            if ((*player_iter)->logged_in)
            {
                if ((*player_iter)->player_id != player->player_id)
                    Client_SendEncoded(*player_iter, action, family, data);
            }
        }
    }
}

// Sends a packet the protocol's way: action and family, interleaved and with the top bits flipped, then its length.
void Packets::Client_SendEncoded(Player *player, unsigned char action, unsigned char family, String data)
{
    FILE *fp;
    // a packet past 20000 bytes is logged to error.log, and still sent
    if (data.Length() > 20000)
    {
        String msg = DateToStr(Now());
        msg.Insert(" ", msg.Length() + 1);
        msg.Insert(TimeToStr(Now()), msg.Length() + 1);
        msg.Insert(" EndlServ ", msg.Length() + 1);
        msg.Insert("Too large encoded packet dropped: " + IntToStr(action) + "," + IntToStr(family), msg.Length() + 1);
        msg.Insert("\n", msg.Length() + 1);
        fp = fopen("error.log", "a");
        fprintf(fp, "%s", msg.c_str());
        fclose(fp);
    }
    if (!player->removing)
    {
        char family_byte = family;
        char action_byte = action;
        String packet = String(action_byte);
        packet.Insert(family_byte, packet.Length() + 1);
        packet.Insert(data, packet.Length() + 1);
        std::string range(packet.c_str());
        packet = EO_Encode_Interleave(player->server_encryption_multiple, range.begin(), range.end());
        int i = 1;
        while (packet.Length() >= i)
        {
            int byte_value = (unsigned char)packet[i];
            if (byte_value < 128)
                packet[i] += (char)128;
            if (byte_value > 128)
                packet[i] += (char)128;
            i++;
        }
        packet.Insert(EO_EncodeNumber(packet.Length(), 2), 1);
        Server_AddSentBytes(packet.Length());
        player->socket->SendText(packet);
    }
}

// A dead player goes back to their inn, the rescue spot when they have none, or the start up to level 3.
void Packets::Player_Respawn(Player *player)
{
    MapCoord coords;
    int map_id = GUI->inn_values->GetSpawnMap(player->home_id, player->level);
    if (map_id < 0)
    {
        map_id = settings->GetRescueMap();
        coords.x = settings->GetRescueX();
        coords.y = settings->GetRescueY();
    }
    else
    {
        coords.x = GUI->inn_values->GetSpawnX(player->home_id, player->level);
        coords.y = GUI->inn_values->GetSpawnY(player->home_id, player->level);
    }
    if (player->level <= 3)
    {
        map_id = settings->GetStartMap();
        coords.x = settings->GetStartX();
        coords.y = settings->GetStartY();
    }
    player->flush_queue = true;
    Player_Warp(player, map_id, coords, WarpEffect_None, 1);
}

// Sends the player to a map and spot: the same map at once, another map once the client has its file.
// do_leave takes them off their map right away
void Packets::Player_Warp(Player *player, int target_map, MapCoord coords, int warp_effect, char do_leave)
{
    // maps 80 and 81 take no warps
    if (target_map == 80 || target_map == 81)
        return;
    if (target_map < 1 && (int)map_control->maps.size() < target_map)
        return;
    if (map_control->maps[target_map - 1].width < 1 || map_control->maps[target_map - 1].height < 1)
        return;
    if (player->warp_effect < 0)
        player->session_id = random(50000) + 10000;
    int old_map = player->map_id;
    player->warp_effect = warp_effect;
    player->warp_pending = true;
    player->dead = false;
    player->warp_map = target_map;
    player->warp_x = coords.x;
    player->warp_y = coords.y;
    if (do_leave)
    {
        Server_BroadcastNearby(player, AC_DELETE, OP_PLAYER,
            EO_EncodeNumber(player->player_id, 2) + EO_EncodeNumber(player->warp_effect, 1));
        if (target_map > 0)
        {
            player->restore_map = target_map;
            player->restore_x = coords.x;
            player->restore_y = coords.y;
        }
        if (player->map_id > 0)
        {
            player->map_has_quakes = 0;
            player->map_has_hp_drain = 0;
            player->map_has_tp_drain = 0;
            player->map_has_spikes = 0;
            map_control->DecPlayerCount(player->map_id);
        }
        player->map_id = 0;
        player->x = 0;
        player->y = 0;
        player->idle_ticks = 0;
        player->trade_partner_id = -1;
        player->guild_inviter_id = -1;
        player->session_token = -1;
    }
    // .. the client answers with Warp Accept, or Warp Take for a map it lacks
    if (target_map == old_map)
    {
        String same_map_reply = EO_EncodeNumber(WarpType_Local, 1);
        same_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].map_id, 2),
            same_map_reply.Length() + 1);
        same_map_reply.Insert(EO_EncodeNumber(player->session_id, 2), same_map_reply.Length() + 1);
        Client_SendEncoded(player, AC_REQUEST, OP_MAP, same_map_reply);
    }
    else
    {
        String new_map_reply = EO_EncodeNumber(WarpType_MapSwitch, 1);
        new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].map_id, 2), new_map_reply.Length() + 1);
        new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].rid1, 2), new_map_reply.Length() + 1);
        new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].rid2, 2), new_map_reply.Length() + 1);
        new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].filesize, 3),
            new_map_reply.Length() + 1);
        new_map_reply.Insert(EO_EncodeNumber(player->session_id, 2), new_map_reply.Length() + 1);
        Client_SendEncoded(player, AC_REQUEST, OP_MAP, new_map_reply);
        Player_FireQuestTriggers(player, QuestCondition_LeaveMap, old_map);
    }
}

// 1 when the tile is a warp the player may take ( no key needed, their level enough ); it takes them.
char Packets::Player_CheckIdleWarp(Player *player, int x, int y)
{
    if (map_control->IsWarpTile(player->map_id, x, y))
    {
        MapCoord coords;
        coords.x = x;
        coords.y = y;
        if (map_control->GetDoorKeyAt(player->map_id, coords) < 2)
        {
            MapCoord dest;
            int target_map = map_control->GetWarpMap(player->map_id, x, y);
            int level_req = map_control->GetWarpLevelReq(player->map_id, x, y);
            dest.x = map_control->GetWarpX(player->map_id, x, y);
            dest.y = map_control->GetWarpY(player->map_id, x, y);
            if (player->level < level_req)
                return 0;
            player->flush_queue = true;
            Player_Warp(player, target_map, dest, WarpEffect_None, 0);
            return 1;
        }
    }
    return 0;
}

// The class's stats and the damage, accuracy, evasion and armor they give, by the class's stat group.
void Packets::Player_CalculateStats(Player *player)
{
    if (player->class_id < 1)
        return;
    ClassValue cls = GUI->class_values->GetByIndex(player->class_id - 1);
    player->adj_strength += cls.str;
    player->adj_intelligence += cls.intl;
    player->adj_wisdom += cls.wis;
    player->adj_agility += cls.agi;
    player->adj_constitution += cls.con;
    player->adj_charisma += cls.cha;
    player->min_damage -= player->class_min_damage;
    player->max_damage -= player->class_max_damage;
    player->accuracy -= player->class_accuracy;
    player->evasion -= player->class_evasion;
    player->armor -= player->class_armor;
    player->class_min_damage = 0;
    player->class_max_damage = 0;
    player->class_accuracy = 0;
    player->class_evasion = 0;
    player->class_armor = 0;
    // .. the four stat groups weigh strength, intelligence, wisdom, agility and constitution each their way
    if (!cls.stat_group)
    {
        player->class_min_damage += player->adj_strength / 3;
        player->class_max_damage += player->adj_strength / 3;
        player->class_accuracy += player->adj_agility / 3;
        player->class_evasion += player->adj_agility / 5;
        player->class_armor += player->adj_constitution / 4;
    }
    if (cls.stat_group == 1)
    {
        player->class_min_damage += player->adj_strength / 5;
        player->class_max_damage += player->adj_strength / 5;
        player->class_accuracy += player->adj_agility / 3;
        player->class_evasion += player->adj_agility / 3;
        player->class_armor += player->adj_constitution / 4;
    }
    if (cls.stat_group == 2)
    {
        player->class_min_damage += player->adj_intelligence / 3;
        player->class_max_damage += player->adj_intelligence / 3;
        player->class_accuracy += player->adj_wisdom / 3;
        player->class_evasion += player->adj_agility / 4;
        player->class_armor += player->adj_constitution / 5;
    }
    if (cls.stat_group == 3)
    {
        player->class_min_damage += player->adj_strength / 6;
        player->class_max_damage += player->adj_strength / 6;
        player->class_accuracy += player->adj_agility / 5;
        player->class_evasion += player->adj_agility / 4;
        player->class_armor += player->adj_constitution / 5;
    }
    player->min_damage += player->class_min_damage;
    player->max_damage += player->class_max_damage;
    player->accuracy += player->class_accuracy;
    player->evasion += player->class_evasion;
    player->armor += player->class_armor;
}

// Adds up the bonuses and weight of what the character wears; an item in the wrong slot comes off.
void Packets::Player_ApplyEquipmentBonuses(Player *player)
{
    player->equip_bonus_hp = 0;
    player->equip_bonus_tp = 0;
    player->equip_strength_bonus = 0;
    player->equip_wisdom_bonus = 0;
    player->equip_intelligence_bonus = 0;
    player->equip_agility_bonus = 0;
    player->equip_constitution_bonus = 0;
    player->equip_charisma_bonus = 0;
    if (GUI->item_values->GetType(player->boots_item_id) == EifItemType_Boots)
    {
        ItemValue *boots = GUI->item_values->GetByIndex(player->boots_item_id - 1);
        if (boots->element < ELEMENT_COUNT)
            player->element_resistances[boots->element] += boots->element_damage;
        player->boots_graphic_id = boots->spec1;
        player->weight_current += boots->weight;
        player->min_damage += boots->min_damage;
        player->max_damage += boots->max_damage;
        player->accuracy += boots->accuracy;
        player->evasion += boots->evade;
        player->armor += boots->armor;
        player->equip_bonus_hp += boots->hp;
        player->equip_bonus_tp += boots->tp;
        player->equip_strength_bonus += boots->strength;
        player->equip_wisdom_bonus += boots->wisdom;
        player->equip_intelligence_bonus += boots->intelligence;
        player->equip_agility_bonus += boots->agility;
        player->equip_constitution_bonus += boots->constitution;
        player->equip_charisma_bonus += boots->charisma;
    }
    else
    {
        player->boots_item_id = 0;
        player->boots_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->accessory_item_id) == EifItemType_Accessory)
    {
        ItemValue *accessory = GUI->item_values->GetByIndex(player->accessory_item_id - 1);
        if (accessory->element < ELEMENT_COUNT)
            player->element_resistances[accessory->element] += accessory->element_damage;
        player->accessory_graphic_id = accessory->spec1;
        player->weight_current += accessory->weight;
        player->min_damage += accessory->min_damage;
        player->max_damage += accessory->max_damage;
        player->accuracy += accessory->accuracy;
        player->evasion += accessory->evade;
        player->armor += accessory->armor;
        player->equip_bonus_hp += accessory->hp;
        player->equip_bonus_tp += accessory->tp;
        player->equip_strength_bonus += accessory->strength;
        player->equip_wisdom_bonus += accessory->wisdom;
        player->equip_intelligence_bonus += accessory->intelligence;
        player->equip_agility_bonus += accessory->agility;
        player->equip_constitution_bonus += accessory->constitution;
        player->equip_charisma_bonus += accessory->charisma;
    }
    else
    {
        player->accessory_item_id = 0;
        player->accessory_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->gloves_item_id) == EifItemType_Gloves)
    {
        ItemValue *gloves = GUI->item_values->GetByIndex(player->gloves_item_id - 1);
        if (gloves->element < ELEMENT_COUNT)
            player->element_resistances[gloves->element] += gloves->element_damage;
        player->gloves_graphic_id = gloves->spec1;
        player->weight_current += gloves->weight;
        player->min_damage += gloves->min_damage;
        player->max_damage += gloves->max_damage;
        player->accuracy += gloves->accuracy;
        player->evasion += gloves->evade;
        player->armor += gloves->armor;
        player->equip_bonus_hp += gloves->hp;
        player->equip_bonus_tp += gloves->tp;
        player->equip_strength_bonus += gloves->strength;
        player->equip_wisdom_bonus += gloves->wisdom;
        player->equip_intelligence_bonus += gloves->intelligence;
        player->equip_agility_bonus += gloves->agility;
        player->equip_constitution_bonus += gloves->constitution;
        player->equip_charisma_bonus += gloves->charisma;
    }
    else
    {
        player->gloves_item_id = 0;
        player->gloves_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->armor_item_id) == EifItemType_Armor)
    {
        ItemValue *armor = GUI->item_values->GetByIndex(player->armor_item_id - 1);
        if (armor->element < ELEMENT_COUNT)
            player->element_resistances[armor->element] += armor->element_damage;
        player->armor_graphic_id = armor->spec1;
        player->weight_current += armor->weight;
        player->min_damage += armor->min_damage;
        player->max_damage += armor->max_damage;
        player->accuracy += armor->accuracy;
        player->evasion += armor->evade;
        player->armor += armor->armor;
        player->equip_bonus_hp += armor->hp;
        player->equip_bonus_tp += armor->tp;
        player->equip_strength_bonus += armor->strength;
        player->equip_wisdom_bonus += armor->wisdom;
        player->equip_intelligence_bonus += armor->intelligence;
        player->equip_agility_bonus += armor->agility;
        player->equip_constitution_bonus += armor->constitution;
        player->equip_charisma_bonus += armor->charisma;
    }
    else
    {
        player->armor_item_id = 0;
        player->armor_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->belt_item_id) == EifItemType_Belt)
    {
        ItemValue *belt = GUI->item_values->GetByIndex(player->belt_item_id - 1);
        if (belt->element < ELEMENT_COUNT)
            player->element_resistances[belt->element] += belt->element_damage;
        player->belt_graphic_id = belt->spec1;
        player->weight_current += belt->weight;
        player->min_damage += belt->min_damage;
        player->max_damage += belt->max_damage;
        player->accuracy += belt->accuracy;
        player->evasion += belt->evade;
        player->armor += belt->armor;
        player->equip_bonus_hp += belt->hp;
        player->equip_bonus_tp += belt->tp;
        player->equip_strength_bonus += belt->strength;
        player->equip_wisdom_bonus += belt->wisdom;
        player->equip_intelligence_bonus += belt->intelligence;
        player->equip_agility_bonus += belt->agility;
        player->equip_constitution_bonus += belt->constitution;
        player->equip_charisma_bonus += belt->charisma;
    }
    else
    {
        player->belt_item_id = 0;
        player->belt_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->necklace_item_id) == EifItemType_Necklace)
    {
        ItemValue *necklace = GUI->item_values->GetByIndex(player->necklace_item_id - 1);
        if (necklace->element < ELEMENT_COUNT)
            player->element_resistances[necklace->element] += necklace->element_damage;
        player->necklace_graphic_id = necklace->spec1;
        player->weight_current += necklace->weight;
        player->min_damage += necklace->min_damage;
        player->max_damage += necklace->max_damage;
        player->accuracy += necklace->accuracy;
        player->evasion += necklace->evade;
        player->armor += necklace->armor;
        player->equip_bonus_hp += necklace->hp;
        player->equip_bonus_tp += necklace->tp;
        player->equip_strength_bonus += necklace->strength;
        player->equip_wisdom_bonus += necklace->wisdom;
        player->equip_intelligence_bonus += necklace->intelligence;
        player->equip_agility_bonus += necklace->agility;
        player->equip_constitution_bonus += necklace->constitution;
        player->equip_charisma_bonus += necklace->charisma;
    }
    else
    {
        player->necklace_item_id = 0;
        player->necklace_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->hat_item_id) == EifItemType_Hat)
    {
        ItemValue *hat = GUI->item_values->GetByIndex(player->hat_item_id - 1);
        if (hat->element < ELEMENT_COUNT)
            player->element_resistances[hat->element] += hat->element_damage;
        player->hat_graphic_id = hat->spec1;
        player->weight_current += hat->weight;
        player->min_damage += hat->min_damage;
        player->max_damage += hat->max_damage;
        player->accuracy += hat->accuracy;
        player->evasion += hat->evade;
        player->armor += hat->armor;
        player->equip_bonus_hp += hat->hp;
        player->equip_bonus_tp += hat->tp;
        player->equip_strength_bonus += hat->strength;
        player->equip_wisdom_bonus += hat->wisdom;
        player->equip_intelligence_bonus += hat->intelligence;
        player->equip_agility_bonus += hat->agility;
        player->equip_constitution_bonus += hat->constitution;
        player->equip_charisma_bonus += hat->charisma;
    }
    else
    {
        player->hat_item_id = 0;
        player->hat_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->shield_item_id) == EifItemType_Shield)
    {
        ItemValue *shield = GUI->item_values->GetByIndex(player->shield_item_id - 1);
        if (shield->element < ELEMENT_COUNT)
            player->element_resistances[shield->element] += shield->element_damage;
        player->shield_graphic_id = shield->spec1;
        player->weight_current += shield->weight;
        player->min_damage += shield->min_damage;
        player->max_damage += shield->max_damage;
        player->accuracy += shield->accuracy;
        player->evasion += shield->evade;
        player->armor += shield->armor;
        player->equip_bonus_hp += shield->hp;
        player->equip_bonus_tp += shield->tp;
        player->equip_strength_bonus += shield->strength;
        player->equip_wisdom_bonus += shield->wisdom;
        player->equip_intelligence_bonus += shield->intelligence;
        player->equip_agility_bonus += shield->agility;
        player->equip_constitution_bonus += shield->constitution;
        player->equip_charisma_bonus += shield->charisma;
    }
    else
    {
        player->shield_item_id = 0;
        player->shield_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->weapon_item_id) == EifItemType_Weapon)
    {
        ItemValue *weapon = GUI->item_values->GetByIndex(player->weapon_item_id - 1);
        if (weapon->element < ELEMENT_COUNT)
            player->element_resistances[weapon->element] += weapon->element_damage;
        player->weapon_graphic_id = weapon->spec1;
        player->weight_current += weapon->weight;
        player->min_damage += weapon->min_damage;
        player->max_damage += weapon->max_damage;
        player->accuracy += weapon->accuracy;
        player->evasion += weapon->evade;
        player->armor += weapon->armor;
        player->equip_bonus_hp += weapon->hp;
        player->equip_bonus_tp += weapon->tp;
        player->equip_strength_bonus += weapon->strength;
        player->equip_wisdom_bonus += weapon->wisdom;
        player->equip_intelligence_bonus += weapon->intelligence;
        player->equip_agility_bonus += weapon->agility;
        player->equip_constitution_bonus += weapon->constitution;
        player->equip_charisma_bonus += weapon->charisma;
    }
    else
    {
        player->weapon_item_id = 0;
        player->weapon_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->ring1_item_id) == EifItemType_Ring)
    {
        ItemValue *ring1 = GUI->item_values->GetByIndex(player->ring1_item_id - 1);
        if (ring1->element < ELEMENT_COUNT)
            player->element_resistances[ring1->element] += ring1->element_damage;
        player->ring1_graphic_id = ring1->spec1;
        player->weight_current += ring1->weight;
        player->min_damage += ring1->min_damage;
        player->max_damage += ring1->max_damage;
        player->accuracy += ring1->accuracy;
        player->evasion += ring1->evade;
        player->armor += ring1->armor;
        player->equip_bonus_hp += ring1->hp;
        player->equip_bonus_tp += ring1->tp;
        player->equip_strength_bonus += ring1->strength;
        player->equip_wisdom_bonus += ring1->wisdom;
        player->equip_intelligence_bonus += ring1->intelligence;
        player->equip_agility_bonus += ring1->agility;
        player->equip_constitution_bonus += ring1->constitution;
        player->equip_charisma_bonus += ring1->charisma;
    }
    else
    {
        player->ring1_item_id = 0;
        player->ring1_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->ring2_item_id) == EifItemType_Ring)
    {
        ItemValue *ring2 = GUI->item_values->GetByIndex(player->ring2_item_id - 1);
        if (ring2->element < ELEMENT_COUNT)
            player->element_resistances[ring2->element] += ring2->element_damage;
        player->ring2_graphic_id = ring2->spec1;
        player->weight_current += ring2->weight;
        player->min_damage += ring2->min_damage;
        player->max_damage += ring2->max_damage;
        player->accuracy += ring2->accuracy;
        player->evasion += ring2->evade;
        player->armor += ring2->armor;
        player->equip_bonus_hp += ring2->hp;
        player->equip_bonus_tp += ring2->tp;
        player->equip_strength_bonus += ring2->strength;
        player->equip_wisdom_bonus += ring2->wisdom;
        player->equip_intelligence_bonus += ring2->intelligence;
        player->equip_agility_bonus += ring2->agility;
        player->equip_constitution_bonus += ring2->constitution;
        player->equip_charisma_bonus += ring2->charisma;
    }
    else
    {
        player->ring2_item_id = 0;
        player->ring2_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->armlet1_item_id) == EifItemType_Armlet)
    {
        ItemValue *armlet1 = GUI->item_values->GetByIndex(player->armlet1_item_id - 1);
        if (armlet1->element < ELEMENT_COUNT)
            player->element_resistances[armlet1->element] += armlet1->element_damage;
        player->armlet1_graphic_id = armlet1->spec1;
        player->weight_current += armlet1->weight;
        player->min_damage += armlet1->min_damage;
        player->max_damage += armlet1->max_damage;
        player->accuracy += armlet1->accuracy;
        player->evasion += armlet1->evade;
        player->armor += armlet1->armor;
        player->equip_bonus_hp += armlet1->hp;
        player->equip_bonus_tp += armlet1->tp;
        player->equip_strength_bonus += armlet1->strength;
        player->equip_wisdom_bonus += armlet1->wisdom;
        player->equip_intelligence_bonus += armlet1->intelligence;
        player->equip_agility_bonus += armlet1->agility;
        player->equip_constitution_bonus += armlet1->constitution;
        player->equip_charisma_bonus += armlet1->charisma;
    }
    else
    {
        player->armlet1_item_id = 0;
        player->armlet1_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->armlet2_item_id) == EifItemType_Armlet)
    {
        ItemValue *armlet2 = GUI->item_values->GetByIndex(player->armlet2_item_id - 1);
        if (armlet2->element < ELEMENT_COUNT)
            player->element_resistances[armlet2->element] += armlet2->element_damage;
        player->armlet2_graphic_id = armlet2->spec1;
        player->weight_current += armlet2->weight;
        player->min_damage += armlet2->min_damage;
        player->max_damage += armlet2->max_damage;
        player->accuracy += armlet2->accuracy;
        player->evasion += armlet2->evade;
        player->armor += armlet2->armor;
        player->equip_bonus_hp += armlet2->hp;
        player->equip_bonus_tp += armlet2->tp;
        player->equip_strength_bonus += armlet2->strength;
        player->equip_wisdom_bonus += armlet2->wisdom;
        player->equip_intelligence_bonus += armlet2->intelligence;
        player->equip_agility_bonus += armlet2->agility;
        player->equip_constitution_bonus += armlet2->constitution;
        player->equip_charisma_bonus += armlet2->charisma;
    }
    else
    {
        player->armlet2_item_id = 0;
        player->armlet2_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->bracer1_item_id) == EifItemType_Bracer)
    {
        ItemValue *bracer1 = GUI->item_values->GetByIndex(player->bracer1_item_id - 1);
        if (bracer1->element < ELEMENT_COUNT)
            player->element_resistances[bracer1->element] += bracer1->element_damage;
        player->bracer1_graphic_id = bracer1->spec1;
        player->weight_current += bracer1->weight;
        player->min_damage += bracer1->min_damage;
        player->min_damage += bracer1->min_damage;
        player->max_damage += bracer1->max_damage;
        player->accuracy += bracer1->accuracy;
        player->evasion += bracer1->evade;
        player->armor += bracer1->armor;
        player->equip_bonus_hp += bracer1->hp;
        player->equip_bonus_tp += bracer1->tp;
        player->equip_strength_bonus += bracer1->strength;
        player->equip_wisdom_bonus += bracer1->wisdom;
        player->equip_intelligence_bonus += bracer1->intelligence;
        player->equip_agility_bonus += bracer1->agility;
        player->equip_constitution_bonus += bracer1->constitution;
        player->equip_charisma_bonus += bracer1->charisma;
    }
    else
    {
        player->bracer1_item_id = 0;
        player->bracer1_graphic_id = 0;
    }
    if (GUI->item_values->GetType(player->bracer2_item_id) == EifItemType_Bracer)
    {
        ItemValue *bracer2 = GUI->item_values->GetByIndex(player->bracer2_item_id - 1);
        if (bracer2->element < ELEMENT_COUNT)
            player->element_resistances[bracer2->element] += bracer2->element_damage;
        player->bracer2_graphic_id = bracer2->spec1;
        player->weight_current += bracer2->weight;
        player->min_damage += bracer2->min_damage;
        player->max_damage += bracer2->max_damage;
        player->accuracy += bracer2->accuracy;
        player->evasion += bracer2->evade;
        player->armor += bracer2->armor;
        player->equip_bonus_hp += bracer2->hp;
        player->equip_bonus_tp += bracer2->tp;
        player->equip_strength_bonus += bracer2->strength;
        player->equip_wisdom_bonus += bracer2->wisdom;
        player->equip_intelligence_bonus += bracer2->intelligence;
        player->equip_agility_bonus += bracer2->agility;
        player->equip_constitution_bonus += bracer2->constitution;
        player->equip_charisma_bonus += bracer2->charisma;
    }
    else
    {
        player->bracer2_item_id = 0;
        player->bracer2_graphic_id = 0;
    }
}

// The players on the map take its quakes, drains and spikes again, after the map was read again.
void Packets::Server_SyncMapHazardFlags(int map_id)
{
    for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
    {
        if ((*it)->map_id != map_id)
            continue;
        (*it)->map_has_quakes = map_control->maps[map_id - 1].has_quakes;
        (*it)->map_has_hp_drain = map_control->maps[map_id - 1].has_hp_drain;
        (*it)->map_has_tp_drain = map_control->maps[map_id - 1].has_tp_drain;
        (*it)->map_has_spikes = map_control->maps[map_id - 1].has_spikes;
    }
}

// Shares a kill's experience with the party members on the map; the share comes back.
int Packets::Party_ShareExp(Player *player, int exp)
{
    if (!player->in_party)
        return exp;
    if (player->CountPartyMembers() < 2)
        return exp;
    int members = 0;
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        Player *member = players->GetById(player->party_ids[i]);
        if (member && member->map_id == player->map_id)
            members++;
    }
    if (members < 2)
        return exp;
    // three or more get a bonus of a share first
    if (exp > 3 && members > 2)
        exp += exp / members;
    exp = exp / members;
    if (exp < 1)
        exp = 1;
    String packet = "";
    char leveled = 0;
    int j = 0;
    do
    {
        Player *member = players->GetById(player->party_ids[j]);
        if (member && member->player_id != player->player_id && member->map_id == player->map_id)
        {
            if (settings->GetMaxKills())
            {
                // each kill counts toward the member's maxkills for the day
                if (kill_counters->IncrementAndGet(member->name) > settings->GetMaxKills())
                    exp = 0;
            }
            member->experience += exp;
            int levelup = players->Player_TryLevelUp(member);
            if (levelup > 0)
            {
                String stats = EO_EncodeNumber(member->stat_points, 2);
                stats.Insert(EO_EncodeNumber(member->skill_points, 2), stats.Length() + 1);
                stats.Insert(EO_EncodeNumber(member->max_hp, 2), stats.Length() + 1);
                stats.Insert(EO_EncodeNumber(member->max_tp, 2), stats.Length() + 1);
                stats.Insert(EO_EncodeNumber(member->max_sp, 2), stats.Length() + 1);
                Client_SendEncoded(member, AC_GROUP, OP_GAMEBAR, stats);
                leveled = 1;
            }
            packet.Insert(EO_EncodeNumber(member->player_id, 2), packet.Length() + 1);
            packet.Insert(EO_EncodeNumber(exp, 4), packet.Length() + 1);
            packet.Insert(EO_EncodeNumber(levelup, 1), packet.Length() + 1);
        }
        j++;
    } while (j < PARTY_MAX_MEMBERS);
    // a level up is shown to the whole map
    if (leveled)
        Server_BroadcastToMap(player->map_id, AC_GROUP, OP_GROUP, packet);
    else
        Server_BroadcastToPartyOnMap(player, AC_GROUP, OP_GROUP, packet);
    return exp;
}

// A turn to one of the four directions, shown to the players near them; not while sitting.
bool Packets::Face_Execute(Player *player, int action, String *data)
{
    player->last_action_tick = DateTimeToTimeStamp(Now());
    if (action == AC_SET)
    {
        if (!player->logged_in)
            return false;
        if (player->on_chair || player->sitting)
            return true;
        if ((*data).Length() < 1)
            return false;
        if (EO_DecodeNumber(String((*data)[1])) > 3)
            return false;
        player->direction = EO_DecodeNumber(String((*data)[1]));
        String packet = EO_EncodeNumber(player->player_id, 2);
        packet = packet + (*data)[1];
        Server_BroadcastNearby(player, AC_SET, OP_DIR, packet);
        return true;
    }
    return false;
}

// Sits on a chair or stands up from it; a chair takes a player who stands in front of it, on the side it faces.
bool Packets::Chair_Execute(Player *player, int action, String *data)
{
    player->last_action_tick = DateTimeToTimeStamp(Now());
    if (action == AC_REQUEST)
    {
        if (!player->logged_in)
            return false;
        if ((*data).Length() < 1)
            return false;
        int sit_action = EO_DecodeNumber(String((*data)[1]));
        if (sit_action == 1)
        {
            if ((*data).Length() < 3)
                return false;
            int x = EO_DecodeNumber(String((*data)[2]));
            int y = EO_DecodeNumber(String((*data)[3]));
            if (player->on_chair)
                return true;
            if (players->IsPlayerAt(player->map_id, x, y))
                return true;
            if (player->map_id < 1 || (int)map_control->maps.size() < player->map_id)
                return true;
            int spec = map_control->GetTileSpecValueAt(player->map_id, x, y);
            if (spec >= TileSpec_ChairDown && spec <= TileSpec_ChairAll)
            {
                MapObject tile = map_control->GetTileSpecObject(player->map_id, x, y);
                String sit_packet = EO_EncodeNumber(player->player_id, 2);
                sit_packet.Insert(EO_EncodeNumber(x, 1), sit_packet.Length() + 1);
                sit_packet.Insert(EO_EncodeNumber(y, 1), sit_packet.Length() + 1);
                if ((spec == TileSpec_ChairDown || spec == TileSpec_ChairDownRight || spec == TileSpec_ChairAll) &&
                    tile.x == player->x && tile.y == player->y - 1)
                {
                    player->on_chair = 1;
                    player->sitting = 0;
                    player->x = x;
                    player->y = y;
                    player->direction = FacingDirection_Down;
                    sit_packet.Insert(EO_EncodeNumber(0, 1), sit_packet.Length() + 1);
                    Client_SendEncoded(player, AC_RESULT, OP_SIT, sit_packet);
                    Server_BroadcastNearby(player, AC_SET, OP_SIT, sit_packet);
                    return true;
                }
                if ((spec == TileSpec_ChairLeft || spec == TileSpec_ChairUpLeft || spec == TileSpec_ChairAll) &&
                    tile.x == player->x + 1 && tile.y == player->y)
                {
                    player->on_chair = 1;
                    player->sitting = 0;
                    player->x = x;
                    player->y = y;
                    player->direction = FacingDirection_Left;
                    sit_packet.Insert(EO_EncodeNumber(1, 1), sit_packet.Length() + 1);
                    Client_SendEncoded(player, AC_RESULT, OP_SIT, sit_packet);
                    Server_BroadcastNearby(player, AC_SET, OP_SIT, sit_packet);
                    return true;
                }
                if ((spec == TileSpec_ChairRight || spec == TileSpec_ChairDownRight || spec == TileSpec_ChairAll) &&
                    tile.x == player->x - 1 && tile.y == player->y)
                {
                    player->on_chair = 1;
                    player->sitting = 0;
                    player->x = x;
                    player->y = y;
                    player->direction = FacingDirection_Right;
                    sit_packet.Insert(EO_EncodeNumber(3, 1), sit_packet.Length() + 1);
                    Client_SendEncoded(player, AC_RESULT, OP_SIT, sit_packet);
                    Server_BroadcastNearby(player, AC_SET, OP_SIT, sit_packet);
                    return true;
                }
                if ((spec == TileSpec_ChairUp || spec == TileSpec_ChairUpLeft || spec == TileSpec_ChairAll) &&
                    tile.x == player->x && tile.y == player->y + 1)
                {
                    player->on_chair = 1;
                    player->sitting = 0;
                    player->x = x;
                    player->y = y;
                    player->direction = FacingDirection_Up;
                    sit_packet.Insert(EO_EncodeNumber(2, 1), sit_packet.Length() + 1);
                    Client_SendEncoded(player, AC_RESULT, OP_SIT, sit_packet);
                    Server_BroadcastNearby(player, AC_SET, OP_SIT, sit_packet);
                    return true;
                }
            }
        }
        else
        {
            // standing up puts them a step in front of the chair
            if (!player->on_chair)
                return true;
            if (player->direction == FacingDirection_Down)
                player->y++;
            if (player->direction == FacingDirection_Left)
                player->x--;
            if (player->direction == FacingDirection_Up)
                player->y--;
            if (player->direction == FacingDirection_Right)
                player->x++;
            player->on_chair = 0;
            player->sitting = 0;
            String stand_packet = EO_EncodeNumber(player->player_id, 2);
            stand_packet.Insert(EO_EncodeNumber(player->x, 1), stand_packet.Length() + 1);
            stand_packet.Insert(EO_EncodeNumber(player->y, 1), stand_packet.Length() + 1);
            Client_SendEncoded(player, AC_CLOSE, OP_SIT, stand_packet);
            Server_BroadcastNearby(player, AC_DELETE, OP_SIT, stand_packet);
            return true;
        }
    }
    return false;
}

// An attack in the direction they face: a player in reach on a pk map, else the first npc in reach, else the arena.
bool Packets::Attack_Execute(Player *attacker, int action, String *data)
{
    attacker->last_action_tick = DateTimeToTimeStamp(Now());
    if (attacker->map_id < 1)
        return true;
    // too heavy to fight
    if (attacker->weight_max + 2 < attacker->weight_current)
        return true;
    if (action == AC_DO)
    {
        if (!attacker->logged_in)
            return false;
        if (attacker->sitting || attacker->on_chair)
            return true;
        if ((*data).Length() < 4)
            return false;
        int attack_tick = EO_DecodeNumber((*data).SubString(2, 3));
        int elapsed = attack_tick - attacker->last_client_tick;
        if (elapsed < 0 && attacker->last_client_tick > CLIENT_TICK_WRAP_MIN)
            elapsed = 44;
        attacker->last_client_tick = attack_tick;
        // the client's clock: 44 hundredths of a second at least between attacks
        if (elapsed < 44)
            return false;
        TTimeStamp stamp;
        stamp = DateTimeToTimeStamp(Now());
        int sync_tick = stamp.Time / 10 + 100;
        int clock_drift;
        // a clock that drifts 8 seconds from where it started is ignored
        if (attack_tick > sync_tick)
        {
            clock_drift = attack_tick - sync_tick;
            if (attacker->sync_base_ahead < 0)
                attacker->sync_base_ahead = clock_drift;
            if (abs(clock_drift - attacker->sync_base_ahead) > 800)
                return true;
        }
        else
        {
            clock_drift = sync_tick - attack_tick;
            if (attacker->sync_base_behind < 0)
                attacker->sync_base_behind = clock_drift;
            if (abs(clock_drift - attacker->sync_base_behind) > 800)
                return true;
        }
        attacker->direction = EO_DecodeNumber(String((*data)[1]));
        int target_x = target_x = attacker->x;
        int target_y = target_y = attacker->y;
        int reach = 1;
        if (attacker->direction > FacingDirection_Right)
            return true;
        // a ranged weapon reaches 6 tiles; the two instruments do not hit
        if (weapon_map->Combat_IsRangedWeapon(attacker->weapon_graphic_id))
        {
            reach = 6;
            if (attacker->weapon_graphic_id == WeaponGraphic_Harp ||
                attacker->weapon_graphic_id == WeaponGraphic_Guitar)
                reach = 0;
        }
        // a bow ( subtype 1 ) needs arrows ( subtype 2 ) in the shield slot
        if (GUI->item_values->GetSubtype(attacker->weapon_item_id) == 1 &&
            GUI->item_values->GetSubtype(attacker->shield_item_id) != 2)
            return true;
        // .. a pk map: the first player in reach outside their party
        if (map_control->maps[attacker->map_id - 1].map_type == MAP_TYPE_PK)
        {
            for (int i = 0; i < reach; i++)
            {
                if (attacker->direction == FacingDirection_Down)
                    target_y++;
                if (attacker->direction == FacingDirection_Left)
                    target_x--;
                if (attacker->direction == FacingDirection_Up)
                    target_y--;
                if (attacker->direction == FacingDirection_Right)
                    target_x++;
                vector<Player *>::iterator it;
                for (it = players->players.begin(); it != players->players.end(); it++)
                {
                    if ((*it)->map_id == attacker->map_id && (*it)->x == target_x && (*it)->y == target_y &&
                        !attacker->IsPartyMember((*it)->player_id))
                    {
                        int pk_damage = 0;
                        int pk_hit_rate = GUI->game_control->Combat_CalcHitRate(attacker->accuracy, (*it)->evasion,
                            0.9);
                        if (random(100) < pk_hit_rate)
                        {
                            pk_hit_rate = GUI->game_control->Combat_CalcArmorPen((attacker->min_damage +
                                attacker->max_damage) / 2, (*it)->armor, 0.8);
                            double scaled = attacker->min_damage;
                            if (scaled < 1.0)
                                scaled = 1.0;
                            scaled = 0.01L * scaled;
                            scaled = pk_hit_rate * scaled;
                            scaled = random(attacker->max_damage - attacker->min_damage + 2) + scaled;
                            pk_damage = scaled;
                            if (pk_damage < 1)
                                pk_damage = 1;
                        }
                        if (attacker->weapon_item_id > 0)
                        {
                            MapCoord pk_element = GUI->item_values->GetElement(attacker->weapon_item_id);
                            pk_element.element_damage = 0;
                            if (pk_element.element == ElementKind_Light)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[1], (*it)->element_resistances[2]) * pk_damage;
                            if (pk_element.element == ElementKind_Dark)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[2], (*it)->element_resistances[1]) * pk_damage;
                            if (pk_element.element == ElementKind_Earth)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[3], (*it)->element_resistances[6]) * pk_damage;
                            if (pk_element.element == ElementKind_Wind)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[4], (*it)->element_resistances[3]) * pk_damage;
                            if (pk_element.element == ElementKind_Water)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[5], (*it)->element_resistances[4]) * pk_damage;
                            if (pk_element.element == ElementKind_Fire)
                                pk_damage = GUI->game_control->Combat_CalcElementMult(pk_element,
                                    attacker->element_resistances[6], (*it)->element_resistances[5]) * pk_damage;
                        }
                        // a hit in the back does half again
                        if ((*it)->direction == attacker->direction)
                            pk_damage += pk_damage / 2;
                        (*it)->hp -= pk_damage;
                        if ((*it)->hp > (*it)->max_hp)
                            (*it)->hp = (*it)->max_hp;
                        if ((*it)->hp < 1)
                            (*it)->hp = 0;
                        if (pk_damage > 0 && (*it)->in_party)
                        {
                            String party_pkt = EO_EncodeNumber((*it)->player_id, 2);
                            party_pkt.Insert(EO_EncodeNumber((*it)->HpPercent(), 1), party_pkt.Length() + 1);
                            Server_BroadcastToParty(*it, AC_UPDATE, OP_GROUP, party_pkt);
                        }
                        String pk_packet = EO_EncodeNumber(attacker->player_id, 2);
                        pk_packet.Insert(EO_EncodeNumber((*it)->player_id, 2), pk_packet.Length() + 1);
                        pk_packet.Insert(EO_EncodeNumber(pk_damage, 3), pk_packet.Length() + 1);
                        pk_packet.Insert(EO_EncodeNumber(attacker->direction, 1), pk_packet.Length() + 1);
                        pk_packet.Insert(EO_EncodeNumber((*it)->HpPercent(), 1), pk_packet.Length() + 1);
                        if ((*it)->hp < 1)
                            pk_packet.Insert(EO_EncodeNumber(1, 1), pk_packet.Length() + 1);
                        else
                            pk_packet.Insert(EO_EncodeNumber(0, 1), pk_packet.Length() + 1);
                        Server_BroadcastNearby(*it, AC_RESULT, OP_PLAYER, pk_packet);
                        Client_SendEncoded(*it, AC_RESULT, OP_PLAYER, pk_packet);
                        pk_packet = EO_EncodeNumber((*it)->hp, 2);
                        pk_packet.Insert(EO_EncodeNumber((*it)->tp, 2), pk_packet.Length() + 1);
                        pk_packet.Insert(EO_EncodeNumber(0, 2), pk_packet.Length() + 1);
                        Client_SendEncoded(*it, AC_SET, OP_GAMEBAR, pk_packet);
                        if ((*it)->hp < 1)
                        {
                            Player_Respawn(*it);
                            Player_FireQuestTriggers(attacker, QuestCondition_KilledPlayers, 1);
                        }
                        return true;
                    }
                }
            }
            target_x = target_x = attacker->x;
            target_y = target_y = attacker->y;
        }
        // .. each tile in reach: a wall stops it, an npc on it is hit
        for (int tile_step = 0; tile_step < reach; tile_step++)
        {
            if (attacker->direction == FacingDirection_Down)
                target_y++;
            if (attacker->direction == FacingDirection_Left)
                target_x--;
            if (attacker->direction == FacingDirection_Up)
                target_y--;
            if (attacker->direction == FacingDirection_Right)
                target_x++;
            if (target_y < 0 || target_x < 0)
            {
                String off_map_packet = EO_EncodeNumber(attacker->player_id, 2);
                off_map_packet = off_map_packet + (*data)[1];
                Server_BroadcastNearby(attacker, AC_SET, OP_ATTACK, off_map_packet);
                return true;
            }
            if (!map_control->IsTileClear(attacker->map_id, target_x, target_y))
            {
                String blocked_packet = EO_EncodeNumber(attacker->player_id, 2);
                blocked_packet = blocked_packet + (*data)[1];
                Server_BroadcastNearby(attacker, AC_SET, OP_ATTACK, blocked_packet);
                return true;
            }
            vector<Npc *>::iterator npc;
            for (npc = map_control->maps[attacker->map_id - 1].npc_list.begin();
                npc != map_control->maps[attacker->map_id - 1].npc_list.end(); npc++)
            {
                if ((*npc)->x != target_x)
                    continue;
                if ((*npc)->y != target_y)
                    continue;
                if ((*npc)->alive)
                {
                    MapCoord v193 = GUI->npc_values->GetType((*npc)->id);
                    // npc types 1 to 5 fight
                    if (v193.x > 0 && v193.x < 6)
                    {
                        // an npc another player fights is theirs alone, or their party's
                        if ((*npc)->chase_target_id != attacker->player_id && (*npc)->chase_target_id > 0 &&
                            !attacker->IsPartyMember((*npc)->chase_target_id))
                        {
                            String contested_reply = EO_EncodeNumber(attacker->player_id, 2);
                            contested_reply.Insert((*data)[1], contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber((*npc)->index, 2), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber(0, 3), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber((*npc)->hp_percent, 2),
                                contested_reply.Length() + 1);
                            Server_BroadcastNearby(attacker, AC_RESULT, OP_NPC, contested_reply);
                            contested_reply.Insert(EO_EncodeNumber(2, 1), contested_reply.Length() + 1);
                            Client_SendEncoded(attacker, AC_RESULT, OP_NPC, contested_reply);
                            return true;
                        }
                        int npc_hit_damage = 0;
                        int npc_hit_rate = GUI->game_control->Combat_CalcHitRate(attacker->accuracy, (*npc)->evade,
                            0.9);
                        if (random(100) < npc_hit_rate)
                        {
                            npc_hit_rate = GUI->game_control->Combat_CalcArmorPen((attacker->min_damage +
                                attacker->max_damage) / 2, (*npc)->armor, 0.8);
                            double scaled_damage = attacker->min_damage;
                            if (scaled_damage < 1.0)
                                scaled_damage = 1.0;
                            scaled_damage = 0.01L * scaled_damage;
                            scaled_damage = npc_hit_rate * scaled_damage;
                            scaled_damage = random(attacker->max_damage - attacker->min_damage + 2) + scaled_damage;
                            npc_hit_damage = scaled_damage;
                            if (npc_hit_damage < 1)
                                npc_hit_damage = 1;
                        }
                        if (attacker->weapon_item_id > 0)
                        {
                            MapCoord npc_hit_element = GUI->item_values->GetElement(attacker->weapon_item_id);
                            npc_hit_element.element_damage = 0;
                            if (npc_hit_element.element == ElementKind_Light)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[1], (*npc)->element_weakness_damage_table[2]) *
                                    npc_hit_damage;
                            if (npc_hit_element.element == ElementKind_Dark)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[2], (*npc)->element_weakness_damage_table[1]) *
                                    npc_hit_damage;
                            if (npc_hit_element.element == ElementKind_Earth)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[3], (*npc)->element_weakness_damage_table[6]) *
                                    npc_hit_damage;
                            if (npc_hit_element.element == ElementKind_Wind)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[4], (*npc)->element_weakness_damage_table[3]) *
                                    npc_hit_damage;
                            if (npc_hit_element.element == ElementKind_Water)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[5], (*npc)->element_weakness_damage_table[4]) *
                                    npc_hit_damage;
                            if (npc_hit_element.element == ElementKind_Fire)
                                npc_hit_damage = GUI->game_control->Combat_CalcElementMult(npc_hit_element,
                                    attacker->element_resistances[6], (*npc)->element_weakness_damage_table[5]) *
                                    npc_hit_damage;
                        }
                        // half again from behind, or at full hp
                        if ((*npc)->facing == attacker->direction || (*npc)->hp == (*npc)->max_hp)
                            npc_hit_damage += npc_hit_damage / 2;
                        if ((*npc)->boss > 0)
                            map_control->AggroChildNpcs(attacker->map_id);
                        (*npc)->aggressive = true;
                        (*npc)->leash_timer = random(50) + 100;
                        // two npcs at most chase one player
                        if (map_control->CountNpcsChasingPlayer(attacker->map_id, attacker->player_id) < 2 && !v193.y)
                            (*npc)->chase_target_id = attacker->player_id;
                        (*npc)->hp -= npc_hit_damage;
                        (*npc)->hp_percent = 100 * (*npc)->hp / GUI->npc_values->GetMaxHp((*npc)->id);
                        // .. the kill: the shared experience, the drop theirs for 61 seconds, a boss's children die
                        if ((*npc)->hp < 1)
                        {
                            int exp_gain = GUI->npc_values->GetExp((*npc)->id);
                            int drop_result = 0;
                            exp_gain = Party_ShareExp(attacker, exp_gain);
                            if ((*npc)->drop_item_id > 0 && (*npc)->drop_amount > 0)
                                drop_result = map_control->AddGroundItem(attacker->map_id, (*npc)->drop_item_id,
                                    (*npc)->x, (*npc)->y, (*npc)->drop_amount, attacker->account_ident, 61);
                            TDateTime now = Now();
                            (*npc)->death_stamp = DateTimeToTimeStamp(now);
                            (*npc)->chase_target_id = -1;
                            (*npc)->alive = false;
                            if ((*npc)->boss > 0 && map_control->KillChildNpcs(attacker->map_id))
                                Server_BroadcastToMap(attacker->map_id, AC_JUNK, OP_NPC,
                                    EO_EncodeNumber(map_control->maps[attacker->map_id - 1].child_npc_id, 2));
                            String kill_reply = EO_EncodeNumber(attacker->player_id, 2);
                            kill_reply.Insert(EO_EncodeNumber(attacker->direction, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*npc)->index, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(drop_result, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*npc)->drop_item_id, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*npc)->x, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*npc)->y, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*npc)->drop_amount, 4), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(npc_hit_damage, 3), kill_reply.Length() + 1);
                            if (settings->GetMaxKills())
                            {
                                if (kill_counters->IncrementAndGet(attacker->name) > settings->GetMaxKills())
                                    exp_gain = 0;
                            }
                            attacker->experience += exp_gain;
                            if (players->Player_TryLevelUp(attacker) > 0)
                            {
                                Server_BroadcastNearby(attacker, AC_CONFIRM, OP_NPC, kill_reply);
                                kill_reply.Insert(EO_EncodeNumber(attacker->experience, 4), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->level, 1), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->stat_points, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->skill_points, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->max_hp, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->max_tp, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(attacker->max_sp, 2), kill_reply.Length() + 1);
                                Client_SendEncoded(attacker, AC_CONFIRM, OP_NPC, kill_reply);
                                return true;
                            }
                            Server_BroadcastNearby(attacker, AC_HIT, OP_NPC, kill_reply);
                            kill_reply.Insert(EO_EncodeNumber(attacker->experience, 4), kill_reply.Length() + 1);
                            if (!attacker->cheater_flag)
                                Client_SendEncoded(attacker, AC_HIT, OP_NPC, kill_reply);
                            // a flagged bot loses the experience half the time, and is not told
                            else if (random(6) > 2)
                                attacker->experience -= exp_gain;
                            Player_FireQuestTriggers(attacker, QuestCondition_KilledNpcs, (*npc)->id);
                            return true;
                        }
                        String reply = EO_EncodeNumber(attacker->player_id, 2);
                        reply.Insert((*data)[1], reply.Length() + 1);
                        reply.Insert(EO_EncodeNumber((*npc)->index, 2), reply.Length() + 1);
                        reply.Insert(EO_EncodeNumber(npc_hit_damage, 3), reply.Length() + 1);
                        // a flagged bot is not told the npc's true hp
                        if (!attacker->cheater_flag)
                            reply.Insert(EO_EncodeNumber((*npc)->hp_percent, 2), reply.Length() + 1);
                        else if ((*npc)->hp_percent < 1)
                            reply.Insert(EO_EncodeNumber(random(2) + 1, 2), reply.Length() + 1);
                        Server_BroadcastNearby(attacker, AC_RESULT, OP_NPC, reply);
                        reply.Insert(EO_EncodeNumber(1, 1), reply.Length() + 1);
                        Client_SendEncoded(attacker, AC_RESULT, OP_NPC, reply);
                        return true;
                    }
                }
            }
            // .. the arena: a hit on the player next to them puts that one out, until one is left
            if (attacker->in_arena)
            {
                if (!map_control->maps[attacker->map_id - 1].arena_enabled)
                    attacker->in_arena = false;
                else if (!tile_step)
                {
                    Player *target = players->GetByMapTile(attacker->map_id, target_x, target_y);
                    if (target)
                    {
                        attacker->arena_kills++;
                        target->in_arena = false;
                        MapCoord coords;
                        coords.x = map_control->maps[attacker->map_id - 1].relog_x;
                        coords.y = map_control->maps[attacker->map_id - 1].relog_y;
                        Player_Warp(target, target->map_id, coords, WarpEffect_None, 1);
                        if (players->CountArenaPlayers(attacker->map_id) < 2)
                        {
                            String arena_win_pkt = attacker->name;
                            arena_win_pkt.Insert(" ", arena_win_pkt.Length() + 1);
                            if (attacker->title.Length() > 0)
                            {
                                arena_win_pkt.Insert("(", arena_win_pkt.Length() + 1);
                                arena_win_pkt.Insert(attacker->title, arena_win_pkt.Length() + 1);
                                arena_win_pkt.Insert(") ", arena_win_pkt.Length() + 1);
                            }
                            arena_win_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_win_pkt.Length() + 1);
                            arena_win_pkt.Insert(EO_EncodeNumber(attacker->arena_kills, 1), arena_win_pkt.Length() + 1);
                            arena_win_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_win_pkt.Length() + 1);
                            arena_win_pkt.Insert(attacker->name, arena_win_pkt.Length() + 1);
                            arena_win_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_win_pkt.Length() + 1);
                            arena_win_pkt.Insert(target->name, arena_win_pkt.Length() + 1);
                            Server_BroadcastToMap(attacker->map_id, AC_CONFIRM, OP_ARENA,
                                arena_win_pkt);
                            Client_SendEncoded(target, AC_CONFIRM, OP_ARENA, arena_win_pkt);
                            // the winner leaves too
                            if (map_control->maps[attacker->map_id - 1].arena_block > 2)
                            {
                                attacker->in_arena = false;
                                Player_Warp(attacker, attacker->map_id, coords, WarpEffect_None, 1);
                            }
                            return true;
                        }
                        String arena_elim_pkt = EO_EncodeNumber(attacker->player_id, 2);
                        arena_elim_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert(EO_EncodeNumber(attacker->direction, 1), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert(EO_EncodeNumber(attacker->arena_kills, 1), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert(attacker->name, arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert((char)EO_GetBreakByte(EO_BREAK_BYTE), arena_elim_pkt.Length() + 1);
                        arena_elim_pkt.Insert(target->name, arena_elim_pkt.Length() + 1);
                        Server_BroadcastToMap(attacker->map_id, AC_HIT, OP_ARENA, arena_elim_pkt);
                        Client_SendEncoded(target, AC_HIT, OP_ARENA, arena_elim_pkt);
                        return true;
                    }
                }
            }
        }
        String final_pkt = EO_EncodeNumber(attacker->player_id, 2);
        final_pkt = final_pkt + (*data)[1];
        Server_BroadcastNearby(attacker, AC_SET, OP_ATTACK, final_pkt);
        return true;
    }
    return false;
}

// A spell: Request starts the cast, TargetOther, TargetGroup and TargetSelf let it go, Use turns them.
bool Packets::Spell_Execute(Player *caster, int action, String *data)
{
    caster->last_action_tick = DateTimeToTimeStamp(Now());
    if (caster->map_id < 1)
        return true;
    if (caster->weight_max + 2 < caster->weight_current)
        return true;
    if (action == AC_REQUEST)
    {
        if ((*data).Length() < 5)
            return false;
        caster->queued_spell_id = EO_DecodeNumber((*data).SubString(1, 2));
        if (!players->Player_HasSpellId(caster, caster->queued_spell_id))
            return false;
        // the client's clock in hundredths, 30 a unit of cast time
        int cast_time = 30 * GUI->skill_values->GetCastTime(caster->queued_spell_id);
        caster->expected_cast_timestamp = cast_time + EO_DecodeNumber((*data).SubString(3, 3)) - 1;
        String cast_packet = EO_EncodeNumber(caster->player_id, 2);
        cast_packet.Insert(EO_EncodeNumber(caster->queued_spell_id, 2), cast_packet.Length() + 1);
        Server_BroadcastNearby(caster, AC_REQUEST, OP_MAGIC, cast_packet);
        return true;
    }
    if (action == AC_TARGET)
    {
        if (!caster->logged_in)
            return false;
        if (caster->sitting || caster->on_chair)
            return true;
        if ((*data).Length() < 11)
            return false;
        int client_tick = EO_DecodeNumber((*data).SubString(2, 3));
        int elapsed = client_tick - caster->last_client_tick;
        if (elapsed < 0 && caster->last_client_tick > CLIENT_TICK_WRAP_MIN)
            elapsed = 44;
        caster->last_client_tick = client_tick;
        if (elapsed < 44)
            return false;
        TTimeStamp stamp;
        stamp = DateTimeToTimeStamp(Now());
        int sync_tick = stamp.Time / 10 + 100;
        int clock_drift;
        if (client_tick > sync_tick)
        {
            clock_drift = client_tick - sync_tick;
            if (caster->sync_base_ahead < 0)
                caster->sync_base_ahead = clock_drift;
            if (abs(clock_drift - caster->sync_base_ahead) > 800)
                return true;
        }
        else
        {
            clock_drift = sync_tick - client_tick;
            if (caster->sync_base_behind < 0)
                caster->sync_base_behind = clock_drift;
            if (abs(clock_drift - caster->sync_base_behind) > 800)
                return true;
        }
        int spell_target = EO_DecodeNumber(String((*data)[1]));
        int target_spell_id = EO_DecodeNumber((*data).SubString(5, 2));
        int target_id = EO_DecodeNumber((*data).SubString(7, 2));
        int target_cast_tick = EO_DecodeNumber((*data).SubString(9, 3));
        int target_type = GUI->skill_values->GetTargetType(target_spell_id);
        if (target_spell_id != caster->queued_spell_id)
            return false;
        // not before the cast time is over, but across midnight
        if (target_cast_tick < caster->expected_cast_timestamp &&
            (target_cast_tick > 1000 || caster->expected_cast_timestamp < 8638000))
            return false;
        if (target_type)
            return true;
        if (!players->Player_HasSpellId(caster, target_spell_id))
            return true;
        int target_skill_type = GUI->skill_values->GetSkillType(target_spell_id);
        int target_tp_cost = GUI->skill_values->GetTpCost(target_spell_id);
        // not enough tp: the client is told
        if (caster->tp < target_tp_cost)
        {
            String target_no_tp_reply = EO_EncodeNumber(target_spell_id, 2);
            target_no_tp_reply.Insert(EO_EncodeNumber(caster->hp, 2), target_no_tp_reply.Length() + 1);
            target_no_tp_reply.Insert(EO_EncodeNumber(caster->tp, 2), target_no_tp_reply.Length() + 1);
            Client_SendEncoded(caster, AC_RESULT, OP_MAGIC, target_no_tp_reply);
            return true;
        }
        caster->tp -= target_tp_cost;
        // .. at a player: heal, or on a pk map hurt one outside their party
        if (spell_target == 1)
        {
            Player *target = players->by_id[target_id];
            if (!target)
                return true;
            if (target->map_id != caster->map_id)
                return true;
            // out of view: the client gets a refresh
            if (!Server_InViewRange(caster->x, caster->y, target->x, target->y))
            {
                Client_SendEncoded(caster, AC_RESULT, OP_REFRESH, Refresh_BuildReply(caster));
                return true;
            }
            if (!target_skill_type)
            {
                int target_hp_heal = GUI->skill_values->GetHpHeal(target_spell_id);
                target->hp += target_hp_heal;
                if (target->hp > target->max_hp)
                    target->hp = target->max_hp;
                int heal_hp_percent = 100 * target->hp / target->max_hp;
                String target_player_reply = EO_EncodeNumber(target->player_id, 2);
                target_player_reply.Insert(EO_EncodeNumber(caster->player_id, 2), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(spell_target, 1), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(target_spell_id, 2), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(target_hp_heal, 4), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(heal_hp_percent, 1), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(target->hp, 2), target_player_reply.Length() + 1);
                Server_BroadcastToMap(target->map_id, AC_TARGET, OP_MAGIC,
                    target_player_reply);
                target_player_reply = EO_EncodeNumber(target_spell_id, 2);
                target_player_reply.Insert(EO_EncodeNumber(caster->hp, 2), target_player_reply.Length() + 1);
                target_player_reply.Insert(EO_EncodeNumber(caster->tp, 2), target_player_reply.Length() + 1);
                Client_SendEncoded(caster, AC_RESULT, OP_MAGIC, target_player_reply);
                if (target->in_party)
                {
                    String heal_packet = EO_EncodeNumber(target->player_id, 2);
                    heal_packet.Insert(EO_EncodeNumber(target->HpPercent(), 1), heal_packet.Length() + 1);
                    Server_BroadcastToParty(target, AC_UPDATE, OP_GROUP, heal_packet);
                }
                return true;
            }
            if (target_skill_type == 1)
            {
                if (map_control->maps[caster->map_id - 1].map_type == MAP_TYPE_PK)
                {
                    if (target->player_id == caster->player_id)
                        return true;
                    if (target->map_id != caster->map_id)
                        return true;
                    if (caster->IsPartyMember(target->player_id))
                        return true;
                    int player_damage = 0;
                    int player_hit_rate = GUI->game_control->Combat_CalcArmorPen(caster->accuracy, target->evasion,
                        0.9);
                    if (random(100) < player_hit_rate)
                    {
                        MapCoord player_dmg = GUI->skill_values->GetDamage(target_spell_id);
                        int player_min_total = player_dmg.min_damage + caster->min_damage;
                        int player_max_total = player_dmg.max_damage + caster->max_damage;
                        player_hit_rate = GUI->game_control->Combat_CalcArmorPen((player_min_total + player_max_total) /
                            2, target->armor, 0.8);
                        double scaled = player_min_total;
                        if (scaled < 1.0)
                            scaled = 1.0;
                        scaled = 0.01L * scaled;
                        scaled = player_hit_rate * scaled;
                        scaled = random(player_max_total - player_min_total + 2) + scaled;
                        player_damage = scaled;
                        if (player_damage < 1)
                            player_damage = 1;
                    }
                    if (target_spell_id > 0)
                    {
                        MapCoord player_element = GUI->skill_values->GetElement(target_spell_id);
                        if (player_element.element == ElementKind_Light)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[1], target->element_resistances[2]) * player_damage;
                        if (player_element.element == ElementKind_Dark)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[2], target->element_resistances[1]) * player_damage;
                        if (player_element.element == ElementKind_Earth)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[3], target->element_resistances[6]) * player_damage;
                        if (player_element.element == ElementKind_Wind)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[4], target->element_resistances[3]) * player_damage;
                        if (player_element.element == ElementKind_Water)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[5], target->element_resistances[4]) * player_damage;
                        if (player_element.element == ElementKind_Fire)
                            player_damage = GUI->game_control->Combat_CalcElementMult(player_element,
                                caster->element_resistances[6], target->element_resistances[5]) * player_damage;
                    }
                    target->hp -= player_damage;
                    if (target->hp > target->max_hp)
                        target->hp = target->max_hp;
                    if (target->hp < 1)
                        target->hp = 0;
                    if (player_damage > 0 && target->in_party)
                    {
                        String hp_pkt = EO_EncodeNumber(target->player_id, 2);
                        hp_pkt.Insert(EO_EncodeNumber(target->HpPercent(), 1), hp_pkt.Length() + 1);
                        Server_BroadcastToParty(target, AC_UPDATE, OP_GROUP, hp_pkt);
                    }
                    String attack_packet = EO_EncodeNumber(caster->player_id, 2);
                    attack_packet.Insert(EO_EncodeNumber(target->player_id, 2), attack_packet.Length() + 1);
                    attack_packet.Insert(EO_EncodeNumber(player_damage, 3), attack_packet.Length() + 1);
                    attack_packet.Insert(EO_EncodeNumber(caster->direction, 1), attack_packet.Length() + 1);
                    attack_packet.Insert(EO_EncodeNumber(target->HpPercent(), 1), attack_packet.Length() + 1);
                    if (target->hp < 1)
                        attack_packet.Insert(EO_EncodeNumber(1, 1), attack_packet.Length() + 1);
                    else
                        attack_packet.Insert(EO_EncodeNumber(0, 1), attack_packet.Length() + 1);
                    attack_packet.Insert(EO_EncodeNumber(target_spell_id, 2), attack_packet.Length() + 1);
                    Server_BroadcastNearby(target, AC_CAST, OP_PLAYER, attack_packet);
                    Client_SendEncoded(target, AC_CAST, OP_PLAYER, attack_packet);
                    attack_packet = EO_EncodeNumber(target->hp, 2);
                    attack_packet.Insert(EO_EncodeNumber(target->tp, 2), attack_packet.Length() + 1);
                    attack_packet.Insert(EO_EncodeNumber(0, 2), attack_packet.Length() + 1);
                    Client_SendEncoded(target, AC_SET, OP_GAMEBAR, attack_packet);
                    if (target->hp < 1)
                        Player_Respawn(target);
                }
                return true;
            }
            return true;
        }
        // .. at an npc: like an attack, with the spell's damage and element
        if (spell_target == 2)
        {
            vector<Npc *>::iterator iter;
            for (iter = map_control->maps[caster->map_id - 1].npc_list.begin();
                iter != map_control->maps[caster->map_id - 1].npc_list.end(); ++iter)
            {
                if ((*iter)->index == target_id)
                {
                    MapCoord type_info = GUI->npc_values->GetType((*iter)->id);
                    if (!(*iter)->alive)
                        return true;
                    if (!Server_InViewRange(caster->x, caster->y, (*iter)->x, (*iter)->y))
                    {
                        Client_SendEncoded(caster, AC_RESULT, OP_REFRESH,
                            Refresh_BuildReply(caster));
                        return true;
                    }
                    MapCoord coords;
                    coords.x = (*iter)->x;
                    coords.y = (*iter)->y;
                    if (target_skill_type == 1 && type_info.type > NpcType_Friendly && type_info.type < NpcType_Shop)
                    {
                        if ((*iter)->chase_target_id != caster->player_id && (*iter)->chase_target_id > 0 &&
                            !caster->IsPartyMember((*iter)->chase_target_id))
                        {
                            String contested_reply = EO_EncodeNumber(target_spell_id, 2);
                            contested_reply.Insert(EO_EncodeNumber(caster->player_id, 2), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber(caster->direction, 1), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber((*iter)->index, 2), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber(0, 3), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber((short)(*iter)->hp_percent, 2),
                                contested_reply.Length() + 1);
                            Server_BroadcastNearTile(caster->player_id, caster->map_id, coords, AC_RESULT,
                                OP_M_NPC, contested_reply);
                            contested_reply.Insert(EO_EncodeNumber(caster->tp, 2), contested_reply.Length() + 1);
                            contested_reply.Insert(EO_EncodeNumber(2, 1), contested_reply.Length() + 1);
                            Client_SendEncoded(caster, AC_RESULT, OP_M_NPC, contested_reply);
                            return true;
                        }
                        int npc_damage = 0;
                        int npc_hit_rate = GUI->game_control->Combat_CalcArmorPen(caster->accuracy, (*iter)->evade,
                            0.9);
                        if (random(100) < npc_hit_rate)
                        {
                            MapCoord npc_dmg = GUI->skill_values->GetDamage(target_spell_id);
                            int npc_min_total = npc_dmg.min_damage + caster->min_damage;
                            int npc_max_total = npc_dmg.max_damage + caster->max_damage;
                            npc_hit_rate = GUI->game_control->Combat_CalcArmorPen((npc_min_total + npc_max_total) / 2,
                                (*iter)->armor, 0.8);
                            double scaled_damage = npc_min_total;
                            if (scaled_damage < 1.0)
                                scaled_damage = 1.0;
                            scaled_damage = 0.01L * scaled_damage;
                            scaled_damage = npc_hit_rate * scaled_damage;
                            scaled_damage = random(npc_max_total - npc_min_total + 2) + scaled_damage;
                            npc_damage = scaled_damage;
                            if (npc_damage < 1)
                                npc_damage = 1;
                        }
                        if (target_spell_id > 0)
                        {
                            MapCoord npc_element = GUI->skill_values->GetElement(target_spell_id);
                            if (npc_element.element == ElementKind_Light)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[1], (*iter)->element_weakness_damage_table[2]) *
                                    npc_damage;
                            if (npc_element.element == ElementKind_Dark)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[2], (*iter)->element_weakness_damage_table[1]) *
                                    npc_damage;
                            if (npc_element.element == ElementKind_Earth)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[3], (*iter)->element_weakness_damage_table[6]) *
                                    npc_damage;
                            if (npc_element.element == ElementKind_Wind)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[4], (*iter)->element_weakness_damage_table[3]) *
                                    npc_damage;
                            if (npc_element.element == ElementKind_Water)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[5], (*iter)->element_weakness_damage_table[4]) *
                                    npc_damage;
                            if (npc_element.element == ElementKind_Fire)
                                npc_damage = GUI->game_control->Combat_CalcElementMult(npc_element,
                                    caster->element_resistances[6], (*iter)->element_weakness_damage_table[5]) *
                                    npc_damage;
                        }
                        if ((*iter)->boss > 0)
                            map_control->AggroChildNpcs(caster->map_id);
                        (*iter)->aggressive = true;
                        (*iter)->leash_timer = random(50) + 100;
                        if (map_control->CountNpcsChasingPlayer(caster->map_id, caster->player_id) < 2 &&
                            !type_info.behavior_id)
                            (*iter)->chase_target_id = caster->player_id;
                        (*iter)->hp -= npc_damage;
                        (*iter)->hp_percent = 100 * (*iter)->hp / GUI->npc_values->GetMaxHp((*iter)->id);
                        if ((*iter)->hp < 1)
                        {
                            int exp_gain = GUI->npc_values->GetExp((*iter)->id);
                            int drop_index = 0;
                            exp_gain = Party_ShareExp(caster, exp_gain);
                            if ((*iter)->drop_item_id > 0 && (*iter)->drop_amount > 0)
                                drop_index = map_control->AddGroundItem(caster->map_id, (*iter)->drop_item_id,
                                    (*iter)->x, (*iter)->y, (*iter)->drop_amount, caster->account_ident, 61);
                            TDateTime now = Now();
                            (*iter)->death_stamp = DateTimeToTimeStamp(now);
                            (*iter)->chase_target_id = -1;
                            (*iter)->alive = false;
                            if ((*iter)->boss > 0 && map_control->KillChildNpcs(caster->map_id))
                                Server_BroadcastToMap(caster->map_id, AC_JUNK, OP_NPC,
                                    EO_EncodeNumber(map_control->maps[caster->map_id - 1].child_npc_id, 2));
                            String kill_reply = EO_EncodeNumber(target_spell_id, 2);
                            kill_reply.Insert(EO_EncodeNumber(caster->player_id, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(caster->direction, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*iter)->index, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(drop_index, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*iter)->drop_item_id, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*iter)->x, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*iter)->y, 1), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber((*iter)->drop_amount, 4), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(npc_damage, 3), kill_reply.Length() + 1);
                            if (settings->GetMaxKills())
                            {
                                if (kill_counters->IncrementAndGet(caster->name) > settings->GetMaxKills())
                                    exp_gain = 0;
                            }
                            caster->experience += exp_gain;
                            if (players->Player_TryLevelUp(caster) > 0)
                            {
                                Server_BroadcastNearTile(caster->player_id, caster->map_id, coords, AC_CONFIRM,
                                    OP_M_NPC, kill_reply);
                                kill_reply.Insert(EO_EncodeNumber(caster->tp, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->experience, 4), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->level, 1), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->stat_points, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->skill_points, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->max_hp, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->max_tp, 2), kill_reply.Length() + 1);
                                kill_reply.Insert(EO_EncodeNumber(caster->max_sp, 2), kill_reply.Length() + 1);
                                Client_SendEncoded(caster, AC_CONFIRM, OP_M_NPC, kill_reply);
                                return true;
                            }
                            Server_BroadcastNearTile(caster->player_id, caster->map_id, coords, AC_HIT,
                                OP_M_NPC, kill_reply);
                            kill_reply.Insert(EO_EncodeNumber(caster->tp, 2), kill_reply.Length() + 1);
                            kill_reply.Insert(EO_EncodeNumber(caster->experience, 4), kill_reply.Length() + 1);
                            if (!caster->cheater_flag)
                                Client_SendEncoded(caster, AC_HIT, OP_M_NPC, kill_reply);
                            else if (random(6) > 2)
                                caster->experience -= exp_gain;
                            Player_FireQuestTriggers(caster, QuestCondition_KilledNpcs, (*iter)->id);
                            return true;
                        }
                        String npc_hit_reply = EO_EncodeNumber(target_spell_id, 2);
                        npc_hit_reply.Insert(EO_EncodeNumber(caster->player_id, 2), npc_hit_reply.Length() + 1);
                        npc_hit_reply.Insert(EO_EncodeNumber(caster->direction, 1), npc_hit_reply.Length() + 1);
                        npc_hit_reply.Insert(EO_EncodeNumber((*iter)->index, 2), npc_hit_reply.Length() + 1);
                        npc_hit_reply.Insert(EO_EncodeNumber(npc_damage, 3), npc_hit_reply.Length() + 1);
                        if (!caster->cheater_flag)
                            npc_hit_reply.Insert(EO_EncodeNumber((short)(*iter)->hp_percent, 2),
                                npc_hit_reply.Length() + 1);
                        else if ((short)(*iter)->hp_percent < 1)
                            npc_hit_reply.Insert(EO_EncodeNumber(random(2) + 1, 2), npc_hit_reply.Length() + 1);
                        Server_BroadcastNearTile(caster->player_id, caster->map_id, coords, AC_RESULT,
                            OP_M_NPC, npc_hit_reply);
                        npc_hit_reply.Insert(EO_EncodeNumber(caster->tp, 2), npc_hit_reply.Length() + 1);
                        npc_hit_reply.Insert(EO_EncodeNumber(1, 1), npc_hit_reply.Length() + 1);
                        Client_SendEncoded(caster, AC_RESULT, OP_M_NPC, npc_hit_reply);
                        return true;
                    }
                    return true;
                }
            }
            return true;
        }
    }
    if (action == AC_GROUP)
    {
        if (!caster->logged_in)
            return false;
        if (caster->sitting || caster->on_chair)
            return true;
        if ((*data).Length() < 5)
            return false;
        int group_spell_id = EO_DecodeNumber((*data).SubString(1, 2));
        int group_cast_tick = EO_DecodeNumber((*data).SubString(3, 3));
        int group_target_type = GUI->skill_values->GetTargetType(group_spell_id);
        if (group_spell_id != caster->queued_spell_id)
            return false;
        if (group_cast_tick < caster->expected_cast_timestamp &&
            (group_cast_tick > 1000 || caster->expected_cast_timestamp < 8638000))
            return false;
        // .. a group heal: the party members on the map
        if (group_target_type != 3)
            return true;
        if (!players->Player_HasSpellId(caster, group_spell_id))
            return true;
        int spell_type = GUI->skill_values->GetSkillType(group_spell_id);
        int group_tp_cost = GUI->skill_values->GetTpCost(group_spell_id);
        int group_hp_heal = GUI->skill_values->GetHpHeal(group_spell_id);
        if (caster->tp < group_tp_cost)
        {
            String reply = EO_EncodeNumber(group_spell_id, 2);
            reply.Insert(EO_EncodeNumber(caster->hp, 2), reply.Length() + 1);
            reply.Insert(EO_EncodeNumber(caster->tp, 2), reply.Length() + 1);
            Client_SendEncoded(caster, AC_RESULT, OP_MAGIC, reply);
            return true;
        }
        if (group_hp_heal < 1)
            return true;
        if (!caster->in_party)
            return true;
        caster->tp -= group_tp_cost;
        String group_reply = EO_EncodeNumber(group_spell_id, 2);
        group_reply.Insert(EO_EncodeNumber(caster->player_id, 2), group_reply.Length() + 1);
        group_reply.Insert(EO_EncodeNumber(caster->tp, 2), group_reply.Length() + 1);
        group_reply.Insert(EO_EncodeNumber(group_hp_heal, 2), group_reply.Length() + 1);
        for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
        {
            Player *group_target = players->GetById(caster->party_ids[i]);
            if (group_target && group_target->map_id == caster->map_id)
            {
                group_target->hp += group_hp_heal;
                if (group_target->hp > group_target->max_hp)
                    group_target->hp = group_target->max_hp;
                int group_hp_percent = 100 * group_target->hp / group_target->max_hp;
                group_reply.Insert(EO_EncodeNumber(group_target->player_id, 2), group_reply.Length() + 1);
                group_reply.Insert(EO_EncodeNumber(group_hp_percent, 1), group_reply.Length() + 1);
                group_reply.Insert(EO_EncodeNumber(group_target->hp, 2), group_reply.Length() + 1);
            }
        }
        Server_BroadcastToMap(caster->map_id, AC_GROUP, OP_MAGIC, group_reply);
        return true;
    }
    if (action == AC_SELF)
    {
        if (!caster->logged_in)
            return false;
        if (caster->sitting || caster->on_chair)
            return true;
        if ((*data).Length() < 6)
            return false;
        int self_spell_id = EO_DecodeNumber((*data).SubString(2, 2));
        int self_cast_tick = EO_DecodeNumber((*data).SubString(4, 3));
        int self_target_type = GUI->skill_values->GetTargetType(self_spell_id);
        if (self_spell_id != caster->queued_spell_id)
            return false;
        if (self_cast_tick < caster->expected_cast_timestamp &&
            (self_cast_tick > 1000 || caster->expected_cast_timestamp < 8638000))
            return false;
        // .. a heal on themselves
        if (self_target_type != 1)
            return true;
        if (!players->Player_HasSpellId(caster, self_spell_id))
            return true;
        int self_skill_type = GUI->skill_values->GetSkillType(self_spell_id);
        int self_tp_cost = GUI->skill_values->GetTpCost(self_spell_id);
        int self_hp_heal = GUI->skill_values->GetHpHeal(self_spell_id);
        if (caster->tp < self_tp_cost)
        {
            String reply = EO_EncodeNumber(self_spell_id, 2);
            reply.Insert(EO_EncodeNumber(caster->hp, 2), reply.Length() + 1);
            reply.Insert(EO_EncodeNumber(caster->tp, 2), reply.Length() + 1);
            Client_SendEncoded(caster, AC_RESULT, OP_MAGIC, reply);
            return true;
        }
        if (self_skill_type)
            return true;
        caster->hp += self_hp_heal;
        caster->tp -= self_tp_cost;
        if (caster->hp > caster->max_hp)
            caster->hp = caster->max_hp;
        int self_hp_percent = 100 * caster->hp / caster->max_hp;
        String self_reply = EO_EncodeNumber(caster->player_id, 2);
        self_reply.Insert(EO_EncodeNumber(self_spell_id, 2), self_reply.Length() + 1);
        self_reply.Insert(EO_EncodeNumber(self_hp_heal, 4), self_reply.Length() + 1);
        self_reply.Insert(EO_EncodeNumber(self_hp_percent, 1), self_reply.Length() + 1);
        Server_BroadcastNearby(caster, AC_SELF, OP_MAGIC, self_reply);
        self_reply.Insert(EO_EncodeNumber(caster->hp, 2), self_reply.Length() + 1);
        self_reply.Insert(EO_EncodeNumber(caster->tp, 2), self_reply.Length() + 1);
        Client_SendEncoded(caster, AC_SELF, OP_MAGIC, self_reply);
        return true;
    }
    if (action == AC_DO)
    {
        if (!caster->logged_in)
            return false;
        if (caster->sitting || caster->on_chair)
            return true;
        if ((*data).Length() < 1)
            return false;
        caster->direction = EO_DecodeNumber(String((*data)[1]));
        String face_reply = EO_EncodeNumber(caster->player_id, 2);
        face_reply = face_reply + (*data)[1];
        Server_BroadcastNearby(caster, AC_SET, OP_MAGIC, face_reply);
        return true;
    }
    return false;
}

// A step: Player walks, Spec walks through players ( the ghost budget ), Admin through walls ( game master and up ).
bool Packets::Walk_Execute(Player *player, int action, String *data)
{
    player->last_action_tick = DateTimeToTimeStamp(Now());
    if (player->map_id < 1)
        return true;
    if (action != AC_SET && action != AC_HIT && action != AC_CAST)
        return false;
    if (action == AC_CAST && player->admin_level < AdminLevel_GameMaster)
        return false;
    if (action == AC_HIT)
    {
        if (player->ghost_walk_tokens <= 0)
            return true;
        player->ghost_walk_tokens--;
    }
    if ((*data).Length() < 6)
        return false;
    int client_tick = EO_DecodeNumber((*data).SubString(2, 3));
    int tick_delta = client_tick - player->last_client_tick;
    if (tick_delta < 0 && player->last_client_tick > CLIENT_TICK_WRAP_MIN)
        tick_delta = 44;
    player->last_client_tick = client_tick;
    // the client's clock: 44 hundredths of a second at least between steps
    if (tick_delta < 44)
        return false;
    TTimeStamp stamp;
    stamp = DateTimeToTimeStamp(Now());
    int sync_tick = stamp.Time / 10 + 100;
    int clock_drift;
    if (client_tick > sync_tick)
    {
        clock_drift = client_tick - sync_tick;
        if (player->sync_base_ahead < 0)
            player->sync_base_ahead = clock_drift;
        if (abs(clock_drift - player->sync_base_ahead) > 800)
            return true;
    }
    else
    {
        clock_drift = sync_tick - client_tick;
        if (player->sync_base_behind < 0)
            player->sync_base_behind = clock_drift;
        if (abs(clock_drift - player->sync_base_behind) > 800)
            return true;
    }
    int direction = EO_DecodeNumber(String((*data)[1]));
    int target_x = EO_DecodeNumber(String((*data)[5]));
    int target_y = EO_DecodeNumber(String((*data)[6]));
    if (direction > FacingDirection_Right)
        return true;
    // a step onto their own tile only turns them
    if (player->x == target_x && player->y == target_y)
    {
        if (player->direction != direction && (!player->on_chair && !player->sitting))
        {
            player->direction = direction;
            String packet = EO_EncodeNumber(player->player_id, 2);
            packet.Insert(EO_EncodeNumber(direction, 1), packet.Length() + 1);
            packet.Insert(EO_EncodeNumber(player->x, 1), packet.Length() + 1);
            packet.Insert(EO_EncodeNumber(player->y, 1), packet.Length() + 1);
            Server_BroadcastNearby(player, AC_SET, OP_MOVE, packet);
        }
        return true;
    }
    if (direction > FacingDirection_Right)
        direction = FacingDirection_Right;
    int coord_delta = player->x + player->y - (target_x + target_y);
    // the client is too far from where the server has them: a refresh
    if (coord_delta > 3 || coord_delta < -3 || (player->on_chair || player->sitting))
    {
        player->flush_queue = true;
        if (!player->cheater_flag)
            Client_SendEncoded(player, AC_RESULT, OP_REFRESH, Refresh_BuildReply(player));
        return true;
    }
    if (direction == FacingDirection_Down)
    {
        target_y = player->y + 1;
        target_x = player->x;
    }
    if (direction == FacingDirection_Left)
    {
        target_x = player->x - 1;
        target_y = player->y;
    }
    if (direction == FacingDirection_Up)
    {
        target_y = player->y - 1;
        target_x = player->x;
    }
    if (direction == FacingDirection_Right)
    {
        target_x = player->x + 1;
        target_y = player->y;
    }
    if (map_control->IsOccupied(player->map_id, target_x, target_y) && action == AC_SET)
        return true;
    // .. a player in the way: through them once each 7 seconds, never onto an arena tile
    if (players->IsPlayerAt(player->map_id, target_x, target_y))
    {
        if (action == AC_SET)
        {
            if (!player->cheater_flag)
                Client_SendEncoded(player, AC_RESULT, OP_REFRESH, Refresh_BuildReply(player));
            return true;
        }
        if (player->admin_level < AdminLevel_Guardian)
        {
            TTimeStamp now = DateTimeToTimeStamp(Now());
            int elapsed = now.Date - player->last_pass_ms.Date;
            int ms = now.Time - player->last_pass_ms.Time;
            elapsed = SECONDS_PER_DAY * elapsed + ms / MS_PER_SECOND;
            player->last_pass_ms = now;
            if (elapsed < 7)
                return true;
            if (map_control->GetTileSpec(player->map_id, target_x, target_y) == TileSpec_Arena)
            {
                Client_SendEncoded(player, AC_RESULT, OP_REFRESH, Refresh_BuildReply(player));
                return true;
            }
        }
    }
    int tile_kind = map_control->GetTileBlock(player->map_id, target_x, target_y, 1);
    // .. the step: the new things in view, and the players near them see it
    if (!tile_kind || (tile_kind == 1 && action == AC_CAST))
    {
        player->direction = direction;
        player->x = target_x;
        player->y = target_y;
        player->idle_ticks = 0;
        String step_reply = Walk_BuildReply(player);
        if (step_reply.Length() > 1)
            Client_SendEncoded(player, AC_RESULT, OP_MOVE, step_reply);
        String walk = EO_EncodeNumber(player->player_id, 2);
        walk.Insert(EO_EncodeNumber(direction, 1), walk.Length() + 1);
        walk.Insert(EO_EncodeNumber(target_x, 1), walk.Length() + 1);
        walk.Insert(EO_EncodeNumber(target_y, 1), walk.Length() + 1);
        Server_BroadcastNearby(player, AC_SET, OP_MOVE, walk);
        if (player->map_has_spikes)
        {
            int spec = map_control->GetTileSpec(player->map_id, player->x, player->y);
            // spikes take a fifth of the max hp
            if (spec == 34 || spec == 35)
            {
                int damage = player->max_hp / 5;
                unsigned int died = 0;
                if (damage < 1)
                    damage = 1;
                player->hp -= damage;
                if (player->hp <= 0)
                {
                    player->hp = 0;
                    died = 1;
                }
                String dmg = EO_EncodeNumber(MapDamageType_Spikes, 1);
                dmg.Insert(EO_EncodeNumber(damage, 2), dmg.Length() + 1);
                dmg.Insert(EO_EncodeNumber(player->hp, 2), dmg.Length() + 1);
                dmg.Insert(EO_EncodeNumber(player->max_hp, 2), dmg.Length() + 1);
                Client_SendEncoded(player, AC_HIT, OP_EFFECT, dmg);
                dmg = EO_EncodeNumber(player->player_id, 2);
                dmg.Insert(EO_EncodeNumber(player->HpPercent(), 1), dmg.Length() + 1);
                dmg.Insert(EO_EncodeNumber(died, 1), dmg.Length() + 1);
                dmg.Insert(EO_EncodeNumber(damage, 2), dmg.Length() + 1);
                Server_BroadcastNearby(player, AC_CAST, OP_EFFECT, dmg);
                if (died)
                {
                    Player_Respawn(player);
                    return true;
                }
            }
        }
        Player_FireQuestTriggers(player, QuestCondition_EnterCoord, 0);
        return true;
    }
    // .. a warp, when their level is enough
    if (tile_kind == 2)
    {
        int target_map = map_control->GetWarpMap(player->map_id, target_x, target_y);
        int level_req = map_control->GetWarpLevelReq(player->map_id, target_x, target_y);
        int warp_x = map_control->GetWarpX(player->map_id, target_x, target_y);
        int warp_y = map_control->GetWarpY(player->map_id, target_x, target_y);
        if (player->level < level_req)
            return true;
        if (target_map > 0 && (int)map_control->maps.size() >= target_map)
        {
            if (map_control->maps[target_map - 1].width < 1 || map_control->maps[target_map - 1].height < 1)
                return true;
            if (player->warp_effect < 0)
                player->session_id = random(50000) + 10000;
            player->warp_effect = 0;
            player->warp_pending = true;
            player->dead = false;
            player->warp_map = target_map;
            player->warp_x = warp_x;
            player->warp_y = warp_y;
            if (target_map == player->map_id)
            {
                String same_map_reply = EO_EncodeNumber(WarpType_Local, 1);
                same_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].map_id, 2),
                    same_map_reply.Length() + 1);
                same_map_reply.Insert(EO_EncodeNumber(player->session_id, 2), same_map_reply.Length() + 1);
                Client_SendEncoded(player, AC_REQUEST, OP_MAP, same_map_reply);
                return true;
            }
            else
            {
                String new_map_reply = EO_EncodeNumber(WarpType_MapSwitch, 1);
                new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].map_id, 2),
                    new_map_reply.Length() + 1);
                new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].rid1, 2),
                    new_map_reply.Length() + 1);
                new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].rid2, 2),
                    new_map_reply.Length() + 1);
                new_map_reply.Insert(EO_EncodeNumber(map_control->maps[target_map - 1].filesize, 3),
                    new_map_reply.Length() + 1);
                new_map_reply.Insert(EO_EncodeNumber(player->session_id, 2), new_map_reply.Length() + 1);
                Client_SendEncoded(player, AC_REQUEST, OP_MAP, new_map_reply);
                Player_FireQuestTriggers(player, QuestCondition_LeaveMap, player->map_id);
                return true;
            }
        }
    }
    return true;
}

// Pings every connected player with a new sequence value; a player who has not answered the last two is removed.
void Packets::Connection_Ping()
{
    randomize();
    char is_new = 0;
    int value = 0;
    // .. a new value, 10 to 211, that is not one of the last three
    while (is_new == 0)
    {
        is_new = 1;
        value = random(202) + 10;
        for (int i = 0; i < PING_HISTORY_SIZE; i++)
        {
            int recent = ping_history[i];
            if (value == recent)
                is_new = 0;
        }
    }
    for (int history_slot = 2; history_slot >= 1; history_slot--)
        ping_history[history_slot] = ping_history[history_slot - 1];
    ping_history[0] = value;
    // the value goes out as value + offset and the offset
    unsigned int ping_offset = random(202) + 10;
    unsigned int sum = ping_history[0] + ping_offset;
    String encoded = EO_EncodeNumber(sum, 2);
    encoded.Insert(EO_EncodeNumber(ping_offset, 1), encoded.Length() + 1);
    vector<Player *>::iterator it;
    for (it = players->players.begin(); it != players->players.end(); it++)
    {
        if ((*it)->ping_timeout > 1)
        {
            (*it)->removing = true;
            players->MarkDirty();
        }
        else if ((*it)->connected)
        {
            (*it)->ping_timeout++;
            Client_SendEncoded(*it, AC_SET, OP_SECURITY, encoded);
        }
    }
}

// The answer to a client's init challenge.
int Packets::Server_HashChallenge(int value)
{
    value++;
    int modulus_factor = value % 11 + 1;
    int scale_factor = value % 9 + 1;
    int final_offset = value % 2004 + 1;
    value = 11092004 - value;
    value = value % (modulus_factor * 119);
    value = scale_factor * value * 119;
    value = final_offset + value + 110904;
    return value;
}

// A password as it is kept: reversed, and the digits and lower case letters mirrored.
String Packets::Account_DecodePassword(String value)
{
    String reversed = "";
    String result = "";
    try
    {
        for (int i = value.Length(); i >= 1; i--)
            reversed = reversed + value[i];
        for (int j = 1; j <= value.Length(); j++)
        {
            unsigned char letter = reversed[j];
            unsigned char ch = letter;
            int code = ch;
            char handled = 0;
            if (!handled && code >= 48 && code <= 57)
            {
                code = 57 - code + 48;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                handled = 1;
            }
            if (!handled && code >= 97 && code <= 122)
            {
                code = 122 - code + 97;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                handled = 1;
            }
            if (!handled)
                result.Insert(String((char)letter), result.Length() + 1);
        }
    }
    catch (...)
    {
    }
    return result;
}

// A password the way it is kept ( the same mirror as the decode ).
String Packets::Account_EncodePassword(String value)
{
    String reversed = "";
    String result = "";
    try
    {
        for (int i = value.Length(); i >= 1; i--)
            reversed = reversed + value[i];
        for (int j = 1; j <= value.Length(); j++)
        {
            unsigned char letter = reversed[j];
            unsigned char ch = letter;
            int code = ch;
            char handled = 0;
            if (!handled && code >= 48 && code <= 57)
            {
                code = 57 - code + 48;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                handled = 1;
            }
            if (!handled && code >= 97 && code <= 122)
            {
                code = 122 - code + 97;
                ch = code;
                letter = ch;
                result.Insert(String((char)letter), result.Length() + 1);
                handled = 1;
            }
            if (!handled)
                result.Insert(String((char)letter), result.Length() + 1);
        }
    }
    catch (...)
    {
    }
    return result;
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
String Packets::EO_EncodeNumber(unsigned int value, signed int width)
{
    int rem;
    char digit;
    try
    {
        unsigned int quotient = 1;
        char flag = 1;
        for (int i = 0; i < width; i++)
        {
            if (flag)
            {
                double scaled = value * EO_NUMBER_BASE_INVERSE;
                quotient = scaled;
                rem = value % EO_NUMBER_BASE;
                digit = rem + 1;
                encode_buffer[i] = digit;
                value = quotient;
                if (quotient < 1)
                    flag = 0;
                else
                {
                    if (i + 1 == width)
                        width++;
                }
            }
            else
            {
                char pad = EO_NUMBER_EMPTY_CHAR;
                encode_buffer[i] = pad;
            }
        }
    }
    catch (...)
    {
        width = 0;
    }
    String result(encode_buffer, width);
    return result;
}

// A packet's length from its first two bytes.
unsigned int Packets::Server_DecodePacketLength(String data)
{
    int result = 0;
    try
    {
        for (int i = 1; i <= data.Length(); i++)
        {
            char raw_char = data[i];
            unsigned char ch = raw_char;
            if (ch == EO_NUMBER_EMPTY || !ch)
                break;
            int digit = ch;
            digit--;
            if (i == 1)
                result += digit;
            if (i == 2)
                result += EO_NUMBER_BASE * digit;
        }
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

// Reads a number of up to four bytes; a 254 ends it.
unsigned int Packets::EO_DecodeNumber(String data)
{
    int result = 0;
    try
    {
        for (int i = 1; i <= data.Length(); i++)
        {
            char raw_char = data[i];
            unsigned char ch = raw_char;
            if (ch == EO_NUMBER_EMPTY || !ch)
                break;
            int digit = ch;
            digit--;
            if (i == 1)
                result += digit;
            if (i == 2)
                result += EO_NUMBER_BASE * digit;
            if (i == 3)
                result += EO_NUMBER_BASE_SQUARED * digit;
            if (i == 4)
                result += EO_NUMBER_BASE_CUBED * digit;
        }
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}

// Scrambles a packet the protocol's way: runs of bytes that are a multiple of multiple are reversed,
// then the bytes are taken first, last, second, second to last, and so on
String Packets::EO_Encode_Interleave(int multiple, char *begin, char *end)
{
    unsigned int len = 0;
    try
    {
        std::stack<char > pending;
        std::deque<char > woven;
        for (; begin != end; begin++)
        {
            unsigned char input_byte = *begin;
            int value = input_byte;
            if (!(value % multiple))
                pending.push(*begin);
            else
            {
                while (!pending.empty())
                {
                    woven.push_back(pending.top());
                    pending.pop();
                }
                woven.push_back(*begin);
            }
        }
        while (!pending.empty())
        {
            woven.push_back(pending.top());
            pending.pop();
        }
        // .. first, last, second, second to last, ..
        while (!woven.empty())
        {
            if (!woven.empty())
            {
                packet_buffer[len] = woven.front();
                woven.pop_front();
                len++;
            }
            if (!woven.empty())
            {
                packet_buffer[len] = woven.back();
                woven.pop_back();
                len++;
            }
        }
    }
    catch (...)
    {
        len = 0;
    }
    String data(packet_buffer, len);
    return data;
}

// Undoes the scramble of a client's packet: the bytes back in order, then the runs of multiples reversed again.
String Packets::EO_Decode_Deinterleave(int multiple, char *begin, char *end)
{
    unsigned int len = 0;
    if (multiple > 0)
    {
        try
        {
            std::stack<char > pending;
            std::queue<char > woven;
            char toggle = 1;
            for (; begin != end; begin++)
            {
                if (toggle)
                {
                    woven.push(*begin);
                    toggle = 0;
                }
                else
                {
                    pending.push(*begin);
                    toggle = 1;
                }
            }
            while (!pending.empty())
            {
                woven.push(pending.top());
                pending.pop();
            }
            pending.empty();
            while (!woven.empty())
            {
                char next_char = woven.front();
                woven.pop();
                unsigned char raw = next_char;
                int value = raw;
                if (!(value % multiple))
                    pending.push(next_char);
                else
                {
                    while (!pending.empty())
                    {
                        packet_buffer[len++] = pending.top();
                        pending.pop();
                    }
                    packet_buffer[len++] = next_char;
                }
            }
            while (!pending.empty())
            {
                packet_buffer[len++] = pending.top();
                pending.pop();
            }
        }
        catch (...)
        {
            len = 0;
        }
    }
    String data(packet_buffer, len);
    return data;
}

int Packets::EO_DecodeByte(unsigned char value)
{
    unsigned char byte_value = value;
    int result = byte_value;
    return result;
}

char Packets::EO_GetBreakByte(int value)
{
    char byte_value = value;
    char result = byte_value;
    return result;
}

// Counts the bytes sent, in bytes, kilobytes and megabytes.
void Packets::Server_AddSentBytes(int value)
{
    sent_bytes += value;
    for (; sent_bytes > 1023; sent_bytes -= 1024)
        sent_kilobytes++;
    while (sent_kilobytes > 1023)
    {
        sent_megabytes++;
        sent_kilobytes -= 1024;
    }
}

// Counts the bytes received, in bytes, kilobytes and megabytes.
void Packets::Server_AddReceivedBytes(int value)
{
    received_bytes += value;
    for (; received_bytes > 1023; received_bytes -= 1024)
        received_kilobytes++;
    while (received_kilobytes > 1023)
    {
        received_megabytes++;
        received_kilobytes -= 1024;
    }
}

// Starts reading data field by field, each field ending at the break byte.
void Packets::PacketReader_Init(String data, char break_byte)
{
    reader_pos = 1;
    reader_data = data;
    reader_len = data.Length();
    reader_break_byte = break_byte;
}

void Packets_UnusedLiterals1()
{
    "";
    "";
}

// The next field; the break byte after it is skipped.
String Packets::PacketReader_GetBreakString()
{
    String result = "";
    try
    {
        if (reader_len >= 1)
        {
            while (reader_pos <= reader_len)
            {
                if (reader_data[reader_pos] != reader_break_byte)
                    result.Insert(reader_data[reader_pos], result.Length() + 1);
                else
                {
                    reader_pos++;
                    break;
                }
                reader_pos++;
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// The field at field_index ( from 1 ) of the text, the fields split by separator.
String Packets::PacketReader_GetBreakStringAt(int field_index, String break_str, char separator)
{
    String result = "";
    try
    {
        if (break_str.Length() >= 1)
        {
            int i = 1;
            int break_length = break_str.Length();
            int field_number = 1;
            String x = "";
            if (i <= break_length)
            {
                do
                {
                    if (field_number == field_index)
                    {
                        if (break_str[i] != separator)
                            result.Insert(break_str[i], result.Length() + 1);
                    }
                    if (field_number <= field_index)
                    {
                        if (break_str[i] == separator)
                            field_number++;
                    }
                    if (field_number > field_index)
                        break;
                    i++;
                } while (i <= break_length);
            }
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}

// False when the name holds a word of the word filter.
bool Packets::PassesWordFilter(String name)
{
    for (int i = 0; i < wordfilter->Count; i++)
    {
        if (wordfilter->Strings[i].Length() <= name.Length())
        {
            for (int j = 1; j <= name.Length(); j++)
            {
                if (wordfilter->Strings[i][1] == name[j])
                {
                    if (name.SubString(j, wordfilter->Strings[i].Length()) == wordfilter->Strings[i])
                        return false;
                }
            }
        }
    }
    return true;
}

// True when more than 20 queries wait for the database.
bool Packets::Login_CheckQueueThreshold()
{
    if (mysql_controls->Db_GetQueuedQueryCount() > 20)
        return true;
    return false;
}

// 1 when the two spots are the same or side by side.
char Packets::Coords_IsAdjacent(int x1, int y1, int x2, int y2)
{
    char result = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 1)
        result = 1;
    return result;
}

// 1 when the second spot is in view of the first: 15 steps to the lower right, 12 any other way.
char Packets::Server_InViewRange(int x1, int y1, int x2, int y2)
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

// 1 when the second spot is at the edge of the view: 14 or 15 steps to the lower right, 11 or 12 any other way.
char Packets::Server_InViewRing(int x1, int y1, int x2, int y2)
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
        if (dx + dy <= 15 && dx + dy > 13)
            result = 1;
    }
    else if (dx + dy <= 12 && dx + dy > 10)
        result = 1;
    return result;
}

// 1 when the first spot is in view of the second: 15 steps to the upper left, 12 any other way.
char Packets::Server_InViewRangeReverse(int x1, int y1, int x2, int y2)
{
    char result = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (x2 < x1 && y2 < y1)
    {
        if (dx + dy <= 15)
            result = 1;
    }
    else if (dx + dy <= 12)
        result = 1;
    return result;
}

// 1 when the two spots are at most 2 steps apart.
char Packets::Coords_IsWithinTwo(int x1, int y1, int x2, int y2)
{
    char result = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 2)
        result = 1;
    return result;
}

// 1 when the second spot is 12 or 13 steps away, where items come into view.
char Packets::Server_InItemViewRing(int x1, int y1, int x2, int y2)
{
    char result = 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 13 && dx + dy > 11)
        result = 1;
    return result;
}

// The bytes sent, as the panel shows them: "1.23 Mb", "4.56 Kb" or "n/a".
String Packets::Server_FormatSentTraffic()
{
    if (sent_megabytes > 0)
        return IntToStr(sent_megabytes) + "." + IntToStr(sent_kilobytes / 103).SubString(1, 2) + " Mb";
    else
    {
        if (sent_kilobytes > 0)
            return IntToStr(sent_kilobytes) + "." + IntToStr(sent_bytes / 103).SubString(1, 2) + " Kb";
        else
            return "n/a";
    }
}

// The bytes received, as the panel shows them: "1.23 Mb", "4.56 Kb" or "n/a".
String Packets::Server_FormatReceivedTraffic()
{
    if (received_megabytes > 0)
        return IntToStr(received_megabytes) + "." + IntToStr(received_kilobytes / 103).SubString(1, 2) + " Mb";
    else
    {
        if (received_kilobytes > 0)
            return IntToStr(received_kilobytes) + "." + IntToStr(received_bytes / 103).SubString(1, 2) + " Kb";
        else
            return "n/a";
    }
}

// 1 once more than 5 seconds have passed since it last said 1; the guild and marriage requests wait on it.
char Packets::Server_TickOncePerFiveSeconds()
{
    TTimeStamp stamp;
    TTimeStamp now;
    stamp = DateTimeToTimeStamp(start_time);
    now = DateTimeToTimeStamp(Now());
    int days = now.Date - stamp.Date;
    int millis = now.Time - stamp.Time;
    int elapsed = millis / MS_PER_SECOND + days * SECONDS_PER_DAY;
    if (elapsed > 5)
    {
        start_time = Now();
        return 1;
    }
    return 0;
}

// Adds a line to logs/chat<nn>.log; after 30000 lines the next of 99 files starts, emptied first.
void Packets::Server_AppendChatLog(String message)
{
    FILE *fp;
    String line = DateToStr(Now());
    line.Insert(" ", line.Length() + 1);
    line.Insert(TimeToStr(Now()), line.Length() + 1);
    line.Insert(" ", line.Length() + 1);
    line.Insert(message, line.Length() + 1);
    line.Insert("\n", line.Length() + 1);
    String log_number = IntToStr(chat_log_number);
    if (log_number.Length() < 2)
        log_number = "0" + log_number;
    String path = ".\\logs\\chat" + log_number + ".log";
    fp = fopen(path.c_str(), "a");
    fprintf(fp, "%s", line.c_str());
    fclose(fp);
    chat_log_lines++;
    if (chat_log_lines > 30000)
    {
        chat_log_number++;
        chat_log_lines = 1;
        if (chat_log_number > 99)
            chat_log_number = 1;
        log_number = IntToStr(chat_log_number);
        if (log_number.Length() < 2)
            log_number = "0" + log_number;
        path = ".\\logs\\chat" + log_number + ".log";
        remove(path.c_str());
    }
}
