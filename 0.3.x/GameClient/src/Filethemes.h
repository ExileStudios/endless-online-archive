#ifndef FilethemesH
#define FilethemesH

#include <vector.h>

struct FileTheme
{
    String unused_text;
    String second_unused_text;
    char unused_bytes[4];
    String third_unused_text;
    String fourth_unused_text;
    String fifth_unused_text;
    String sixth_unused_text;
    String seventh_unused_text;
    String eighth_unused_text;
    String ninth_unused_text;
    char second_unused_bytes[4];

    ~FileTheme();
};

struct FileThemes
{
    bool is_loaded;
    bool is_outdated;
    bool unused_flag;
    String error_message;
    String theme_folder;
    int color_edits;
    int color_chat_edit;
    int color_popup_edits;
    int color_menu;
    int color_menu_version;
    int color_top_bar;
    int color_bottom_bar;
    int color_panel;
    int color_panel_list;
    int color_dialog;
    int color_dialog_soft;
    int color_dialog_caption;
    int color_dialog_link_over;
    int color_dialog_link;
    int color_popup_caption;
    int color_popup;
    int color_locker_item;
    int color_locker_item_highlight;
    int color_chat_default;
    int color_chat_private;
    int color_chat_system;
    int color_chat_warning;
    int color_chat_global;
    int color_chat_admin;
    int color_chat_tab;
    int color_chat_tab_active;
    int color_chat_tab_highlight;
    String theme_name;
    String theme_description;
    String theme_author;
    String co_author_1;
    String co_author_2;
    String co_author_3;
    String theme_version;
    short target_major_version;
    short target_minor_version;
    short client_major_version;
    short client_minor_version;
    String first_graphics_path;
    String second_graphics_path;
    bool second_unused_flag;
    vector<FileTheme> themes;
    String ini_file_name;
    String first_graphics_name;
    String second_graphics_name;
    String key;
    String upper_value;
    String value;
    int int_value;
    int color_value;

    FileThemes(String folder, int major_version, int minor_version);
    ~FileThemes();
    String StripExtension(String file_name, String extension);
    bool IsKey(String other_key);
    bool ReadIntValue(String name);
    bool ReadColorValue(String name);
    char HexDigitValue(char digit);
    bool LoadTheme(String file_name);
    bool ReadThemeFile(String file_name);
    String TrimSpaces(String text);
    String CleanThemeText(String text);
};

#endif
