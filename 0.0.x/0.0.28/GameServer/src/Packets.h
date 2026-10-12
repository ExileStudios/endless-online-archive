#ifndef PacketsH
#define PacketsH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

enum AccountReply
{
    AccountReply_Exists = 1,
    AccountReply_Created = 3,
    AccountReply_ChangeFailed = 5,
    AccountReply_Changed = 6,
    AccountReply_RequestDenied = 7
};

enum AdminMessageType
{
    AdminMessageType_Message = 1,
    AdminMessageType_Report = 2
};

enum CharacterReply
{
    CharacterReply_Exists = 1,
    CharacterReply_Full = 2,
    CharacterReply_FullRequest = 3,
    CharacterReply_NotApproved = 4,
    CharacterReply_Ok = 5,
    CharacterReply_Deleted = 6
};

enum GuildInfoType
{
    GuildInfoType_Description = 1,
    GuildInfoType_Ranks = 2,
    GuildInfoType_Bank = 3
};

enum GuildReply
{
    GuildReply_Busy = 1,
    GuildReply_NotApproved = 2,
    GuildReply_AlreadyMember = 3,
    GuildReply_NoCandidates = 4,
    GuildReply_Exists = 5,
    GuildReply_CreateBegin = 6,
    GuildReply_CreateAddConfirm = 7,
    GuildReply_CreateAdd = 8,
    GuildReply_RecruiterOffline = 9,
    GuildReply_RecruiterNotHere = 10,
    GuildReply_RecruiterWrongGuild = 11,
    GuildReply_NotRecruiter = 12,
    GuildReply_JoinRequest = 13,
    GuildReply_Accepted = 16,
    GuildReply_NotFound = 17,
    GuildReply_Updated = 18,
    GuildReply_RanksUpdated = 19,
    GuildReply_RemoveNotMember = 20,
    GuildReply_RemoveLeader = 21,
    GuildReply_Removed = 22,
    GuildReply_RankingLeader = 23,
    GuildReply_RankingNotMember = 24
};

enum LoginReply
{
    LoginReply_WrongUser = 1,
    LoginReply_WrongUserPassword = 2,
    LoginReply_Ok = 3,
    LoginReply_Banned = 4,
    LoginReply_LoggedIn = 5,
    LoginReply_Busy = 6
};

enum MapDamageType
{
    MapDamageType_TpDrain = 1,
    MapDamageType_Spikes = 2
};

enum MarriageReply
{
    MarriageReply_AlreadyMarried = 1,
    MarriageReply_NotMarried = 2,
    MarriageReply_Success = 3,
    MarriageReply_NotEnoughGold = 4,
    MarriageReply_WrongName = 5,
    MarriageReply_ServiceBusy = 6,
    MarriageReply_DivorceNotification = 7
};

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
    AC_SYN3 = 242
};

enum PartyReplyCode
{
    PartyReplyCode_AlreadyInAnotherParty = 0,
    PartyReplyCode_AlreadyInYourParty = 1,
    PartyReplyCode_PartyIsFull = 2
};

enum PriestReply
{
    PriestReply_NotDressed = 1,
    PriestReply_LowLevel = 2,
    PriestReply_PartnerNotPresent = 3,
    PriestReply_PartnerNotDressed = 4,
    PriestReply_Busy = 5,
    PriestReply_DoYou = 6,
    PriestReply_PartnerAlreadyMarried = 7,
    PriestReply_NoPermission = 8
};

enum PubFileType
{
    PubFileType_Emf = 1,
    PubFileType_Eif = 2,
    PubFileType_Enf = 3,
    PubFileType_Esf = 4,
    PubFileType_Ecf = 5
};

enum QuestPage
{
    QuestPage_Progress = 1,
    QuestPage_History = 2
};

enum SkillMasterReply
{
    SkillMasterReply_RemoveItems = 1,
    SkillMasterReply_WrongClass = 2
};

