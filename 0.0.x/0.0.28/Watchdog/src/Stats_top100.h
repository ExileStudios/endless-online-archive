#ifndef Stats_top100H
#define Stats_top100H

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


struct Top100Stats;

// The top 100 page ( top100.html ), filled from the server's top players.
struct Top100Stats
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
    Settings *settings;
    SettingsHTM *html_settings;
    SerialKey *serial_key;

    Top100Stats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings);
    ~Top100Stats();
    void GenerateHTML();
    void ClearList();
    void AddPlayer(String name, String title, int experience, int level, int gender, int admin_level);
    void BuildPage();
    bool IsListMarker(String line, int which);
    String FillListLine(String template_line);
    String StripTags(String text);
};


#endif
