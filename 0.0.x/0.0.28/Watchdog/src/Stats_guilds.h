#ifndef Stats_guildsH
#define Stats_guildsH

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


struct GuildStats;

// The guild page ( guilds.html ), filled from the server's top guilds.
struct GuildStats
{
    TStringList *template_lines;
    TStringList *page_lines;
    char has_template;
    char list_found;
    int list_begin;
    int list_end;
    int guild_count;
    int list_index;
    TStringList *tags;
    TStringList *names;
    TStringList *exps;
    TStringList *exp_highs;
    TStringList *members;
    Settings *settings;
    SettingsHTM *html_settings;
    SerialKey *serial_key;

    GuildStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings);
    ~GuildStats();
    void GenerateHTML();
    void ClearList();
    void AddGuild(String tag, String guild_name, int experience, int exp_high, int member_count);
    String FillListLine(String template_line);
    void BuildPage();
    bool IsListMarker(String line, int which);
};


#endif
