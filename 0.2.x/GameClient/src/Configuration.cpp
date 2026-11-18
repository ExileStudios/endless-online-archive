#include <vcl.h>
#pragma hdrstop

#include "Configuration.h"
#include <io.h>
#include <Classes.hpp>
#include <Forms.hpp>

#pragma package(smart_init)

// The defaults, then setup.ini read over them; also notes whether a "screen" folder is next to the program.
Config::Config()
{
    port = 8078;
    build_version = -1;
    auto_account_name = "";
    force_buffer = false;
    ddraw_hardware = false;
    fullscreen = false;
    skip_intro = true;
    skip_softpad = true;
    use_page_flip = true;
    use_bit_blt_draw = false;
    music_volume = 50;
    sound_volume = 100;
    stay_on_top = false;
    sizeable = true;
    scale = 1;
    window_position.x = -1;
    window_position.y = -1;
    preload_character_gfx = true;
    preload_map_gfx = true;
    preload_tile_gfx = true;
    language = 0;
    keyboard_layout = 0;
    wasd_keys = false;
    filter_enabled = true;
    strict_filter = false;
    log_chat = false;
    accept_private_messages = true;
    accept_requests = true;
    idle_disconnect = false;
    show_balloons = true;
    extended_tooltip = true;
    elemental_icons = true;
    minimap_mode = 0;
    radar_position = 0;
    minimap_icons = true;
    minimap_grid = false;
    radar_coords = false;
    show_shadows = true;
    chat_log_path = "chatlog.txt";
    debug = false;
    group_color = 4;
    npc_color = 0;
    text_theme = 0;
    settings_read = 0;
    standard_settings_read = 0;
    extra_settings_read = 0;
    settings_loaded = LoadSettings();
    music_available = music_volume;
    sound_available = sound_volume;
    app_path = Application->ExeName;
    exe_name = "";
    int exe_name_length = Application->ExeName.Length();
    try
    {
        if (app_path.Length() >= 1)
        {
            for (int i = app_path.Length(); i > 0; i--)
            {
                if (app_path[i] == '\\' || app_path[i] == '/')
                {
                    int name_length = exe_name_length - i;
                    exe_name = app_path.SubString(i + 1, name_length);
                    app_path = app_path.SubString(1, i);
                    break;
                }
            }
        }
    }
    catch (...)
    {
        app_path = ".\\";
    }
    String lp_file_name = app_path + "screen";
    screen_folder_exists = false;
    if (!access(lp_file_name.c_str(), 0))
        screen_folder_exists = true;
}

Config::~Config()
{
}

String Config::GetAppPath()
{
    return app_path;
}

bool Config::ReadString(String key)
{
    if (key_name != key)
        return false;
    return true;
}

// Reads the value of the key as a number of digits only; false for another key or value.
bool Config::ReadInteger(String key)
{
    if (key_name != key)
        return false;
    for (int i = 1; i <= value_upper.Length(); i++)
    {
        if (value_upper[i] != '0' && value_upper[i] != '1' && value_upper[i] != '2' && value_upper[i] != '3' &&
            value_upper[i] != '4' && value_upper[i] != '5' && value_upper[i] != '6' && value_upper[i] != '7' &&
            value_upper[i] != '8' && value_upper[i] != '9')
            return false;
    }
    setting_number = StrToInt(value_upper);
    return true;
}

// Reads the value of the key as a switch, ON or 1 for on and OFF or 0 for off; false for another key or value.
bool Config::ReadSwitch(String key)
{
    if (key_name != key)
        return false;
    for (int i = 1; i <= value_upper.Length(); i++)
    {
        if (value_upper[i] != 'O' && value_upper[i] != 'N' && value_upper[i] != 'F' && value_upper[i] != '0' &&
            value_upper[i] != '1')
            return false;
    }
    if (value_upper == "ON")
    {
        setting_switch = true;
        return true;
    }
    if (value_upper == "1")
    {
        setting_switch = true;
        return true;
    }
    if (value_upper == "OFF")
    {
        setting_switch = false;
        return true;
    }
    if (value_upper == "0")
    {
        setting_switch = false;
        return true;
    }
    return false;
}

// Reads the value of the key as a window position: CENTER, DESKTOP, or x and y split by a comma.
bool Config::ReadPosition(String key)
{
    if (key_name != key)
        return false;
    if (value_upper == "CENTER")
    {
        setting_x = -1;
        return true;
    }
    if (value_upper == "DESKTOP")
    {
        setting_x = -2;
        return true;
    }
    int comma = value_upper.Pos(",");
    if (comma <= 1)
        return false;
    if (comma >= value_upper.Length())
        return false;
    String remaining = value_upper;
    value_upper = remaining.SubString(1, comma - 1);
    if (!ReadInteger(key))
        return false;
    setting_x = setting_number;
    remaining.Delete(1, comma);
    value_upper = remaining;
    if (!ReadInteger(key))
        return false;
    setting_y = setting_number;
    return true;
}

