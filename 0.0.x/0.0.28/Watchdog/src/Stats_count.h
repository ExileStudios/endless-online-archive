#ifndef Stats_countH
#define Stats_countH

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


struct CountStats;

// The count page ( count.html ): the number of players online.
struct CountStats
{
    TStringList *template_lines;
    TStringList *page_lines;
    char has_template;
    String players;
    Settings *settings;
    SettingsHTM *html_settings;
    SerialKey *serial_key;

    CountStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings);
    ~CountStats();
    void GenerateHTML();
    void FillTemplateLine(int line_index);
};


#endif
