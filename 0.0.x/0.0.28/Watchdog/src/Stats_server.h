#ifndef Stats_serverH
#define Stats_serverH

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

struct SerialKey;
struct Settings;
struct SettingsHTM;


struct ServerStats;

// The server page ( server.html ): the status, the counts and the uptime.
struct ServerStats
{
    TStringList *template_lines;
    TStringList *page_lines;
    char has_template;
    String server_name;
    String version;
    bool server_running;
    char connected;
    String uptime;
    String connections;
    String max_connections;
    String players;
    String max_players;
    String most_players;
    String accounts;
    String characters;
    String guilds;
    String received;
    String sent;
    Settings *settings;
    SettingsHTM *html_settings;
    SerialKey *serial_key;

    ServerStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings);
    ~ServerStats();
    void CheckConnectionLoad();
    void GenerateHTML();
    void FillTemplateLine(int line_index);
};


#endif
