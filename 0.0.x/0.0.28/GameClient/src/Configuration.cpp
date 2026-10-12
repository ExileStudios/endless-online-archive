#include <vcl.h>
#pragma hdrstop

#include "Configuration.h"
#include <io.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

// The defaults, then setup.ini read over them; also notes whether a "screen" folder is next to the exe.
Config::Config()
{
    int position;
    fullscreen = true;
    skip_intro = false;
    skip_softpad = false;
    use_page_flip = true;
    use_bit_blt_draw = 0;
    music_enabled = 1;
    sound_enabled = 1;
    stay_on_top = false;
    sizeable = true;
    preload_character_gfx = false;
    preload_map_gfx = false;
    preload_tile_gfx = false;
    language = 0;
    keyboard_layout = 0;
    filter_enabled = 1;
    strict_filter = 0;
    log_chat = 0;
    accept_private_messages = 1;
    accept_requests = 1;
    show_balloons = 1;
    show_shadows = 1;
    chat_log_path = "chatlog.txt";
    LoadSettings();
    music_available = music_enabled;
    sound_available = sound_enabled;
    String exe_dir = Application->ExeName;
    if (exe_dir.Length() >= 1)
    {
        position = exe_dir.Length();
        while (position > 0)
        {
            if (exe_dir[position] == '\\' || exe_dir[position] == '/')
            {
                exe_dir = exe_dir.SubString(1, position);
                break;
            }
            position--;
        }
    }
    String lpFileName = exe_dir + "screen";
    screen_folder_exists = false;
    if (!access(lpFileName.c_str(), 0))
        screen_folder_exists = true;
}

Config::~Config()
{
}

