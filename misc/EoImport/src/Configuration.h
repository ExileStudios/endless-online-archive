#ifndef ConfigurationH
#define ConfigurationH

// The settings from EoPub.ini, the pub editor's configuration file, read when the importer starts.
struct Config
{
    short settings_read;
    short standard_settings_read;
    short extra_settings_read;
    char settings_loaded;
    String gfx_path;
    String pub_path;
    String export_path;
    String backup_path;
    char backup;
    char backup_folders;
    char show_recycled;
    char create_recycled;
    char delete_warning;
    char paste_warning;
    char view_all;
    char numbered_lists;
    char row_select;
    char name_hints;
    char show_zeros;
    char show_basics;
    char smart_filter;
    char smart_editors;
    char mouse_quick_edit;
    char mouse_quick_save;
    char auto_capitalization;
    char unused_bytes[1];
    char locked_index_column;
    char oob_warnings;
    char hotkeys;
    char fullscreen;
    char exit_save;
    bool export_empty;
    String theme;
    String loaded_theme;
    String app_path;
    String exe_name;
    String key_name;
    String value_upper;
    String value_text;
    int setting_number;
    bool setting_switch;

    Config();
    ~Config();
    bool ReadString(String key);
    bool ReadSwitch(String key);
    bool ReadInteger(String key);
    bool ReadColor(String key);
    bool LoadSettings();
    void SaveSettings();
    String SwitchText(int on);
    String TrimSpaces(String text);
    String GetExportPath();
};

#endif