enum StatId
{
    StatId_Str = 1,
    StatId_Int = 2,
    StatId_Wis = 3,
    StatId_Agi = 4,
    StatId_Con = 5,
    StatId_Cha = 6
};

enum WarpEffect
{
    WarpEffect_None = 0,
    WarpEffect_Scroll = 1,
    WarpEffect_Admin = 2
};

enum WarpType
{
    WarpType_Local = 1,
    WarpType_MapSwitch = 2
};

enum WelcomeCode
{
    WelcomeCode_SelectCharacter = 1,
    WelcomeCode_EnterGame = 2,
    WelcomeCode_ServerBusy = 3
};
#define GLOBAL_HISTORY_SIZE 7
#define GUILD_RANK_NEW_MEMBER 9
#define GUILD_TAG_MIN_LENGTH 2
#define PING_HISTORY_SIZE 3

struct Banned;
struct KillCounters;
struct Logins;
struct MapContainer;
struct Player;
struct PlayerQuest;
struct Players;
struct QuestContainer;
struct QuestCounters;
struct QuestState;
struct MapCoord;
struct Settings;
struct WeaponMapper;
struct mySQLdb;
struct mySQLtask;


struct Packets;

// The game server: reads each player's packets and answers them, and runs the timed work of the game.
struct Packets
{
    int chat_log_lines;
    int chat_log_number;
    TStringList *wordfilter;
    WeaponMapper *weapon_map;
    Players *players;
    mySQLdb *mysql_controls;
    Logins *logins;
    Banned *banned;
    QuestContainer *quest_engine;
    MapContainer *map_control;
    Settings *settings;
    KillCounters *kill_counters;
    QuestCounters *quest_counters;
    TDateTime start_time;
    char *encode_buffer;
    char *packet_buffer;
    int version_major;
    int version_minor;
    int version_patch;
    int sent_bytes;
    int sent_kilobytes;
    int sent_megabytes;
    int received_bytes;
    int received_kilobytes;
    int received_megabytes;
    String reader_data;
    int reader_pos;
    int reader_len;
    char reader_break_byte;
    int online_names_ttl;
    String online_names_cache;
    int online_list_ttl;
    String online_list_cache;
    String global_history[GLOBAL_HISTORY_SIZE];
    int ping_history[PING_HISTORY_SIZE];
    int ticks;
    bool save_gate;
    bool kill_counters_cleared;
    bool shutting_down;
    int cheat_offset_x;
    int cheat_offset_y;

