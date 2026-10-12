#include <vcl.h>
#pragma hdrstop

#include "Settings.h"
#include "Constants.h"

#pragma package(smart_init)

// The defaults, then config/server.ini over them.
Settings::Settings()
{
    ini_lines = new TStringList();
    server_name = "endl serv";
    mysql_host = "localhost";
    mysql_port = 3306;
    port = 8642;
    max_players = 100;
    max_connections = 100;
    max_clones = 2;
    watchdog = 1;
    generator = 0;
    downtime_log = 0;
    watch_interval = 10;
    generator_interval = 5;
    watch_app = "gameserver.exe";
    output_path = "";
    popup_style = 0;
    LoadConfig();
}

Settings::~Settings()
{
}

String Settings::GetServerName()
{
    return server_name;
}

int Settings::GetPort()
{
    return port;
}

int Settings::GetMaxPlayers()
{
    return max_players;
}

int Settings::GetMaxConnections()
{
    return max_connections;
}

bool Settings::GetWatchdog()
{
    return watchdog;
}

bool Settings::GetGenerator()
{
    return generator;
}

bool Settings::GetDowntimeLog()
{
    return downtime_log;
}

int Settings::GetWatchInterval()
{
    return watch_interval;
}

String Settings::GetWatchApp()
{
    return watch_app;
}

String Settings::GetOutputPath()
{
    return output_path;
}

// Reads config/server.ini; a key that is not there keeps its value.
void Settings::LoadConfig()
{
    SetIniPath("./config/server.ini");
    server_name = ReadIniString("servername", server_name);
    mysql_host = ReadIniString("mysqlhost", mysql_host);
    mysql_port = ReadIniInt("mysqlport", mysql_port);
    port = ReadIniInt("port", port);
    max_players = ReadIniInt("maxplayers", max_players);
    max_connections = ReadIniInt("maxconnections", max_connections);
    max_clones = ReadIniInt("maxclones", max_clones);
    max_maps = ReadIniInt("maxmaps", max_maps);
    popup_style = ReadIniInt("popupstyle", popup_style);
    watchdog = ReadIniBool("watchdog", watchdog);
    generator = ReadIniBool("generator", generator);
    downtime_log = ReadIniBool("downtimelog", downtime_log);
    watch_interval = ReadIniInt("watchInterval", watch_interval);
    generator_interval = ReadIniInt("generatorinterval", generator_interval);
    watch_app = ReadIniString("watchapp", watch_app);
    output_path = ReadIniString("outputpath", output_path);
    // popupstyle is 0 or 1
    if (popup_style < 0)
        popup_style = 0;
    if (popup_style > 1)
        popup_style = 1;
    CloseIni();
}

DEFINE_SET_INI_PATH(Settings)

void Settings::CloseIni()
{
    ini_lines->Clear();
}

DEFINE_READ_INI_INT(Settings)

DEFINE_READ_INI_STRING(Settings)

DEFINE_READ_INI_BOOL(Settings)

void Settings::SetGenerator(bool value)
{
    generator = value;
}

void Settings::SetWatchdog(bool value)
{
    watchdog = value;
}

void Settings::SetDowntimeLog(bool value)
{
    downtime_log = value;
}
