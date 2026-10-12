#include <vcl.h>
#pragma hdrstop

#include "Settings.h"

#pragma package(smart_init)

#define REFRESH_MINUTES_DEFAULT 30
#define REFRESH_MINUTES_MAX 30

// The defaults, then config/server.ini over them.
Settings::Settings()
{
    ini_lines = new TStringList();
    welcome_message = "";
    server_name = "endl serv";
    mysql_host = "localhost";
    mysql_port = 3306;
    port = 8642;
    login_protection = 6;
    max_players = 100;
    max_connections = 100;
    max_clones = 2;
    max_maps = 250;
    max_quests = 250;
    start_map = 1;
    start_x = 1;
    start_y = 1;
    rescue_map = 1;
    rescue_x = 1;
    rescue_y = 1;
    jail_map = 1;
    jail_x = 1;
    jail_y = 1;
    max_kills = 0;
    spy_and_light_guide_flood_rate = 10;
    guardian_flood_rate = 10;
    game_master_flood_rate = 10;
    high_game_master_flood_rate = 10;
    world_communication = 1;
    chat_log = 0;
    account_lock = 0;
    access_lock = 0;
    server_status = 0;
    memory_map = 0;
    sql_queue = 1;
    mm_timer = 0;
    refresh_minutes = REFRESH_MINUTES_DEFAULT;
    popup_style = 0;
    LoadConfig();
}

Settings::~Settings()
{
}

String Settings::GetJoinMessage()
{
    return join_message;
}

String Settings::GetServerName()
{
    return server_name;
}

int Settings::GetPort()
{
    return port;
}

int Settings::GetStartMap()
{
    return start_map;
}

int Settings::GetStartX()
{
    return start_x;
}

int Settings::GetStartY()
{
    return start_y;
}

int Settings::GetJailMap()
{
    return jail_map;
}

int Settings::GetJailX()
{
    return jail_x;
}

int Settings::GetJailY()
{
    return jail_y;
}

int Settings::GetRescueMap()
{
    return rescue_map;
}

int Settings::GetRescueX()
{
    return rescue_x;
}

int Settings::GetRescueY()
{
    return rescue_y;
}

int Settings::GetMaxPlayers()
{
    return max_players;
}

int Settings::GetMaxConnections()
{
    return max_connections;
}

int Settings::GetMaxClones()
{
    return max_clones;
}

int Settings::GetMaxMaps()
{
    return max_maps;
}

int Settings::GetMaxQuests()
{
    return max_quests;
}

char Settings::GetAccountLock()
{
    return account_lock;
}

char Settings::GetAccessLock()
{
    return access_lock;
}

int Settings::GetMaxKills()
{
    return max_kills;
}

char Settings::GetMemoryMap()
{
    return memory_map;
}

char Settings::GetSqlSmart()
{
    return sql_smart;
}

char Settings::GetWorldCommunication()
{
    return world_communication;
}

// refreshtime in seconds ( the ini gives minutes ).
int Settings::GetRefreshSeconds()
{
    return 60 * refresh_minutes;
}

int Settings::GetGroupMax()
{
    return group_max;
}

int Settings::GetSpyAndLightGuideFloodRate()
{
    return spy_and_light_guide_flood_rate;
}

int Settings::GetGuardianFloodRate()
{
    return guardian_flood_rate;
}

int Settings::GetGameMasterFloodRate()
{
    return game_master_flood_rate;
}

int Settings::GetHighGameMasterFloodRate()
{
    return high_game_master_flood_rate;
}

char Settings::GetChatLog()
{
    return chat_log;
}

// Reads config/server.ini; a key that is not there keeps its value, and the values are kept in range.
void Settings::LoadConfig()
{
    LoadIni("./config/server.ini");
    server_name = ReadIniString("servername", server_name);
    mysql_host = ReadIniString("mysqlhost", mysql_host);
    mysql_port = ReadIniInt("mysqlport", mysql_port);
    welcome_message = ReadIniString("welcomemessage", welcome_message);
    join_message = ReadIniString("joinmessage", join_message);
    port = ReadIniInt("port", port);
    max_players = ReadIniInt("maxplayers", max_players);
    max_connections = ReadIniInt("maxconnections", max_connections);
    max_clones = ReadIniInt("maxclones", max_clones);
    max_maps = ReadIniInt("maxmaps", max_maps);
    max_quests = ReadIniInt("maxquests", max_maps);
    start_map = ReadIniInt("startmap", start_map);
    start_x = ReadIniInt("startx", start_x);
    start_y = ReadIniInt("starty", start_y);
    rescue_map = ReadIniInt("rescuemap", rescue_map);
    rescue_x = ReadIniInt("rescuex", rescue_x);
    rescue_y = ReadIniInt("rescuey", rescue_y);
    jail_map = ReadIniInt("jailmap", jail_map);
    jail_x = ReadIniInt("jailx", jail_x);
    jail_y = ReadIniInt("jaily", jail_y);
    // .. adminlevel1 to 4: the flood rate of each admin level
    spy_and_light_guide_flood_rate = ReadIniInt("adminlevel1", spy_and_light_guide_flood_rate);
    guardian_flood_rate = ReadIniInt("adminlevel2", guardian_flood_rate);
    game_master_flood_rate = ReadIniInt("adminlevel3", game_master_flood_rate);
    high_game_master_flood_rate = ReadIniInt("adminlevel4", high_game_master_flood_rate);
    account_lock = ReadIniBool("accountlock", account_lock);
    access_lock = ReadIniBool("accesslock", access_lock);
    max_kills = ReadIniInt("maxkills", max_kills);
    server_status = ReadIniBool("serverstatus", server_status);
    memory_map = ReadIniBool("memorymap", memory_map);
    sql_queue = ReadIniBool("sqlqueque", sql_queue);
    sql_smart = ReadIniBool("sqlsmart", sql_smart);
    db_shutdown = ReadIniBool("dbshutdown", db_shutdown);
    mm_timer = ReadIniBool("mmtimer", mm_timer);
    refresh_minutes = ReadIniInt("refreshtime", refresh_minutes);
    popup_style = ReadIniInt("popupstyle", popup_style);
    group_max = ReadIniInt("groupmax", group_max);
    group_balance = ReadIniInt("groupbalance", group_balance);
    chat_log = ReadIniBool("chatlog", chat_log);
    // .. loginprotection: low = 8, high = 4, anything else 6
    String protection = ReadIniString("loginprotection", join_message);
    if (LowerCase(protection) == "low")
        login_protection = 8;
    if (LowerCase(protection) == "high")
        login_protection = 4;
    if (group_max > 10)
        group_max = 10;
    if (refresh_minutes > REFRESH_MINUTES_MAX)
        refresh_minutes = REFRESH_MINUTES_MAX;
    if (refresh_minutes < 1)
        refresh_minutes = 1;
    if (rescue_map < 1)
        rescue_map = 1;
    if (start_map < 1)
        start_map = 1;
    if (jail_map < 1)
        jail_map = 1;
    if (popup_style < 0)
        popup_style = 0;
    if (popup_style > 1)
        popup_style = 1;
    if (max_kills > 10000)
        max_kills = 10000;
    if (welcome_message.Length() > 250)
        welcome_message = welcome_message.SubString(1, 250);
    CloseIni();
}

