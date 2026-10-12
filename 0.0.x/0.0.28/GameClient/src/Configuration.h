#ifndef ConfigurationH
#define ConfigurationH

// The settings from config/setup.ini, read once when the client starts.
struct Config
{
    bool fullscreen;
    bool skip_intro;
    bool skip_softpad;
    bool use_page_flip;
    char use_bit_blt_draw;
    char music_enabled;
    char sound_enabled;
    bool screen_folder_exists;
    bool stay_on_top;
    bool sizeable;
    bool preload_character_gfx;
    bool preload_map_gfx;
    bool preload_tile_gfx;
    int language;
    int keyboard_layout;
    char filter_enabled;
    char strict_filter;
    char log_chat;
    String chat_log_path;
    char accept_private_messages;
    char accept_requests;
    char show_balloons;
    char show_shadows;
    char music_available;
    char sound_available;
    String host;
    String port_text;
    String app_path;

    Config();
    ~Config();
    void LoadSettings();
    void SetUseBitBltDraw(char use_bit_blt);
    void SetMusicEnabled(char enabled);
    void SetSoundEnabled(char enabled);
    void SetLanguage(int new_language);
    char SetShowBalloons(char show);
    char SetShowShadows(char show);
    void SetFilterEnabled(char enabled);
    void SetStrictFilter(char strict);
    char SetLogChat(char enabled);
    char SetAcceptPrivateMessages(char accept);
    char SetAcceptRequests(char accept);
    void SetKeyboardLayout(int layout);
    String GetAppPath();
    bool GetScreenFolderExists();
    bool GetFullscreen();
    bool GetSkipIntro();
    bool GetSkipSoftpad();
    bool GetUsePageFlip();
    char GetUseBitBltDraw();
    char GetMusicEnabled();
    char GetSoundEnabled();
    bool GetStayOnTop();
    bool GetSizeable();
    bool GetPreloadCharacterGfx();
    bool GetPreloadMapGfx();
    bool GetPreloadTileGfx();
    int GetLanguage();
    int GetKeyboardLayout();
    String GetHost();
    char GetFilterEnabled();
    char GetStrictFilter();
    char GetLogChat();
    char GetAcceptPrivateMessages();
    char GetAcceptRequests();
    char GetShowBalloons();
    char GetShowShadows();
    String GetChatLogPath();
    int GetPort();
    char GetMusicAvailable();
    char GetSoundAvailable();
    void SetMusicAvailable(char available);
    void SetSoundAvailable(char available);
};

#endif