// Reads the colour value of the key; DEFAULT reads as 0.
bool Config::ReadColor(String key)
{
    if (value_upper == "DEFAULT")
        value_upper = "0";
    return ReadInteger(key);
}

// Reads the value of the key as a volume: a switch gives 0 or 100, a number is kept to 100 at most.
bool Config::ReadVolume(String key)
{
    if (ReadSwitch(key))
    {
        setting_number = 0;
        if (setting_switch)
            setting_number = 100;
        return true;
    }
    if (ReadInteger(key))
    {
        setting_switch = true;
        if (setting_number > 100)
            setting_number = 100;
        return true;
    }
    return false;
}

// Reads config/setup.ini line by line, skipping comments and section heads, and sets each known key from its value;
// false when the file is missing.
bool Config::LoadSettings()
{
    int file_size;
    char *file_bytes;
    int handle;
    TStringList *file_lines;
    String path = "./config/setup.ini";
    if (access(path.c_str(), 0))
        return false;
    String file_text;
    file_lines = new TStringList();
    try
    {
        handle = FileOpen(path.c_str(), fmOpenRead);
        file_size = FileSeek(handle, 0, 2);
        FileSeek(handle, 0, 0);
        file_bytes = new char[file_size + 1];
        FileRead(handle, file_bytes, file_size);
        FileClose(handle);
        file_text = file_bytes;
        file_text.SetLength(file_size);
        delete[] file_bytes;
    }
    catch (...)
    {
        FileClose(handle);
        return false;
    }
    try
    {
        String line = "";
        int line_start = 1;
        do
        {
            line = "";
            for (int i = line_start; i <= file_text.Length(); i++)
            {
                if (file_text[i] == '\n' || file_text.Length() == i)
                {
                    if (file_text.Length() == i)
                    {
                        if ((unsigned char)file_text[i] != '\n' && (unsigned char)file_text[i] != '\r')
                            line = line + file_text[i];
                    }
                    file_lines->Add(line);
                    line_start = i + 1;
                    break;
                }
                if ((unsigned char)file_text[i] != '\n' && (unsigned char)file_text[i] != '\r')
                    line = line + file_text[i];
            }
        } while (file_text.Length() >= line_start);
        for (int line_index = 0; line_index < file_lines->Count; line_index++)
        {
            key_name = "";
            value_text = "";
            int position = 1;
            int equals_count = 0;
            if (!file_lines->Strings[line_index].Length())
                continue;
            if (file_lines->Strings[line_index][1] == '#')
                continue;
            if (file_lines->Strings[line_index][1] == '[')
                continue;
            while (file_lines->Strings[line_index].Length() >= position)
            {
                if (file_lines->Strings[line_index][position] == '=')
                    equals_count++;
                else
                {
                    if (!equals_count)
                    {
                        if (file_lines->Strings[line_index][position] != ' ')
                            key_name += (char)file_lines->Strings[line_index][position];
                    }
                    if (equals_count == 1)
                        value_text += (char)file_lines->Strings[line_index][position];
                }
                position++;
            }
            value_text = TrimSpaces(value_text);
            key_name = UpperCase(key_name);
            value_upper = UpperCase(value_text);
            if (value_upper.Length() > 0)
            {
                if (ReadString("HOST"))
                {
                    host = value_text;
                    standard_settings_read++;
                }
                else if (ReadInteger("PORT"))
                {
                    port = setting_number;
                    standard_settings_read++;
                }
                else if (ReadInteger("BUILDVERSION"))
                {
                    build_version = setting_number;
                    extra_settings_read++;
                }
                else if (ReadString("AUTOACCOUNTNAME"))
                {
                    auto_account_name = value_text;
                    extra_settings_read++;
                }
                else if (ReadSwitch("DDRAWHARDWARE"))
                {
                    ddraw_hardware = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("FULLSCREEN"))
                {
                    fullscreen = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("SKIPINTRO"))
                {
                    skip_intro = setting_switch;
                    extra_settings_read++;
                }
                else if (ReadSwitch("SKIPSOFTPAD"))
                {
                    skip_softpad = setting_switch;
                    extra_settings_read++;
                }
                else if (ReadSwitch("PAGEFLIPPING"))
                {
                    use_page_flip = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("BLOCKTRANSFER"))
                {
                    use_bit_blt_draw = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("FORCEBUFFER"))
                {
                    force_buffer = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("STAYONTOP"))
                {
                    stay_on_top = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("SIZEABLE"))
                {
                    sizeable = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadInteger("SCALE"))
                {
                    scale = setting_number;
                    standard_settings_read++;
                }
                else if (ReadPosition("POSITION"))
                {
                    window_position = *(TPoint *)&setting_x;
                    standard_settings_read++;
                }
                else if (ReadSwitch("STATICSPRITES"))
                {
                    preload_character_gfx = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("STATICOBJECTS"))
                {
                    preload_map_gfx = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("STATICTILES"))
                {
                    preload_tile_gfx = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadInteger("LANGUAGE"))
                {
                    language = setting_number;
                    standard_settings_read++;
                }
                else if (ReadInteger("KEYBOARD"))
                {
                    keyboard_layout = setting_number;
                    standard_settings_read++;
                }
                else if (ReadSwitch("WASDKEYS"))
                {
                    wasd_keys = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("FILTER"))
                {
                    filter_enabled = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("FILTERALL"))
                {
                    strict_filter = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadVolume("SOUND"))
                {
                    sound_volume = setting_number;
                    standard_settings_read++;
                }
                else if (ReadVolume("MUSIC"))
                {
                    music_volume = setting_number;
                    standard_settings_read++;
                }
                else if (ReadSwitch("SHOWBALOONS"))
                {
                    show_balloons = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("EXTENDEDTOOLTIP"))
                {
                    extended_tooltip = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("ELEMENTALICONS"))
                {
                    elemental_icons = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadInteger("MINIMAPMODE"))
                {
                    minimap_mode = setting_number;
                    standard_settings_read++;
                }
                else if (ReadInteger("RADARPOSITION"))
                {
                    radar_position = setting_number;
                    standard_settings_read++;
                }
                else if (ReadSwitch("RADARCOORDS"))
                {
                    radar_coords = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("MINIMAPICONS"))
                {
                    minimap_icons = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("MINIMAPGRID"))
                {
                    minimap_grid = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("SHOWSHADOWS"))
                {
                    show_shadows = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("HEARWHISPER"))
                {
                    accept_private_messages = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("INTERACTION"))
                {
                    accept_requests = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("IDLEDISCONNECT"))
                {
                    idle_disconnect = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadSwitch("LOGCHAT"))
                {
                    log_chat = setting_switch;
                    standard_settings_read++;
                }
                else if (ReadString("LOGFILE"))
                {
                    chat_log_path = value_text;
                    standard_settings_read++;
                }
                else if (ReadColor("NPCCOLOR"))
                {
                    npc_color = setting_number;
                    standard_settings_read++;
                }
                else if (ReadColor("GROUPCOLOR"))
                {
                    group_color = setting_number;
                    standard_settings_read++;
                }
                else if (ReadColor("TEXTTHEME"))
                {
                    text_theme = setting_number;
                    extra_settings_read++;
                }
                else if (ReadSwitch("DEBUG"))
                {
                    debug = setting_switch;
                    extra_settings_read++;
                }
            }
        }
    }
    catch (...)
    {
    }
    settings_read = standard_settings_read + extra_settings_read;
    if (language < 0 || language > 3)
        language = 0;
    if (keyboard_layout < 0 || keyboard_layout > 3)
        keyboard_layout = 0;
    if (scale < 1)
        scale = 1;
    if (scale > 4)
        scale = 4;
    if (minimap_mode < MinimapMode_Toggled)
        minimap_mode = 0;
    if (minimap_mode > MinimapMode_Radar)
        minimap_mode = 2;
    if (radar_position < 0)
        radar_position = 0;
    if (radar_position > 3)
        radar_position = 3;
    if (text_theme < 0)
        text_theme = 0;
    if (text_theme > 1)
        text_theme = 1;
    return true;
}

