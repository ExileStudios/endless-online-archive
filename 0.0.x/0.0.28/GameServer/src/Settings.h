#ifndef SettingsH
#define SettingsH

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



struct Settings;

// The server's settings, from config/server.ini.
struct Settings
{
    String welcome_message;
    String join_message;
    String server_name;
    int port;
    String mysql_host;
    int mysql_port;
    int login_protection;
    int max_players;
    int max_connections;
    int max_clones;
    int max_maps;
    int max_quests;
    int start_map;
    int start_x;
    int start_y;
    int rescue_map;
    int rescue_x;
    int rescue_y;
    int jail_map;
    int jail_x;
    int jail_y;
    int spy_and_light_guide_flood_rate;
    int guardian_flood_rate;
    int game_master_flood_rate;
    int high_game_master_flood_rate;
    char account_lock;
    char access_lock;
    int max_kills;
    char server_status;
    char memory_map;
    char sql_queue;
    char sql_smart;
    char db_shutdown;
    char mm_timer;
    int refresh_minutes;
    char world_communication;
    char chat_log;
    int group_max;
    int group_balance;
    int popup_style;
    TStringList *ini_lines;

    Settings();
    ~Settings();
    String GetJoinMessage();
    String GetServerName();
    int GetPort();
    int GetStartMap();
    int GetStartX();
    int GetStartY();
    int GetJailMap();
    int GetJailX();
    int GetJailY();
    int GetRescueMap();
    int GetRescueX();
    int GetRescueY();
    int GetMaxPlayers();
    int GetMaxConnections();
    int GetMaxClones();
    int GetMaxMaps();
    int GetMaxQuests();
    char GetAccountLock();
    char GetAccessLock();
    int GetMaxKills();
    char GetMemoryMap();
    char GetSqlSmart();
    char GetWorldCommunication();
    int GetRefreshSeconds();
    int GetGroupMax();
    int GetSpyAndLightGuideFloodRate();
    int GetGuardianFloodRate();
    int GetGameMasterFloodRate();
    int GetHighGameMasterFloodRate();
    char GetChatLog();
    void LoadConfig();
    void LoadIni(String path);
    void CloseIni();
    int ReadIniInt(String key, int default_value);
    String ReadIniString(String key, String default_value);
    char ReadIniBool(String key, char default_value);
    void SetWorldCommunication(char value);
};


#endif
