#ifndef SettingshtmlH
#define SettingshtmlH

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



struct SettingsHTM;

// The colours and status texts of the pages, from config/htmlgen.ini.
struct SettingsHTM
{
    String online_status;
    String offline_status;
    String font_color_online;
    String font_color_offline;
    String bg_color_admin;
    String font_color_player;
    String font_color_guide;
    String font_color_guardian;
    String font_color_gm;
    String font_color_hgm;
    TStringList *ini_lines;

    SettingsHTM();
    ~SettingsHTM();
    String CopyString();
    String GetOfflineStatus();
    String GetFontColorOnline();
    String GetFontColorOffline();
    String GetBgColorAdmin();
    String GetFontColorPlayer();
    String GetFontColorGuide();
    String GetFontColorGuardian();
    String GetFontColorGM();
    String GetFontColorHGM();
    void LoadConfig();
    void SetIniPath(String path);
    void CloseIni();
    int ReadIniInt(String key, int default_value);
    String ReadIniString(String key, String default_value);
    bool ReadIniBool(String key, bool default_value);
};


#endif