    Packets(MapContainer *map_control, QuestContainer *quest_engine, Players *players, Settings *settings,
        mySQLdb *mysql_controls, Logins *logins, int version_major, int version_minor, int version_patch);
    ~Packets();
    void Game_Tick();
    void Server_Shutdown();
    void Server_RemovePlayer(Scktcomp::TCustomWinSocket *socket);
    void Server_ClientRead(Scktcomp::TCustomWinSocket *socket, String data);
    bool Player_HandlePacket(Player *player, String data);
    void MysqlCallback_Dispatch(mySQLtask *query_result);
    void Player_FireQuestTriggers(Player *player, int event, int value);
    void Player_EvaluateQuestRules(Player *player, PlayerQuest *tracker, QuestState *state, int event, int arg);
    void Player_ApplyQuestActions(Player *player, PlayerQuest *tracker, char run_actions);
    void Login_SendCharacterList(Player *player, unsigned char action, unsigned char family, String data);
    String Party_EncodeMemberList(Player *player);
    String Walk_BuildReply(Player *player);
    String Refresh_BuildReply(Player *player);
    String Player_SerializeAvatar(Player *player, signed int warp_state);
    String Book_BuildReply(Player *player);
    String Paperdoll_BuildReply(Player *player);
    String Server_BuildOnlineNames();
    String Message_BuildPlayerList();
    String Server_BuildOnlineList();
    String NpcRange_Lookup(Player *player, int npc_index);
    bool Server_HandleInit(Player *player, String data);
    String Server_BuildInitOkReply(Player *player);
    String Server_BuildInitVersionReply();
    String Server_BuildInitBanReply();
    void Client_SendRaw(Player *client, String data, int reply_code);
    void Server_SendRawToMap(int map_id, String data, int reply_code);
    void Server_BroadcastToPartyExceptSelf(Player *player, unsigned char action, unsigned char family, String data);
    void Server_BroadcastToParty(Player *player, unsigned char action, unsigned char family, String data);
    void Server_BroadcastToPartyOnMap(Player *player, unsigned char action, unsigned char family, String data);
    void Guild_BroadcastToAll(Player *player, unsigned char action, unsigned char family, String data);
    void Server_BroadcastAdjacent(Player *player, MapCoord coords, unsigned char action, unsigned char family,
        String data);
    void Server_BroadcastToMapAndAdmins(int map_id, unsigned char action, unsigned char family, String data);
    void Server_BroadcastToMap(int map_id, unsigned char action, unsigned char family, String data);
    void Server_BroadcastNearTile(int skip_id, int map_id, MapCoord coord, unsigned char action, unsigned char family,
        String data);
    void Admin_BroadcastToOtherAdmins(Player *player, unsigned char action, unsigned char family, String data);
    void Admin_BroadcastToAll(unsigned char action, unsigned char family, String data);
    void Server_BroadcastToAll(unsigned char action, unsigned char family, String data);
    void Server_BroadcastToGlobalChat(Player *player, unsigned char action, unsigned char family, String data);
    void Server_BroadcastToAllExceptSelf(Player *player, unsigned char action, unsigned char family, String data);
    void Server_BroadcastNearby(Player *player, unsigned char action, unsigned char family, String data);
    void Client_SendEncoded(Player *player, unsigned char action, unsigned char family, String data);
    void Player_Respawn(Player *player);
    void Player_Warp(Player *player, int target_map, MapCoord coords, int warp_effect, char do_leave);
    char Player_CheckIdleWarp(Player *player, int x, int y);
    void Player_CalculateStats(Player *player);
    void Player_ApplyEquipmentBonuses(Player *player);
    void Server_SyncMapHazardFlags(int map_id);
    int Party_ShareExp(Player *player, int exp);
    bool Face_Execute(Player *player, int action, String *data);
    bool Chair_Execute(Player *player, int action, String *data);
    bool Attack_Execute(Player *attacker, int action, String *data);
    bool Spell_Execute(Player *caster, int action, String *data);
    bool Walk_Execute(Player *player, int action, String *data);
    void Connection_Ping();
    int Server_HashChallenge(int value);
    String Account_DecodePassword(String value);
    String Account_EncodePassword(String value);
    String EO_EncodeNumber(unsigned int value, signed int width);
    unsigned int Server_DecodePacketLength(String data);
    unsigned int EO_DecodeNumber(String data);
    String EO_Encode_Interleave(int multiple, char *begin, char *end);
    String EO_Decode_Deinterleave(int multiple, char *begin, char *end);
    int EO_DecodeByte(unsigned char value);
    char EO_GetBreakByte(int value);
    void Server_AddSentBytes(int value);
    void Server_AddReceivedBytes(int value);
    void PacketReader_Init(String data, char break_byte);
    String PacketReader_GetBreakString();
    String PacketReader_GetBreakStringAt(int field_index, String break_str, char separator);
    bool PassesWordFilter(String name);
    bool Login_CheckQueueThreshold();
    char Coords_IsAdjacent(int x1, int y1, int x2, int y2);
    char Server_InViewRange(int x1, int y1, int x2, int y2);
    char Server_InViewRing(int x1, int y1, int x2, int y2);
    char Server_InViewRangeReverse(int x1, int y1, int x2, int y2);
    char Coords_IsWithinTwo(int x1, int y1, int x2, int y2);
    char Server_InItemViewRing(int x1, int y1, int x2, int y2);
    String Server_FormatSentTraffic();
    String Server_FormatReceivedTraffic();
    char Server_TickOncePerFiveSeconds();
    void Server_AppendChatLog(String message);
};


#endif
