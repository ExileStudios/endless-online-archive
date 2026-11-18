#ifndef ConfigurationH
#define ConfigurationH

enum MinimapMode
{
    MinimapMode_Toggled = 0,
    MinimapMode_Overlay = 1,
    MinimapMode_Radar = 2
};

// The settings from config/setup.ini, read once when the client starts.
struct Config
{
    short settings_read;
    short standard_settings_read;
    short extra_settings_read;
    char settings_loaded;
    unsigned char ddraw_hardware;
    char fullscreen;
    unsigned char lighting;
    char skip_intro;
    char skip_softpad;
    char use_page_flip;
    char use_bit_blt_draw;
    int music_volume;
    int sound_volume;
    int sound_buffers;
    bool screen_folder_exists;
    char stay_on_top;
    char force_client_size;
    char sizeable;
    int scale;
    TPoint window_position;
    char auto_open_inventory;
    bool extended_tooltip;
    bool elemental_icons;
    char hotkey_notifications;
    int status_widget;
    bool auto_drop_damage_bar;
    bool auto_drop_recover_bar;
    bool auto_drop_exp_bar;
    int minimap_mode;
    int radar_position;
    bool minimap_icons;
    bool minimap_grid;
    char radar_coords;
    char preload_character_gfx;
    char preload_map_gfx;
    char preload_tile_gfx;
    int language;
    int keyboard_layout;
    bool wasd_keys;
    bool always_ghost;
    char filter_enabled;
    char strict_filter;
    String theme_file;
    int default_color;
    int npc_color;
    int group_color;
    char log_chat;
    String chat_log_path;
    char accept_private_messages;
    char accept_requests;
    bool show_balloons;
    bool show_balloon_name;
    char show_shadows;
    char idle_disconnect;
    int music_available;
    int sound_available;
    String host;
    int port;
    int build_version;
    String auto_account_name;
    char force_buffer;
    char debug;
    bool show_grid_cursor;
    String app_path;
    String exe_name;
    String key_name;
    String value_upper;
    String value_text;
    int setting_number;
    int setting_x;
    int setting_y;
    bool setting_switch;

    Config();
    ~Config();
    bool ReadString(String key);
    bool ReadInteger(String key);
    bool ReadSwitch(String key);
    bool ReadPosition(String key);
    bool ReadColor(String key);
    bool ReadVolume(String key);
    bool LoadSettings();
    String TrimSpaces(String text);
    void SetFullscreen(char enabled);
    void SetUseBitBltDraw(char use_bit_blt);
    void SetMusicEnabled(int enabled);
    void SetSoundVolume(int enabled);
    void SetSoundBuffers(int count);
    void SetScale(int count);
    void SetExtendedTooltip(bool value);
    void SetElementalIcons(bool enabled);
    void SetStatusWidget(int value);
    void SetAutoDropDamageBar(bool enabled);
    void SetAutoDropRecoverBar(bool enabled);
    void SetAutoDropExpBar(bool enabled);
    void SetMinimapMode(int mode);
    void SetMinimapIcons(bool enabled);
    void SetMinimapGrid(bool enabled);
    void SetLanguage(int new_language);
    char SetShowBalloons(bool enabled);
    char SetShowBalloonName(bool enabled);
    char SetShowShadows(char show);
    void SetFilterEnabled(char enabled);
    void SetStrictFilter(char strict);
    char SetLogChat(char enabled);
    char SetAcceptPrivateMessages(char accept);
    char SetAcceptRequests(char accept);
    void SetKeyboardLayout(int layout);
    void SetWasdKeys(bool value);
    void SetAlwaysGhost(bool enabled);
    String GetAppPath();
    bool GetScreenFolderExists();
    int GetDDrawHardware();
    char GetFullscreen();
    int GetLighting();
    char GetSkipIntro();
    char GetSkipSoftpad();
    char GetUsePageFlip();
    char GetUseBitBltDraw();
    int GetMusicVolume();
    int GetSoundVolume();
    int GetSoundBuffers();
    char GetStayOnTop();
    char GetForceClientSize();
    char GetSizeable();
    int GetScale();
    TPoint GetWindowPosition();
    char GetAutoOpenInventory();
    bool GetExtendedTooltip();
    bool GetElementalIcons();
    char GetHotkeyNotifications();
    int GetStatusWidget();
    bool GetAutoDropDamageBar();
    bool GetAutoDropRecoverBar();
    bool GetAutoDropExpBar();
    int GetMinimapMode();
    int GetRadarPosition();
    bool GetMinimapIcons();
    bool GetMinimapGrid();
    char GetRadarCoords();
    char GetPreloadCharacterGfx();
    char GetPreloadMapGfx();
    char GetPreloadTileGfx();
    int GetLanguage();
    int GetKeyboardLayout();
    bool GetWasdKeys();
    bool GetAlwaysGhost();
    String GetHost();
    int GetBuildVersion();
    String GetAutoAccountName();
    char GetForceBuffer();
    char GetFilterEnabled();
    char GetStrictFilter();
    String GetThemeFile();
    char GetLogChat();
    char GetAcceptPrivateMessages();
    char GetAcceptRequests();
    char GetIdleDisconnect();
    bool GetShowBalloons();
    bool GetShowBalloonName();
    char GetShowShadows();
    String GetChatLogPath();
    int GetPort();
    int GetDefaultColor();
    int GetNpcColor();
    int GetGroupColor();
    int GetMusicAvailable();
    int GetSoundAvailable();
    char GetDebug();
    bool GetShowGridCursor();
    void SetMusicAvailable(int available);
    void SetSoundAvailable(int available);
    char SetShowGridCursor(bool enabled);
};


// The text without the spaces at its start and end.
// Shared body: a class defines its TrimSpaces with DEFINE_TRIM_SPACES(TheClass).
#define DEFINE_TRIM_SPACES(Class) \
String Class::TrimSpaces(String text) \
{ \
    int start = 1; \
    int trailing = 0; \
    while (start < text.Length()) \
    { \
        if (text[start] != ' ') \
            break; \
        start++; \
    } \
    for (int i = text.Length(); i > 0; i--) \
    { \
        if (text[i] != ' ') \
            break; \
        trailing++; \
    } \
    return text.SubString(start, text.Length() + 1 - start - trailing); \
}

#endif
