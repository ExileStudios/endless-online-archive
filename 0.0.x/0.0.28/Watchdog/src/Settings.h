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

// The Watchdog's settings, from config/server.ini.
struct Settings
{
    String server_name;
    int port;
    String mysql_host;
    int mysql_port;
    int max_players;
    int max_connections;
    int max_clones;
    int max_maps;
    bool watchdog;
    bool generator;
    bool downtime_log;
    int watch_interval;
    int generator_interval;
    String watch_app;
    String output_path;
    int popup_style;
    TStringList *ini_lines;

    Settings();
    ~Settings();
    String GetServerName();
    int GetPort();
    int GetMaxPlayers();
    int GetMaxConnections();
    bool GetWatchdog();
    bool GetGenerator();
    bool GetDowntimeLog();
    int GetWatchInterval();
    String GetWatchApp();
    String GetOutputPath();
    void LoadConfig();
    void SetIniPath(String path);
    void CloseIni();
    int ReadIniInt(String key, int default_value);
    String ReadIniString(String key, String default_value);
    bool ReadIniBool(String key, bool default_value);
    void SetGenerator(bool value);
    void SetWatchdog(bool value);
    void SetDowntimeLog(bool value);
};


#endif
