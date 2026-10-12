#ifndef PacketcontrolsH
#define PacketcontrolsH

#include <Classes.hpp>
#include <ScktComp.hpp>

enum PacketAction
{
    AC_REQUEST = 1,
    AC_CONFIRM = 2,
    AC_RESULT = 3,
    AC_DELETE = 4,
    AC_UPDATE = 5,
    AC_NEW = 6,
    AC_ADD = 7,
    AC_SET = 8,
    AC_GET = 9,
    AC_DO = 10,
    AC_BUY = 11,
    AC_SELL = 12,
    AC_OPEN = 13,
    AC_CLOSE = 14,
    AC_PLAY = 15,
    AC_HIT = 16,
    AC_CAST = 17,
    AC_LIST = 18,
    AC_PRIVATE = 20,
    AC_PUBLIC = 21,
    AC_GLOBAL = 22,
    AC_SYSTEM = 23,
    AC_DROP = 24,
    AC_JUNK = 25,
    AC_GIVE = 26,
    AC_PICKUP = 27,
    AC_REMOVE = 28,
    AC_RANK = 29,
    AC_SELF = 30,
    AC_TARGET = 31,
    AC_GROUP = 33,
    AC_TALK = 34,
    AC_SYN1 = 240,
    AC_SYN2 = 241,
    AC_SYN3 = 242,
    AC_SYN4 = 243,
    AC_SYN5 = 244,
    AC_ERROR = 250
};

enum PacketFamily
{
    OP_SECURITY = 1,
    OP_ACCOUNT = 2,
    OP_CHARACTER = 3,
    OP_LOGIN = 4,
    OP_PLAY = 5,
    OP_MOVE = 6,
    OP_DIR = 7,
    OP_SIT = 8,
    OP_EMOTE = 9,
    OP_ATTACK = 11,
    OP_MAGIC = 12,
    OP_SHOP = 13,
    OP_ITEM = 14,
    OP_SKILL = 16,
    OP_PCHAT = 17,
    OP_CHAT = 18,
    OP_MAP = 19,
    OP_MUSIC = 21,
    OP_PLAYERS = 22,
    OP_PLAYER = 23,
    OP_GROUP = 24,
    OP_REFRESH = 25,
    OP_NPC = 26,
    OP_PLAYERRANGE = 27,
    OP_NPCRANGE = 28,
    OP_RANGE = 29,
    OP_PAPERDOLL = 30,
    OP_EFFECT = 31,
    OP_TRADE = 32,
    OP_CHEST = 33,
    OP_DOOR = 34,
    OP_SYSTEM = 35,
    OP_BANK = 36,
    OP_LOCKER = 37,
    OP_BARBER = 38,
    OP_GUILD = 39,
    OP_SOUND = 40,
    OP_REST = 41,
    OP_GAMEBAR = 42,
    OP_MSGBOARD = 43,
    OP_M_NPC = 44,
    OP_ARENA = 45,
    OP_PRIEST = 46,
    OP_LAW = 47,
    OP_ADMIN = 48,
    OP_INN = 49,
    OP_QUEST = 50,
    OP_BOOK = 51
};
#define ACCOUNT_CONFIRM_EDIT_Y 171
#define ACCOUNT_EDIT10_Y 362
#define ACCOUNT_EDIT8_Y 260
#define ACCOUNT_EDIT9_Y 311
#define ACCOUNT_FORM_EDIT_X 358
#define ACCOUNT_NAME_EDIT_Y 69
#define ACCOUNT_PASSWORD_EDIT_Y 120
#define FILE_SYSTEM_NAME_BUFFER_SIZE 1000
#define GRID_LOGIN_NAME_EDIT_Y 353
#define LOGIN_EDIT_X 402
#define LOGIN_NAME_EDIT_Y 315
#define LOGIN_PASSWORD_EDIT_Y 351
#define NUMBER_ENCODING_BASE 253
#define VOLUME_NAME_BUFFER_SIZE 1000

struct PacketOperator;
struct PacketProcessor;

// The connection to the game server: packet framing, the protocol's number encoding and byte scrambling, the chat
// word filter, and the distance tests the map draw and sound use.
struct Client
{
    bool init_rejected;
    char is_initialized;
    bool is_banned;
    bool account_reply_disconnect;
    bool login_retry_pending;
    bool is_permanent_ban;
    int ban_minutes;
    int unread_int_at_12;
    int unread_int_at_16;
    bool unread_flag_at_20;
    int login_failure_count;
    bool version_rejected;
    int server_challenge_hash;
    int challenge;
    int last_action_tick;
    int pending_action_tick;
    PacketOperator *sender;
    PacketProcessor *processor;
    String receive_buffer;
    String token_text;
    int token_position;
    int token_length;
    char token_separator;
    TStringList *word_filter;
    String unused_text_at_80;
    char pad_0x54[12];
    int sequence_start;
    int sequence_counter;
    int receive_key;
    int send_key;
    int player_id;
    int session_id;
    int admin_level;
    TStringList *spare_string_list;
    String unused_text_at_128;

    Client();
    ~Client();
    void LoadWordFilter();
    void SendInitRequest();
    void SetAdminLevel(int new_admin_level);
    int GetAdminLevel();
    void SetPlayerId(int new_player_id);
    int GetPlayerId();
    bool HandleRawPacket(String frame);
    void ReceiveData(Scktcomp::TCustomWinSocket *socket, String text);
    bool IsWithinReach(int from_x, int from_y, int to_x, int to_y);
    bool IsInSoundRange(int from_x, int from_y, int to_x, int to_y);
    bool IsWallInDrawRange(int from_x, int from_y, int to_x, int to_y);
    bool IsObjectInDrawRange(int from_x, int from_y, int to_x, int to_y);
    bool IsTallObjectInDrawRange(int from_x, int from_y, int to_x, int to_y);
    bool IsInDrawRange(int from_x, int from_y, int to_x, int to_y);
    bool IsAdjacent(int from_x, int from_y, int to_x, int to_y);
    unsigned int DecodeLength(String encoded);
    static String EncodeNumber(Client *self, unsigned int value, int min_length);
    unsigned int DecodeNumber(String encoded);
    int HashChallenge(int sent_challenge);
    String EncryptPacket(int key, String packet);
    static String ScrambleBytes(Client *self, int divisor, char *data_begin, char *data_end);
    static String UnscrambleBytes(Client *self, int divisor, unsigned char *data_begin, unsigned char *data_end);
    static String DeobfuscateString(Client *self, String text);
    static String FlipHighBits(Client *self, String text);
    bool SendPacket(int action, int family, String data);
    void ShowPendingScreen();
    void HandleDisconnect();
    static String MakeDrunkText(Client *self, String text);
    static String Capitalize(Client *self, String text);
    static String CensorText(Client *self, String text);
    void BeginTokens(String text, char separator);
    String RemainingTokens();
    String NextToken();
    void SetConnectionKeys(int new_sequence_start, int new_receive_key, int new_send_key);
    void SetSequenceStart(int new_sequence_start);
    void SetSessionId(int new_session_id);
    int GetSessionId();
    static String BuildPacket(Client *self, int key, int action, int family, String data);
    int ByteValue(unsigned char ch);
    char CanAct();
    void CommitActionTime();
    bool IsIpAddress(String text);
};

#endif
