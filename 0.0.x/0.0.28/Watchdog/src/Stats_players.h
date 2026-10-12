#ifndef Stats_playersH
#define Stats_playersH

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

enum AdminLevel
{
    AdminLevel_Player = 0,
    AdminLevel_Guide = 2,
    AdminLevel_Guardian = 3,
    AdminLevel_GameMaster = 4,
    AdminLevel_HighGameMaster = 5
};

struct SerialKey;
struct Settings;
struct SettingsHTM;


struct PlayerStats;

// The online list pages: playerlist.html by name and playerlist_exp.html by experience.
struct PlayerStats
{
    TStringList *template_lines;
    TStringList *page_lines;
    char has_template;
    char list_found;
    int list_begin;
    int list_end;
    int player_count;
    int list_index;
    TStringList *names;
    TStringList *titles;
    TStringList *exps;
    TStringList *levels;
    TStringList *genders;
    TStringList *admin_levels;
    TStringList *ranked_names;
    TStringList *ranked_titles;
    TStringList *ranked_exps;
    TStringList *ranked_levels;
    TStringList *ranked_genders;
    TStringList *ranked_admin_levels;
    Settings *settings;
    SettingsHTM *html_settings;
    SerialKey *serial_key;

    PlayerStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings);
    ~PlayerStats();
    void GenerateHTML();
    void GenerateExpHTML();
    void ClearList();
    void AddPlayer(String name, String title, int experience, int level, int gender, int admin_level);
    bool IsListMarker(String line, int which);
    String FillListLine(char by_name, String template_line);
    void BuildPage(char by_name);
    String StripTags(String text);
    bool SortsAfter(String name, String other);
};


#endif
