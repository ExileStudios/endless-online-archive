#include <vcl.h>
#pragma hdrstop

#include "Settingshtml.h"
#include "Constants.h"

#pragma package(smart_init)

// The default colours and status texts, then config/htmlgen.ini over them.
SettingsHTM::SettingsHTM()
{
    ini_lines = new TStringList();
    online_status = "Online!";
    offline_status = "Offline";
    font_color_online = "#228822";
    font_color_offline = "#cc0000";
    bg_color_admin = "#dddddd";
    font_color_player = "#111111";
    font_color_guide = "#ccaa33";
    font_color_guardian = "#cc00cc";
    font_color_gm = "#1111cc";
    font_color_hgm = "#cc0000";
    LoadConfig();
}

SettingsHTM::~SettingsHTM()
{
}

// The status text shown while the server is online.
String SettingsHTM::CopyString()
{
    return online_status;
}

String SettingsHTM::GetOfflineStatus()
{
    return offline_status;
}

String SettingsHTM::GetFontColorOnline()
{
    return font_color_online;
}

String SettingsHTM::GetFontColorOffline()
{
    return font_color_offline;
}

String SettingsHTM::GetBgColorAdmin()
{
    return bg_color_admin;
}

String SettingsHTM::GetFontColorPlayer()
{
    return font_color_player;
}

String SettingsHTM::GetFontColorGuide()
{
    return font_color_guide;
}

String SettingsHTM::GetFontColorGuardian()
{
    return font_color_guardian;
}

String SettingsHTM::GetFontColorGM()
{
    return font_color_gm;
}

String SettingsHTM::GetFontColorHGM()
{
    return font_color_hgm;
}

// Reads config/htmlgen.ini; a key that is not there keeps its value.
void SettingsHTM::LoadConfig()
{
    SetIniPath("./config/htmlgen.ini");
    online_status = ReadIniString("online_status", online_status);
    offline_status = ReadIniString("offline_status", offline_status);
    font_color_online = ReadIniString("fontcolor_online", font_color_online);
    font_color_offline = ReadIniString("fontcolor_offline", font_color_offline);
    bg_color_admin = ReadIniString("bgcolor_admin", bg_color_admin);
    font_color_player = ReadIniString("fontcolor_player", font_color_player);
    font_color_guide = ReadIniString("fontcolor_guide", font_color_guide);
    font_color_guardian = ReadIniString("fontcolor_guardian", font_color_guardian);
    font_color_gm = ReadIniString("fontcolor_gm", font_color_gm);
    font_color_hgm = ReadIniString("fontcolor_hgm", font_color_hgm);
    CloseIni();
}

DEFINE_SET_INI_PATH(SettingsHTM)

void SettingsHTM::CloseIni()
{
    ini_lines->Clear();
}

DEFINE_READ_INI_INT(SettingsHTM)

DEFINE_READ_INI_STRING(SettingsHTM)


DEFINE_READ_INI_BOOL(SettingsHTM)