// Reads config/setup.ini: one KEY=VALUE a line, in upper case and with the spaces taken out.
// Only O, N and F of a value are kept, so it reads as ON or OFF.
void Config::LoadSettings()
{
    int hFile;
    DWORD nNumberOfBytesToRead = 0;
    String file_text;
    char *FileHandle;
    TStringList *lines = new Classes::TStringList();
    try
    {
        hFile = FileOpen("./config/setup.ini", 0);
        nNumberOfBytesToRead = FileSeek(hFile, 0, 2);
        FileSeek(hFile, 0, 0);
        FileHandle = new char[nNumberOfBytesToRead + 1];
        FileRead(hFile, FileHandle, nNumberOfBytesToRead);
        FileClose(hFile);
        file_text = FileHandle;
        file_text.SetLength(nNumberOfBytesToRead);
        delete[] FileHandle;
    }
    catch (...)
    {
        FileClose(hFile);
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
                        if ((unsigned char)file_text[i] != 10)
                        {
                            if ((unsigned char)file_text[i] != 13)
                                line = line + file_text[i];
                        }
                    }
                    lines->Add(UpperCase(line));
                    line_start = i + 1;
                    break;
                }
                if (file_text[i] != ' ')
                {
                    if ((unsigned char)file_text[i] != 10)
                    {
                        if ((unsigned char)file_text[i] != 13)
                            line = line + file_text[i];
                    }
                }
            }
        } while (file_text.Length() >= line_start);
        for (int j = 0; j < lines->Count; j++)
        {
            String config_line = lines->Strings[j];
            String key_name = "";
            String value_text = "";
            for (int k = 1; k <= config_line.Length(); k++)
            {
                if (config_line[k] == '=')
                {
                    if (key_name == "HOST")
                    {
                        for (int m = k + 1; m <= config_line.Length(); m++)
                            value_text = value_text + config_line[m];
                        host = value_text;
                    }
                    if (key_name == "PORT")
                    {
                        for (int n = k + 1; n <= config_line.Length(); n++)
                            value_text = value_text + config_line[n];
                        port_text = value_text;
                    }
                    if (key_name == "FULLSCREEN")
                    {
                        for (int ii = k + 1; ii <= config_line.Length(); ii++)
                        {
                            if (config_line[ii] == 'O' || config_line[ii] == 'N' || config_line[ii] == 'F')
                                value_text = value_text + config_line[ii];
                        }
                        if (value_text == "OFF")
                            fullscreen = false;
                    }
                    if (key_name == "SKIPINTRO")
                    {
                        for (int jj = k + 1; jj <= config_line.Length(); jj++)
                        {
                            if (config_line[jj] == 'O' || config_line[jj] == 'N' || config_line[jj] == 'F')
                                value_text = value_text + config_line[jj];
                        }
                        if (value_text == "ON")
                            skip_intro = true;
                    }
                    if (key_name == "SKIPSOFTPAD")
                    {
                        for (int kk = k + 1; kk <= config_line.Length(); kk++)
                        {
                            if (config_line[kk] == 'O' || config_line[kk] == 'N' || config_line[kk] == 'F')
                                value_text = value_text + config_line[kk];
                        }
                        if (value_text == "ON")
                            skip_softpad = true;
                    }
                    if (key_name == "PAGEFLIPPING")
                    {
                        for (int mm = k + 1; mm <= config_line.Length(); mm++)
                        {
                            if (config_line[mm] == 'O' || config_line[mm] == 'N' || config_line[mm] == 'F')
                                value_text = value_text + config_line[mm];
                        }
                        if (value_text == "OFF")
                            use_page_flip = false;
                    }
                    if (key_name == "BLOCKTRANSFER")
                    {
                        for (int nn = k + 1; nn <= config_line.Length(); nn++)
                        {
                            if (config_line[nn] == 'O' || config_line[nn] == 'N' || config_line[nn] == 'F')
                                value_text = value_text + config_line[nn];
                        }
                        if (value_text == "ON")
                            use_bit_blt_draw = 1;
                    }
                    if (key_name == "STAYONTOP")
                    {
                        for (int i1 = k + 1; i1 <= config_line.Length(); i1++)
                        {
                            if (config_line[i1] == 'O' || config_line[i1] == 'N' || config_line[i1] == 'F')
                                value_text = value_text + config_line[i1];
                        }
                        if (value_text == "ON")
                            stay_on_top = true;
                    }
                    if (key_name == "SIZEABLE")
                    {
                        for (int i2 = k + 1; i2 <= config_line.Length(); i2++)
                        {
                            if (config_line[i2] == 'O' || config_line[i2] == 'N' || config_line[i2] == 'F')
                                value_text = value_text + config_line[i2];
                        }
                        if (value_text == "OFF")
                            sizeable = false;
                    }
                    if (key_name == "STATICSPRITES")
                    {
                        for (int i3 = k + 1; i3 <= config_line.Length(); i3++)
                        {
                            if (config_line[i3] == 'O' || config_line[i3] == 'N' || config_line[i3] == 'F')
                                value_text = value_text + config_line[i3];
                        }
                        if (value_text == "ON")
                            preload_character_gfx = true;
                    }
                    if (key_name == "STATICOBJECTS")
                    {
                        for (int i4 = k + 1; i4 <= config_line.Length(); i4++)
                        {
                            if (config_line[i4] == 'O' || config_line[i4] == 'N' || config_line[i4] == 'F')
                                value_text = value_text + config_line[i4];
                        }
                        if (value_text == "ON")
                            preload_map_gfx = true;
                    }
                    if (key_name == "STATICTILES")
                    {
                        for (int i5 = k + 1; i5 <= config_line.Length(); i5++)
                        {
                            if (config_line[i5] == 'O' || config_line[i5] == 'N' || config_line[i5] == 'F')
                                value_text = value_text + config_line[i5];
                        }
                        if (value_text == "ON")
                            preload_tile_gfx = true;
                    }
                    // Language and keyboard are 0 to 3; anything else is 0.
                    if (key_name == "LANGUAGE")
                    {
                        for (int i6 = k + 1; i6 <= config_line.Length(); i6++)
                            value_text = value_text + config_line[i6];
                        try
                        {
                            language = StrToInt(value_text);
                        }
                        catch (...)
                        {
                            language = 0;
                        }
                        if (language < 0 || language > 3)
                            language = 0;
                    }
                    if (key_name == "KEYBOARD")
                    {
                        for (int i7 = k + 1; i7 <= config_line.Length(); i7++)
                            value_text = value_text + config_line[i7];
                        try
                        {
                            keyboard_layout = StrToInt(value_text);
                        }
                        catch (...)
                        {
                            keyboard_layout = 0;
                        }
                        if (keyboard_layout < 0 || keyboard_layout > 3)
                            keyboard_layout = 0;
                    }
                    if (key_name == "FILTER")
                    {
                        for (int i8 = k + 1; i8 <= config_line.Length(); i8++)
                        {
                            if (config_line[i8] == 'O' || config_line[i8] == 'N' || config_line[i8] == 'F')
                                value_text = value_text + config_line[i8];
                        }
                        if (value_text == "OFF")
                            filter_enabled = 0;
                    }
                    if (key_name == "FILTERALL")
                    {
                        for (int i9 = k + 1; i9 <= config_line.Length(); i9++)
                        {
                            if (config_line[i9] == 'O' || config_line[i9] == 'N' || config_line[i9] == 'F')
                                value_text = value_text + config_line[i9];
                        }
                        if (value_text == "ON")
                            strict_filter = 1;
                    }
                    if (key_name == "SOUND")
                    {
                        for (int i10 = k + 1; i10 <= config_line.Length(); i10++)
                        {
                            if (config_line[i10] == 'O' || config_line[i10] == 'N' || config_line[i10] == 'F')
                                value_text = value_text + config_line[i10];
                        }
                        if (value_text == "OFF")
                            sound_enabled = 0;
                    }
                    if (key_name == "MUSIC")
                    {
                        for (int i11 = k + 1; i11 <= config_line.Length(); i11++)
                        {
                            if (config_line[i11] == 'O' || config_line[i11] == 'N' || config_line[i11] == 'F')
                                value_text = value_text + config_line[i11];
                        }
                        if (value_text == "OFF")
                            music_enabled = 0;
                    }
                    if (key_name == "SHOWBALOONS")
                    {
                        for (int i12 = k + 1; i12 <= config_line.Length(); i12++)
                        {
                            if (config_line[i12] == 'O' || config_line[i12] == 'N' || config_line[i12] == 'F')
                                value_text = value_text + config_line[i12];
                        }
                        if (value_text == "OFF")
                            show_balloons = 0;
                    }
                    if (key_name == "SHOWSHADOWS")
                    {
                        for (int i13 = k + 1; i13 <= config_line.Length(); i13++)
                        {
                            if (config_line[i13] == 'O' || config_line[i13] == 'N' || config_line[i13] == 'F')
                                value_text = value_text + config_line[i13];
                        }
                        if (value_text == "OFF")
                            show_shadows = 0;
                    }
                    if (key_name == "HEARWHISPER")
                    {
                        for (int i14 = k + 1; i14 <= config_line.Length(); i14++)
                        {
                            if (config_line[i14] == 'O' || config_line[i14] == 'N' || config_line[i14] == 'F')
                                value_text = value_text + config_line[i14];
                        }
                        if (value_text == "OFF")
                            accept_private_messages = 0;
                    }
                    if (key_name == "INTERACTION")
                    {
                        for (int i15 = k + 1; i15 <= config_line.Length(); i15++)
                        {
                            if (config_line[i15] == 'O' || config_line[i15] == 'N' || config_line[i15] == 'F')
                                value_text = value_text + config_line[i15];
                        }
                        if (value_text == "OFF")
                            accept_requests = 0;
                    }
                    if (key_name == "LOGCHAT")
                    {
                        for (int i16 = k + 1; i16 <= config_line.Length(); i16++)
                        {
                            if (config_line[i16] == 'O' || config_line[i16] == 'N' || config_line[i16] == 'F')
                                value_text = value_text + config_line[i16];
                        }
                        if (value_text == "ON")
                            log_chat = 1;
                    }
                    if (key_name == "LOGFILE")
                    {
                        for (int i17 = k + 1; i17 <= config_line.Length(); i17++)
                            value_text = value_text + config_line[i17];
                        chat_log_path = value_text;
                    }
                }
                else
                    key_name = key_name + config_line[k];
            }
        }
    }
    catch (...)
    {
    }
}

