#include <vcl.h>
#pragma hdrstop

#include "Configuration.h"
#include <Classes.hpp>
#include <Forms.hpp>
#include <io.h>

#pragma package(smart_init)

// The defaults, then EoPub.ini read over them; also splits the program's path into its folder and name.
Config::Config()
{
    gfx_path = ".\\gfx\\";
    pub_path = ".\\pub\\";
    export_path = ".\\pub\\";
    backup_path = ".\\backup\\";
    backup = true;
    backup_folders = false;
    create_recycled = true;
    view_all = false;
    delete_warning = true;
    paste_warning = true;
    numbered_lists = false;
    show_recycled = false;
    row_select = false;
    name_hints = false;
    show_zeros = false;
    show_basics = false;
    smart_filter = true;
    smart_editors = true;
    auto_capitalization = true;
    mouse_quick_edit = false;
    mouse_quick_save = true;
    locked_index_column = 0;
    oob_warnings = true;
    hotkeys = true;
    fullscreen = false;
    exit_save = true;
    export_empty = false;
    theme = "";
    loaded_theme = "";
    settings_read = 0;
    standard_settings_read = 0;
    extra_settings_read = 0;
    settings_loaded = LoadSettings();
    app_path = Application->ExeName;
    exe_name = "";
    int exe_name_length = Application->ExeName.Length();
    try
    {
        if (app_path.Length() >= 1)
        {
            for (int i = app_path.Length(); i > 0; i--)
            {
                if (app_path[i] == '.')
                    exe_name_length = i - 1;
                if (!(app_path[i] == '\\' || app_path[i] == '/'))
                    continue;
                int name_length = exe_name_length - i;
                exe_name = app_path.SubString(i + 1, name_length);
                app_path = app_path.SubString(1, i);
                break;
            }
        }
    }
    catch (...)
    {
        app_path = ".\\";
        exe_name = "EoPubs";
    }
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

// Reads the value of the key as a number of digits only; false for another key or value.
bool Config::ReadInteger(String key)
{
    int i;
    if ((char)(key_name != key))
        return false;
    i = 1;
    while (i <= value_upper.Length())
    {
        if (value_upper[i] < '0' || value_upper[i] > '9')
            return false;
        i = i + 1;
    }
    setting_number = Sysutils::StrToInt(value_upper);
    return true;
}

// Reads the value of the key as a color number; DEFAULT reads as 0.
bool Config::ReadColor(String key)
{
    if ((char)(value_upper == "DEFAULT"))
        value_upper = "0";
    return ReadInteger(key);
}

// Reads EoPub.ini: each key = value line, outside comments and section names, sets its setting.
bool Config::LoadSettings()
{
    int file_size;
    TStringList *lines = 0;
    char *file_bytes;
    int handle;
    String path = "EoPub.ini";
    if (access(path.c_str(), 0))
        return false;
    String file_text;
    lines = new TStringList();
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
        String config_line = "";
        int line_start = 1;
        do
        {
            config_line = "";
            for (int i = line_start; i <= file_text.Length(); i++)
            {
                if (file_text[i] == '\n' || i == file_text.Length())
                {
                    if (i == file_text.Length() && (unsigned char)file_text[i] != '\n' &&
                        (unsigned char)file_text[i] != '\r')
                        config_line = config_line + file_text[i];
                    lines->Add(config_line);
                    line_start = i + 1;
                    break;
                }
                if ((unsigned char)file_text[i] != '\n' && (unsigned char)file_text[i] != '\r')
                    config_line = config_line + file_text[i];
            }
        } while (file_text.Length() >= line_start);
        for (int line_index = 0; line_index < lines->Count; line_index++)
        {
            key_name = "";
            value_text = "";
            int position = 1;
            int equals_count = 0;
            if (!lines->Strings[line_index].Length())
                continue;
            // Comment lines (#) and section names ([..]) are skipped.
            if (lines->Strings[line_index][1] == '#')
                continue;
            if (lines->Strings[line_index][1] == '[')
                continue;
            while (lines->Strings[line_index].Length() >= position)
            {
                if (lines->Strings[line_index][position] == '=')
                    equals_count++;
                else
                {
                    if (!equals_count)
                    {
                        if (lines->Strings[line_index][position] != ' ')
                            key_name += (char)lines->Strings[line_index][position];
                    }
                    if (equals_count == 1)
                        value_text += (char)lines->Strings[line_index][position];
                }
                position++;
            }
            value_text = TrimSpaces(value_text);
            key_name = UpperCase(key_name);
            value_upper = UpperCase(value_text);
            if (ReadString("GFXPATH"))
            {
                gfx_path = value_text;
                standard_settings_read++;
            }
            else if (ReadString("PUBPATH"))
            {
                pub_path = value_text;
                standard_settings_read++;
            }
            else if (ReadString("EXPORTPATH"))
            {
                export_path = value_text;
                standard_settings_read++;
            }
            else if (ReadString("BACKUPPATH"))
            {
                backup_path = value_text;
                standard_settings_read++;
            }
            else if (ReadSwitch("BACKUP"))
            {
                backup = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("BACKUPFOLDERS"))
            {
                backup_folders = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("HOTKEYS"))
            {
                hotkeys = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("FULLSCREEN"))
            {
                fullscreen = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("NUMBEREDLISTS"))
            {
                numbered_lists = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("SHOWRECYCLED"))
            {
                show_recycled = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("CREATERECYCLED"))
            {
                create_recycled = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("LOCKEDINDEXCOL"))
            {
                locked_index_column = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("OOBWARNINGS"))
            {
                oob_warnings = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("NAMEHINTS"))
            {
                name_hints = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("ROWSELECT"))
            {
                row_select = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("AUTOCAPITALIZATION"))
            {
                auto_capitalization = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("MOUSEQUICKSAVE"))
            {
                mouse_quick_save = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("MOUSEQUICKEDIT"))
            {
                mouse_quick_edit = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("DELETEWARNING"))
            {
                delete_warning = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("PASTE WARNING"))
            {
                paste_warning = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("EXITSAVE"))
            {
                exit_save = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("SHOWZEROS"))
            {
                show_zeros = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("SHOWBASICS"))
            {
                show_basics = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("SMARTFILTER"))
            {
                smart_filter = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("SMARTEDITORS"))
            {
                smart_editors = setting_switch;
                standard_settings_read++;
            }
            else if (ReadSwitch("VIEWALL"))
            {
                view_all = setting_switch;
                standard_settings_read++;
            }
            else if (ReadString("THEME"))
            {
                theme = value_text;
                standard_settings_read++;
            }
        }
    }
    catch (...)
    {
    }
    settings_read = standard_settings_read + extra_settings_read;
    loaded_theme = theme;
    return true;
}