// The text without the spaces at its start and end.
DEFINE_TRIM_SPACES(Config)

void Config::SetFullscreen(char enabled)
{
    fullscreen = enabled;
}

void Config::SetUseBitBltDraw(char use_bit_blt)
{
    use_bit_blt_draw = use_bit_blt;
}

void Config::SetMusicEnabled(int enabled)
{
    music_volume = enabled;
}

void Config::SetSoundVolume(int enabled)
{
    sound_volume = enabled;
}

void Config::SetScale(int count)
{
    scale = count;
}

void Config::SetExtendedTooltip(bool value)
{
    extended_tooltip = value;
}

void Config::SetElementalIcons(bool enabled)
{
    elemental_icons = enabled;
}

void Config::SetMinimapMode(int mode)
{
    minimap_mode = mode;
}

void Config::SetMinimapIcons(bool enabled)
{
    minimap_icons = enabled;
}

void Config::SetMinimapGrid(bool enabled)
{
    minimap_grid = enabled;
}

void Config::SetLanguage(int new_language)
{
    language = new_language;
}

char Config::SetShowBalloons(char show)
{
    return show_balloons = show;
}

char Config::SetShowShadows(char show)
{
    return show_shadows = show;
}

void Config::SetFilterEnabled(char enabled)
{
    filter_enabled = enabled;
}