void Config::SetUseBitBltDraw(char use_bit_blt)
{
    use_bit_blt_draw = use_bit_blt;
}

void Config::SetMusicEnabled(char enabled)
{
    music_enabled = enabled;
}

void Config::SetSoundEnabled(char enabled)
{
    sound_enabled = enabled;
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

String Config::GetAppPath()
{
    return app_path;
}

bool Config::GetScreenFolderExists()
{
    return screen_folder_exists;
}

bool Config::GetFullscreen()
{
    return fullscreen;
}

bool Config::GetSkipIntro()
{
    return skip_intro;
}

bool Config::GetSkipSoftpad()
{
    return skip_softpad;
}

bool Config::GetUsePageFlip()
{
    return use_page_flip;
}

char Config::GetUseBitBltDraw()
{
    return use_bit_blt_draw;
}

char Config::GetMusicEnabled()
{
    return music_enabled;
}

char Config::GetSoundEnabled()
{
    return sound_enabled;
}

bool Config::GetStayOnTop()
{
    return stay_on_top;
}

bool Config::GetSizeable()
{
    return sizeable;
}

bool Config::GetPreloadCharacterGfx()
{
    return preload_character_gfx;
}

bool Config::GetPreloadMapGfx()
{
    return preload_map_gfx;
}

bool Config::GetPreloadTileGfx()
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

String Config::GetHost()
{
    return host;
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

// The PORT setting, or 8078 when it is not a number.
int Config::GetPort()
{
    int port;
    try
    {
        port = StrToInt(port_text);
    }
    catch (...)
    {
        port = 8078;
    }
    return port;
}

char Config::GetMusicAvailable()
{
    return music_available;
}

char Config::GetSoundAvailable()
{
    return sound_available;
}

void Config::SetMusicAvailable(char available)
{
    music_available = available;
}

void Config::SetSoundAvailable(char available)
{
    sound_available = available;
}
