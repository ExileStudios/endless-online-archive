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
    int position;
    port = 8078;
    build_version = -1;
    auto_account_name = "";
    force_buffer = false;
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
    show_balloons = true;
    extended_tooltip = true;
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
    String exe_dir = Application->ExeName;
    if (exe_dir.Length() >= 1)
    {
        position = exe_dir.Length();
        for (; position > 0; position--)
        {
            if (exe_dir[position] == '\\' || exe_dir[position] == '/')
            {
                exe_dir = exe_dir.SubString(1, position);
                break;
            }
        }
    }
    String lp_file_name = exe_dir + "screen";
    screen_folder_exists = false;
    if (!access(lp_file_name.c_str(), 0))
        screen_folder_exists = true;
}

Config::~Config()
{
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
    bool loop_entered = false;
    int file_size;
    TStringList *lines;
    char *file_bytes;
    int handle;
    String path = "./config/setup.ini";
    if (access(path.c_str(), 0))
        return false;
    String file_text;
    lines = new TStringList();
    try
    {
        handle = FileOpen(path.c_str(), fmOpenRead);
        while (true)
        {
            file_size = FileSeek(handle, 0, 2);
            FileSeek(handle, 0, 0);
            file_bytes = new char[file_size + 1];
            FileRead(handle, file_bytes, file_size);
            FileClose(handle);
            file_text = file_bytes;
            file_text.SetLength(file_size);
            delete[] file_bytes;
            break;
        }
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
        while (true)
        {
            loop_entered = true;
            line = "";
            for (int i = line_start; i <= file_text.Length(); i++)
            {
                loop_entered = true;
                if (file_text[i] == '\n' || file_text.Length() == i)
                {
                    if (file_text.Length() == i)
                    {
                        if ((unsigned char)file_text[i] != '\n')
                        {
                            if ((unsigned char)file_text[i] != '\r')
                                line = line + file_text[i];
                        }
                    }
                    lines->Add(line);
                    line_start = i + 1;
                    break;
                }
                if (file_text[i] != ' ' && (unsigned char)file_text[i] != '\n' && (unsigned char)file_text[i] != '\r')
                    line = line + file_text[i];
            }
            if (file_text.Length() < line_start)
                break;
        }
        for (int line_index = 0; line_index < lines->Count; line_index++)
        {
            loop_entered = true;
            key_name = "";
            value_text = "";
            int position = 1;
            int equals_count = 0;
            if (!lines->Strings[line_index].Length())
                continue;
            if (lines->Strings[line_index][1] == '#')
                continue;
            if (lines->Strings[line_index][1] == '[')
                continue;
            while (lines->Strings[line_index].Length() >= position)
            {
                loop_entered = true;
                if (lines->Strings[line_index][position] == '=')
                    equals_count++;
                else
                {
                    if (!equals_count)
                        key_name += (char)lines->Strings[line_index][position];
                    if (equals_count == 1)
                        value_text += (char)lines->Strings[line_index][position];
                }
                position++;
            }
            key_name = UpperCase(key_name);
            value_upper = UpperCase(value_text);
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
    catch (...)
    {
    }
    settings_read = standard_settings_read + extra_settings_read;
    while (true)
    {
        if (language < 0 || language > 3)
            language = 0;
        if (keyboard_layout < 0 || keyboard_layout > 3)
            keyboard_layout = 0;
        break;
    }
    if (scale < 1)
        scale = 1;
    if (scale > 4)
        scale = 4;
    if (text_theme < 0)
        text_theme = 0;
    if (text_theme > 1)
        text_theme = 1;
    return true;
}

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

String Config::GetAppPath()
{
    return app_path;
}

bool Config::GetScreenFolderExists()
{
    return screen_folder_exists;
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

char Config::GetExtendedTooltip()
{
    return extended_tooltip;
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