// Loads an ini file, its path taken from the folder the exe is in.
void Settings::LoadIni(String path)
{
    ini_lines->Clear();
    try
    {
        int last_slash_pos;
        String exe = Application->ExeName;
        last_slash_pos = 0;
        if (exe.Length() >= 1)
        {
            int i = exe.Length();
            while (i >= 1)
            {
                if (exe[i] == '\\')
                {
                    last_slash_pos = i;
                    break;
                }
                i--;
            }
        }
        String dir = "";
        if (exe.Length() >= 1 && last_slash_pos >= 1)
        {
            for (int p = 1; p < last_slash_pos; p++)
                dir = dir + exe[p];
        }
        exe = dir;
        String tail;
        if (exe[exe.Length()] != '\\')
            tail = "\\" + path;
        else
            tail = path;
        ini_lines->LoadFromFile(exe + tail);
    }
    catch (...)
    {
    }
}

void Settings::CloseIni()
{
    ini_lines->Clear();
}

// A number from the loaded ini ( the name in any case ), or the default; spaces anywhere in the line are skipped.
int Settings::ReadIniInt(String key, int default_value)
{
    int result = default_value;
    if (ini_lines->Count >= 1)
    {
        try
        {
            for (int i = 0; i < ini_lines->Count; i++)
            {
                String line = ini_lines->Strings[i];
                char before_eq = 1;
                String ini_name = "";
                String value = "";
                if (line.Length() >= 1)
                {
                    for (int j = 1; j <= line.Length(); j++)
                    {
                        if (line[j] == '=')
                            before_eq = 0;
                        if (line[j] != ' ' && line[j] != '=')
                        {
                            if (before_eq)
                                ini_name = ini_name + line[j];
                            else
                                value = value + line[j];
                        }
                    }
                    if (LowerCase(ini_name) == LowerCase(key))
                    {
                        if (value.Length() >= 1)
                            result = StrToInt(value);
                        break;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return result;
}

// A text from the loaded ini ( the name in any case ), or the default; the spaces before the value are skipped.
String Settings::ReadIniString(String key, String default_value)
{
    String result = default_value;
    if (ini_lines->Count >= 1)
    {
        try
        {
            for (int i = 0; i < ini_lines->Count; i++)
            {
                char before_eq;
                String line = ini_lines->Strings[i];
                before_eq = 1;
                char after_eq = 1;
                String ini_name = "";
                String value = "";
                if (line.Length() >= 1)
                {
                    for (int j = 1; j <= line.Length(); j++)
                    {
                        if (line[j] == '=')
                            before_eq = 0;
                        if (before_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                                ini_name = ini_name + line[j];
                        }
                        else if (after_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                            {
                                after_eq = 0;
                                value = value + line[j];
                            }
                        }
                        else
                            value = value + line[j];
                    }
                    if (LowerCase(ini_name) == LowerCase(key))
                    {
                        if (value.Length() >= 1)
                            result = value;
                        break;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return result;
}

// A switch from the loaded ini: "on" or "1" is true, any other value false.
char Settings::ReadIniBool(String key, char default_value)
{
    char result = default_value;
    if (ini_lines->Count >= 1)
    {
        try
        {
            for (int i = 0; i < ini_lines->Count; i++)
            {
                char before_eq;
                String line = ini_lines->Strings[i];
                before_eq = 1;
                char after_eq = 1;
                String ini_name = "";
                String value = "";
                if (line.Length() >= 1)
                {
                    for (int j = 1; j <= line.Length(); j++)
                    {
                        if (line[j] == '=')
                            before_eq = 0;
                        if (before_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                                ini_name = ini_name + line[j];
                        }
                        else if (after_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                            {
                                after_eq = 0;
                                value = value + line[j];
                            }
                        }
                        else
                            value = value + line[j];
                    }
                    if (LowerCase(ini_name) == LowerCase(key))
                    {
                        if (value.Length() >= 1)
                        {
                            if (LowerCase(value) == "on" || LowerCase(value) == "1")
                                result = 1;
                            else
                                result = 0;
                        }
                        break;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return result;
}

void Settings::SetWorldCommunication(char value)
{
    world_communication = value;
}
