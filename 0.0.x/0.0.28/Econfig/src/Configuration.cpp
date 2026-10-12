#include <vcl.h>
#pragma hdrstop

#include "Configuration.h"

#pragma package(smart_init)

#include <io.h>

// The defaults, then config/setup.ini over them; notes whether a "screen" file is next to the exe.
Config::Config()
{
    int pos;
    ini_keys = new TStringList();
    ini_lines = new TStringList();
    fullscreen = true;
    skip_intro = false;
    page_flipping = true;
    block_transfer = false;
    music = true;
    sound = true;
    stay_on_top = false;
    sizeable = true;
    static_sprites = false;
    static_objects = false;
    static_tiles = false;
    language = 0;
    keyboard = 0;
    filter = true;
    filter_all = false;
    log_chat = false;
    hear_whisper = true;
    interaction = true;
    show_baloons = true;
    show_shadows = true;
    log_file = "chatlog.txt";
    LoadConfig();
    loaded_music = music;
    loaded_sound = sound;
    String exe_dir = Application->ExeName;
    if (exe_dir.Length() >= 1)
    {
        pos = exe_dir.Length();
        while (pos >= 1)
        {
            if (exe_dir[pos] == '\\' || exe_dir[pos] == '/')
            {
                exe_dir = exe_dir.SubString(1, pos);
                break;
            }
            pos--;
        }
    }
    String screen_file = exe_dir + "screen";
    screen_file_found = false;
    if (!access(screen_file.c_str(), 0))
        screen_file_found = true;
}

// Frees the ini's lines.
Config::~Config()
{
    ini_keys->Clear();
    ini_lines->Clear();
    delete ini_keys;
    delete ini_lines;
}

// Reads config/setup.ini: each line is kept as it is, and again in upper case without its spaces.
void Config::LoadConfig()
{
    String file_text;
    int file_handle;
    ini_keys->Clear();
    ini_lines->Clear();
    try
    {
        DWORD file_size;
        char *buffer;
        file_handle = FileOpen("./config/setup.ini", 0);
        file_size = FileSeek(file_handle, 0, 2);
        FileSeek(file_handle, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(file_handle, buffer, file_size);
        FileClose(file_handle);
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(file_handle);
    }
    try
    {
        String key_line = "";
        String raw_line = "";
        int line_start = 1;
        do
        {
            key_line = "";
            raw_line = "";
            for (int i = line_start; i <= file_text.Length(); i++)
            {
                if (file_text[i] == '\n' || file_text.Length() == i)
                {
                    if (file_text.Length() == i && (unsigned char)file_text[i] != 10 &&
                        (unsigned char)file_text[i] != 13)
                    {
                        raw_line = raw_line + file_text[i];
                        key_line = key_line + file_text[i];
                    }
                    ini_lines->Add(raw_line);
                    ini_keys->Add(UpperCase(key_line));
                    line_start = i + 1;
                    break;
                }
                if (file_text[i] != ' ' && (unsigned char)file_text[i] != 10 && (unsigned char)file_text[i] != 13)
                    key_line = key_line + file_text[i];
                if ((unsigned char)file_text[i] != 10 && (unsigned char)file_text[i] != 13)
                    raw_line = raw_line + file_text[i];
            }
        } while (file_text.Length() >= line_start);
        for (int j = 0; j < ini_keys->Count; j++)
        {
            String line = ini_keys->Strings[j];
            String key = "";
            String value = "";
            for (int k = 1; k <= line.Length(); k++)
            {
                if (line[k] == '=')
                {
                    if (key == "HOST")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        host = value;
                    }
                    if (key == "PORT")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        port = value;
                    }
                    if (key == "FULLSCREEN")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            fullscreen = false;
                    }
                    if (key == "SKIPINTRO")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            skip_intro = true;
                    }
                    if (key == "PAGEFLIPPING")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            page_flipping = false;
                    }
                    if (key == "BLOCKTRANSFER")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            block_transfer = true;
                    }
                    if (key == "STAYONTOP")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            stay_on_top = true;
                    }
                    if (key == "SIZEABLE")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            sizeable = false;
                    }
                    if (key == "STATICSPRITES")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            static_sprites = true;
                    }
                    if (key == "STATICOBJECTS")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            static_objects = true;
                    }
                    if (key == "STATICTILES")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            static_tiles = true;
                    }
                    if (key == "LANGUAGE")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        try
                        {
                            language = StrToInt(value);
                        }
                        catch (...)
                        {
                            language = 0;
                        }
                        // languages and keyboards 0 to 3; anything else is 0
                        if (language < 0 || language > 3)
                            language = 0;
                    }
                    if (key == "KEYBOARD")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        try
                        {
                            keyboard = StrToInt(value);
                        }
                        catch (...)
                        {
                            keyboard = 0;
                        }
                        if (keyboard < 0 || keyboard > 3)
                            keyboard = 0;
                    }
                    if (key == "FILTER")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            filter = false;
                    }
                    if (key == "FILTERALL")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            filter_all = true;
                    }
                    if (key == "SOUND")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            sound = false;
                    }
                    if (key == "MUSIC")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            music = false;
                    }
                    if (key == "SHOWBALOONS")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            show_baloons = false;
                    }
                    if (key == "SHOWSHADOWS")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            show_shadows = false;
                    }
                    if (key == "HEARWHISPER")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            hear_whisper = false;
                    }
                    if (key == "INTERACTION")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "OFF")
                            interaction = false;
                    }
                    if (key == "LOGCHAT")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                        {
                            if (line[value_pos] == 'O' || line[value_pos] == 'N' || line[value_pos] == 'F')
                                value = value + line[value_pos];
                        }
                        if (value == "ON")
                            log_chat = true;
                    }
                    if (key == "LOGFILE")
                    {
                        for (int value_pos = k + 1; value_pos <= line.Length(); value_pos++)
                            value = value + line[value_pos];
                        log_file = value;
                    }
                }
                else
                    key = key + line[k];
            }
        }
    }
    catch (...)
    {
    }
}