// Writes the settings to the program's .ini file, under the comments that explain them.
void Config::SaveSettings()
{
    Classes::TStringList *lines;
    lines = new Classes::TStringList();
    lines->Add("# Endless Pub Editor 0.02 [ configuration file ]");
    lines->Add("");
    lines->Add("# this is the Endless Pub Editor [ EoPub.exe ] configuration file");
    lines->Add("# if you do not know what you are doing, please consider leaving.");
    lines->Add("# visit www.endless-online.com for more instructions.");
    lines->Add("");
    lines->Add("[FILE PATHS]");
    lines->Add("");
    lines->Add("# folders on harddrive pointing the editor where to read or write.");
    lines->Add("# default paths:  ./gfx  and  ./pub  ( relative paths from EoPub.exe)");
    lines->Add("");
    lines->Add("GfxPath=" + gfx_path);
    lines->Add("PubPath=" + pub_path);
    lines->Add("ExportPath=" + export_path);
    lines->Add("");
    lines->Add("[BACKUP]");
    lines->Add("");
    lines->Add("# folder on harddrive to auto-save backup pub files.");
    lines->Add("# manual making backups before export is also recommended.");
    lines->Add("");
    lines->Add("Backup=" + SwitchText(backup));
    lines->Add("BackupFolders=" + SwitchText(backup_folders));
    lines->Add("BackupPath=" + backup_path);
    lines->Add("");
    lines->Add("[MAIN GRID]");
    lines->Add("");
    lines->Add("# all general settings for the big main edit grid.");
    lines->Add("");
    lines->Add("CreateRecycled=" + SwitchText(create_recycled));
    lines->Add("DeleteWarning=" + SwitchText(delete_warning));
    lines->Add("NameHints=" + SwitchText(name_hints));
    lines->Add("NumberedLists=" + SwitchText(numbered_lists));
    lines->Add("PasteWarning=" + SwitchText(paste_warning));
    lines->Add("RowSelect=" + SwitchText(row_select));
    lines->Add("ShowRecycled=" + SwitchText(show_recycled));
    lines->Add("ViewAll=" + SwitchText(view_all));
    lines->Add("");
    lines->Add("[EDITORS]");
    lines->Add("");
    lines->Add("# all settings for the main and sub grid incell-editors.");
    lines->Add("");
    lines->Add("AutoCapitalization=" + SwitchText(auto_capitalization));
    lines->Add("MouseQuickedit=" + SwitchText(mouse_quick_edit));
    lines->Add("MouseQuicksave=" + SwitchText(mouse_quick_save));
    lines->Add("ShowBasics=" + SwitchText(show_basics));
    lines->Add("ShowZeros=" + SwitchText(show_zeros));
    lines->Add("SmartFilter=" + SwitchText(smart_filter));
    lines->Add("SmartEditors=" + SwitchText(smart_editors));
    lines->Add("");
    lines->Add("[APPLICATION]");
    lines->Add("");
    lines->Add("ExitSave=" + SwitchText(exit_save));
    lines->Add("Fullscreen=" + SwitchText(fullscreen));
    lines->Add("Hotkeys=" + SwitchText(hotkeys));
    lines->Add("");
    lines->Add("[THEME]");
    lines->Add("");
    lines->Add("# supported themes;");
    lines->Add("# 1. default");
    lines->Add("# 2. dark");
    lines->Add("");
    lines->Add("theme=" + theme);
    lines->Add("");
    lines->Add("[FINETUNE/BETA]");
    lines->Add("");
    lines->Add("OobWarnings=" + SwitchText(oob_warnings));
    lines->Add("LockedIndexCol=" + SwitchText(locked_index_column));
    lines->Add("");
    lines->Add("# end of configuration");
    String path = "./" + exe_name + ".ini";
    lines->SaveToFile(path);
}

// A switch as the .ini file writes it, on or off.
String Config::SwitchText(int on)
{
    if (!on)
        return "off";
    return "on";
}

// The text without the spaces at its start and end.
String Config::TrimSpaces(String text)
{
    int start = 1;
    int trailing = 0;
    while (start < text.Length())
    {
        if (text[start] != ' ')
            break;
        start++;
    }
    for (int i = text.Length(); i > 0; i--)
    {
        if (text[i] != ' ')
            break;
        trailing++;
    }
    return text.SubString(start, text.Length() + 1 - start - trailing);
}

String Config::GetExportPath()
{
    return export_path;
}
