#ifndef AnimationprotectorH
#define AnimationprotectorH

enum GamePanel
{
    GamePanel_Inventory = 6,
    GamePanel_Spells = 8,
    GamePanel_Skills = 9,
    GamePanel_Status = 10,
    GamePanel_Chat = 11,
    GamePanel_Playerlist = 12,
    GamePanel_Group = 13,
    GamePanel_Settings = 15,
    GamePanel_Help = 16,
    GamePanel_News = 18
};

enum MenuScreen
{
    MenuScreen_Main = 1,
    MenuScreen_Credits = 2,
    MenuScreen_CreateAccount = 3,
    MenuScreen_Login = 4,
    MenuScreen_CharacterSelect = 5,
    MenuScreen_GridAccountName = 7,
    MenuScreen_PasswordGrid = 8
};

struct BankController;
struct BarberController;
struct BookController;
struct ChestController;
struct CreditsController;
struct DialogController;
struct GameController;
struct GuildController;
struct InnController;
struct InterfaceController;
struct IntroController;
struct JukeboxController;
struct LawController;
struct LearnController;
struct LockerController;
struct MemoController;
struct MenuController;
struct MsgBoardController;
struct MusicianController;
struct PaperdollController;
struct PriestController;
struct QuestController;
struct ReportController;
struct ShopController;
struct SocialController;
struct StatController;
struct TradeController;

// The screen mode and sub-state, the action and chat delays, and every window controller.
struct AnimationController
{
    String account_name;
    String account_password;
    String account_detail_a;
    String account_detail_b;
    String account_email;
    InterfaceController *interface_controller;
    IntroController *intro;
    MenuController *menu;
    CreditsController *credits;
    GameController *game;
    PaperdollController *paperdoll;
    BookController *book;
    TradeController *trade;
    ShopController *shop;
    LearnController *learn;
    GuildController *guild;
    MemoController *memo;
    DialogController *dialog;
    QuestController *quests;
    ReportController *report;
    LawController *law;
    InnController *inn;
    ChestController *chest;
    BankController *bank;
    JukeboxController *jukebox;
    MusicianController *musician;
    BarberController *barber;
    LockerController *locker;
    PriestController *priest;
    StatController *stat;
    SocialController *social;
    MsgBoardController *message_board;
    int screen_mode;
    int sub_state;
    int frame_counter;
    bool animating;
    int account_progress;
    int reply_timeout;
    int chat_delay;
    int chat_decay_ticks;
    int chat_message_count;
    int global_decay_ticks;
    int global_message_count;
    String chat_decay_check;
    String chat_count_check;
    bool refresh_delayed;
    bool emote_delayed;
    bool sit_delayed;
    bool path_delayed;
    int refresh_delay;
    int emote_delay;
    int sit_delay;
    int path_delay;
    int object_click_delay;
    int tile_use_delay;
    int walk_block;
    int command_delay;
    int command_delay_length;

    AnimationController(char show_intro);
    ~AnimationController();
    void SetScreenMode(int new_screen_mode);
    void SetSubState(int new_sub_state);
    void SetFrameCounter(int frame);
    int GetSubState();
    int GetScreenMode();
    bool IsAnimating();
    void StartObjectClickDelay();
    bool IsObjectClickDelayed();
    void SetCommandDelay(int seconds);
    void RestartCommandDelay();
    bool IsCommandDelayed();
    void StartTileUseDelay();
    bool IsTileUseDelayed();
    void BlockWalking();
    bool IsWalkingBlocked();
    void UpdateTimers();
    void StartChatDelay();
    void RecordChatMessage();
    void RecordGlobalMessage();
    bool CanSendChat();
    bool IsChatAllowed();
    bool IsGlobalMessageAllowed();
    void StartAccountProgress();
    void StopAccountProgress();
    int GetAccountProgress();
    void StartReplyTimeout();
    void StopReplyTimeout();
    void StartEmoteDelay(int ticks);
    bool IsEmoteDelayed();
    void StartRefreshDelay(int ticks);
    bool IsSitDelayed();
    void StartSitDelay(int ticks);
    bool IsPathDelayed();
    void StartPathDelay(int ticks);
    bool IsRefreshDelayed();
    void UpdateIntroAndDelays();
};

#endif