// Puts a new value on the key's line.
void Config::SetValue(String name, String value)
{
    int i;
    int j;
    for (i = 0; ini_keys->Count > i && ini_lines->Count > i; i++)
    {
        String line = ini_keys->Strings[i];
        String line_key = "";
        String line_value = "";
        for (j = 1; j <= line.Length(); j++)
        {
            if (line[j] == '=')
            {
                if (line_key == UpperCase(name))
                    ini_lines->Strings[i] = name + "=" + value;
            }
            else
                line_key = line_key + line[j];
        }
    }
}

// Writes the lines back to config/setup.ini.
void Config::SaveConfig()
{
    ini_lines->SaveToFile("./config/setup.ini");
}

bool Config::GetFullscreen()
{
    return fullscreen;
}

bool Config::GetPageFlipping()
{
    return page_flipping;
}

bool Config::GetBlockTransfer()
{
    return block_transfer;
}

bool Config::GetMusic()
{
    return music;
}

bool Config::GetSound()
{
    return sound;
}

bool Config::GetStayOnTop()
{
    return stay_on_top;
}

bool Config::GetSizeable()
{
    return sizeable;
}

bool Config::GetStaticSprites()
{
    return static_sprites;
}

bool Config::GetStaticObjects()
{
    return static_objects;
}

bool Config::GetStaticTiles()
{
    return static_tiles;
}

int Config::GetLanguage()
{
    return language;
}

int Config::GetKeyboard()
{
    return keyboard;
}

String Config::GetHost()
{
    return host;
}

bool Config::GetFilter()
{
    return filter;
}

bool Config::GetFilterAll()
{
    return filter_all;
}

bool Config::GetLogChat()
{
    return log_chat;
}

bool Config::GetHearWhisper()
{
    return hear_whisper;
}

bool Config::GetInteraction()
{
    return interaction;
}

bool Config::GetShowBaloons()
{
    return show_baloons;
}

bool Config::GetShowShadows()
{
    return show_shadows;
}

String Config::GetLogFile()
{
    return log_file;
}

// The port as a number; 8078 when it is not one.
int Config::GetPort()
{
    int port_number;
    try
    {
        port_number = StrToInt(port);
    }
    catch (...)
    {
        port_number = 8078;
    }
    return port_number;
}