void Config::SetStrictFilter(char strict)
{
    strict_filter = strict;
}

char Config::SetLogChat(char enabled)
{
    return log_chat = enabled;
}

char Config::SetAcceptPrivateMessages(char accept)
{
    return accept_private_messages = accept;
}

char Config::SetAcceptRequests(char accept)
{
    return accept_requests = accept;
}

void Config::SetKeyboardLayout(int layout)
{
    keyboard_layout = layout;
}

void Config::SetWasdKeys(bool value)
{
    wasd_keys = value;
}

bool Config::GetScreenFolderExists()
{
    return screen_folder_exists;
}

int Config::GetDDrawHardware()
{
    return ddraw_hardware;
}

char Config::GetFullscreen()
{
    return fullscreen;
}

char Config::GetSkipIntro()
{
    return skip_intro;
}

char Config::GetSkipSoftpad()
{
    return skip_softpad;
}

char Config::GetUsePageFlip()
{
    return use_page_flip;
}

char Config::GetUseBitBltDraw()
{
    return use_bit_blt_draw;
}

int Config::GetMusicVolume()
{
    return music_volume;
}

int Config::GetSoundVolume()
{
    return sound_volume;
}

char Config::GetStayOnTop()
{
    return stay_on_top;
}

char Config::GetSizeable()
{
    return sizeable;
}

int Config::GetScale()
{
    return scale;
}

TPoint Config::GetWindowPosition()
{
    return window_position;
}

bool Config::GetExtendedTooltip()
{
    return extended_tooltip;
}

bool Config::GetElementalIcons()
{
    return elemental_icons;
}

int Config::GetMinimapMode()
{
    return minimap_mode;
}

int Config::GetRadarPosition()
{
    return radar_position;
}

bool Config::GetMinimapIcons()
{
    return minimap_icons;
}

bool Config::GetMinimapGrid()
{
    return minimap_grid;
}

char Config::GetRadarCoords()
{
    return radar_coords;
}

char Config::GetPreloadCharacterGfx()
{
    return preload_character_gfx;
}

char Config::GetPreloadMapGfx()
{
    return preload_map_gfx;
}

char Config::GetPreloadTileGfx()
{
    return preload_tile_gfx;
}

int Config::GetLanguage()
{
    return language;
}

int Config::GetKeyboardLayout()
{
    return keyboard_layout;
}

bool Config::GetWasdKeys()
{
    return wasd_keys;
}

String Config::GetHost()
{
    return host;
}

int Config::GetBuildVersion()
{
    return build_version;
}

String Config::GetAutoAccountName()
{
    return auto_account_name;
}

char Config::GetForceBuffer()
{
    return force_buffer;
}

char Config::GetFilterEnabled()
{
    return filter_enabled;
}

char Config::GetStrictFilter()
{
    return strict_filter;
}

char Config::GetLogChat()
{
    return log_chat;
}

char Config::GetAcceptPrivateMessages()
{
    return accept_private_messages;
}

char Config::GetAcceptRequests()
{
    return accept_requests;
}

char Config::GetIdleDisconnect()
{
    return idle_disconnect;
}

char Config::GetShowBalloons()
{
    return show_balloons;
}

char Config::GetShowShadows()
{
    return show_shadows;
}

String Config::GetChatLogPath()
{
    return chat_log_path;
}

// The PORT setting, or 8078 when it is not a number.
int Config::GetPort()
{
    return port;
}

int Config::GetNpcColor()
{
    return npc_color;
}

int Config::GetGroupColor()
{
    return group_color;
}

int Config::GetTextTheme()
{
    return text_theme;
}

int Config::GetMusicAvailable()
{
    return music_available;
}

int Config::GetSoundAvailable()
{
    return sound_available;
}

char Config::GetDebug()
{
    return debug;
}

void Config::SetMusicAvailable(int available)
{
    music_available = available;
}

void Config::SetSoundAvailable(int available)
{
    sound_available = available;
}
