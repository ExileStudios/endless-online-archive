#include <vcl.h>
#pragma hdrstop

#include "Configuration.h"
#include <Classes.hpp>
#include <Forms.hpp>
#include <io.h>

#pragma package(smart_init)

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
    export_empty = true;
    expert_mode = false;
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

String Config::GetAppPath()
{
    return AddBackslash(app_path);
}

String Config::GetExeName()
{
    return exe_name;
}

// Whether the line read has this key; the caller then takes its value as text.
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

void Configuration_UnusedLiterals1()
{
    "DEFAULT";
    "0";
}

// Reads the program's .ini file: each key = value line, outside comments and section names, sets its setting.
bool Config::LoadSettings()
{
    int file_size;
    TStringList *lines = 0;
    char *file_bytes;
    int handle;
    String path = "./" + exe_name + ".ini";
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
        // Each line splits at its first = into a key, spaces dropped, and a value.
        for (int line_index = 0; line_index < lines->Count; line_index++)
        {
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
            else if (ReadSwitch("EXPORTEMPTY"))
            {
                export_empty = setting_switch;
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

// The path with a backslash at its end, unless it already ends in a slash.
String Config::AddBackslash(String path)
{
    String result = path;
    if (result.Length() > 0)
    {
        if (result[result.Length()] != '/' && result[result.Length()] != '\\')
            result += "\\";
    }
    return result;
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

void Config::SetGfxPath(String path)
{
    gfx_path = path;
}

void Config::SetPubPath(String path)
{
    pub_path = path;
}

void Config::SetExportPath(String path)
{
    export_path = path;
}

void Config::SetBackupPath(String path)
{
    backup_path = path;
}

void Config::SetBackup(bool on)
{
    backup = on;
}

void Config::SetBackupFolders(bool on)
{
    backup_folders = on;
}

void Config::SetFullscreen(bool on)
{
    fullscreen = on;
}

void Config::SetNumberedLists(bool on)
{
    numbered_lists = on;
}

void Config::SetShowRecycled(bool on)
{
    show_recycled = on;
}

void Config::SetCreateRecycled(bool on)
{
    create_recycled = on;
}

void Config::SetDeleteWarning(bool on)
{
    delete_warning = on;
}

void Config::SetPasteWarning(bool on)
{
    paste_warning = on;
}

void Config::SetRowSelect(bool on)
{
    row_select = on;
}

void Config::SetHotkeys(bool on)
{
    hotkeys = on;
}

void Config::SetExportEmpty(bool on)
{
    export_empty = on;
}

void Config::SetExitSave(bool on)
{
    exit_save = on;
}

void Config::SetExpertMode(bool on)
{
    expert_mode = on;
}

void Config::SetShowZeros(bool on)
{
    show_zeros = on;
}

void Config::SetShowBasics(bool on)
{
    show_basics = on;
}

void Config::SetSmartFilter(bool on)
{
    smart_filter = on;
}

void Config::SetSmartEditors(bool on)
{
    smart_editors = on;
}

void Config::SetViewAll(bool on)
{
    view_all = on;
}

void Config::SetLoadedTheme(String theme)
{
    loaded_theme = theme;
}

void Config::SetMouseQuickEdit(bool on)
{
    mouse_quick_edit = on;
}

void Config::SetMouseQuickSave(bool on)
{
    mouse_quick_save = on;
}

void Config::SetAutoCapitalization(bool on)
{
    auto_capitalization = on;
}

void Config::SetNameHints(bool on)
{
    name_hints = on;
}

String Config::GetGfxPath()
{
    return AddBackslash(gfx_path);
}

String Config::GetPubPath()
{
    return AddBackslash(pub_path);
}

String Config::GetExportPath()
{
    return AddBackslash(export_path);
}

String Config::GetBackupPath()
{
    return AddBackslash(backup_path);
}

bool Config::GetBackup()
{
    return backup;
}

bool Config::GetBackupFolders()
{
    return backup_folders;
}

bool Config::GetFullscreen()
{
    return fullscreen;
}

bool Config::GetNumberedLists()
{
    return numbered_lists;
}

bool Config::GetShowRecycled()
{
    return show_recycled;
}

bool Config::GetCreateRecycled()
{
    return create_recycled;
}

bool Config::GetDeleteWarning()
{
    return delete_warning;
}

bool Config::GetPasteWarning()
{
    return paste_warning;
}

bool Config::GetRowSelect()
{
    return row_select;
}

bool Config::GetHotkeys()
{
    return hotkeys;
}

bool Config::GetExportEmpty()
{
    return export_empty;
}

bool Config::GetExitSave()
{
    return exit_save;
}

bool Config::GetExpertMode()
{
    return expert_mode;
}

bool Config::GetShowZeros()
{
    return show_zeros;
}

bool Config::GetShowBasics()
{
    return show_basics;
}

bool Config::GetSmartFilter()
{
    return smart_filter;
}

bool Config::GetSmartEditors()
{
    return smart_editors;
}

bool Config::GetViewAll()
{
    return view_all;
}

String Config::GetTheme()
{
    return theme;
}

String Config::GetLoadedTheme()
{
    return loaded_theme;
}

bool Config::GetMouseQuickEdit()
{
    return mouse_quick_edit;
}

bool Config::GetMouseQuickSave()
{
    return mouse_quick_save;
}

bool Config::GetAutoCapitalization()
{
    return auto_capitalization;
}

unsigned char Config::GetLockedIndexColumn()
{
    return locked_index_column;
}

unsigned char Config::GetOobWarnings()
{
    return oob_warnings;
}

bool Config::GetNameHints()
{
    return name_hints;
}

// Writes the settings to the program's .ini file, under the comments that explain them.
void Config::SaveSettings()
{
    TStringList *lines = new TStringList();
    lines->Add("# Endless Pub Studio 0.02 [ configuration file ]");
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
    lines->Add("GfxPath=" + GetGfxPath());
    lines->Add("PubPath=" + GetPubPath());
    lines->Add("ExportPath=" + GetExportPath());
    lines->Add("");
    lines->Add("[BACKUP]");
    lines->Add("");
    lines->Add("# folder on harddrive to auto-save backup pub files.");
    lines->Add("# manual making backups before export is also recommended.");
    lines->Add("");
    lines->Add("Backup=" + SwitchText(GetBackup()));
    lines->Add("BackupFolders=" + SwitchText(GetBackupFolders()));
    lines->Add("BackupPath=" + GetBackupPath());
    lines->Add("");
    lines->Add("[MAIN GRID]");
    lines->Add("");
    lines->Add("# all general settings for the big main edit grid.");
    lines->Add("");
    lines->Add("CreateRecycled=" + SwitchText(GetCreateRecycled()));
    lines->Add("DeleteWarning=" + SwitchText(GetDeleteWarning()));
    lines->Add("NameHints=" + SwitchText(GetNameHints()));
    lines->Add("NumberedLists=" + SwitchText(GetNumberedLists()));
    lines->Add("PasteWarning=" + SwitchText(GetPasteWarning()));
    lines->Add("RowSelect=" + SwitchText(GetRowSelect()));
    lines->Add("ShowRecycled=" + SwitchText(GetShowRecycled()));
    lines->Add("ViewAll=" + SwitchText(GetViewAll()));
    lines->Add("");
    lines->Add("[EDITORS]");
    lines->Add("");
    lines->Add("# all settings for the main and sub grid incell-editors.");
    lines->Add("");
    lines->Add("AutoCapitalization=" + SwitchText(GetAutoCapitalization()));
    lines->Add("MouseQuickedit=" + SwitchText(GetMouseQuickEdit()));
    lines->Add("MouseQuicksave=" + SwitchText(GetMouseQuickSave()));
    lines->Add("ShowBasics=" + SwitchText(GetShowBasics()));
    lines->Add("ShowZeros=" + SwitchText(GetShowZeros()));
    lines->Add("SmartFilter=" + SwitchText(GetSmartFilter()));
    lines->Add("SmartEditors=" + SwitchText(GetSmartEditors()));
    lines->Add("");
    lines->Add("[APPLICATION]");
    lines->Add("");
    lines->Add("ExitSave=" + SwitchText(GetExitSave()));
    lines->Add("ExportEmpty=" + SwitchText(GetExportEmpty()));
    lines->Add("Fullscreen=" + SwitchText(GetFullscreen()));
    lines->Add("Hotkeys=" + SwitchText(GetHotkeys()));
    lines->Add("");
    lines->Add("[THEME]");
    lines->Add("");
    lines->Add("# supported themes;");
    lines->Add("# 1. default");
    lines->Add("# 2. dark");
    lines->Add("");
    lines->Add("theme=" + loaded_theme);
    lines->Add("");
    lines->Add("[FINETUNE/BETA]");
    lines->Add("");
    lines->Add("OobWarnings=" + SwitchText(GetOobWarnings()));
    lines->Add("LockedIndexCol=" + SwitchText(GetLockedIndexColumn()));
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
