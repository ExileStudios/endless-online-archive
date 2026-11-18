#ifndef PacketprocesH
#define PacketprocesH

#define CHAT_COLOR_NOTICE 4
#define ENTRY_ID_LENGTH 2
#define MINUTES_PER_HOUR 60

enum AccountReply
{
    AccountReply_NameDialog24 = 1,
    AccountReply_NameDialog25 = 2,
    AccountReply_Created = 3,
    AccountReply_FieldsDialog27 = 5,
    AccountReply_ResetDialog28 = 6,
    AccountReply_Disconnect = 7,
    AccountReply_WrongPin = 8,
    AccountReply_SessionBase = 10
};

enum AppearanceChange
{
    AppearanceChange_Equipment = 1,
    AppearanceChange_Hair = 2,
    AppearanceChange_HairColor = 3,
    AppearanceChange_Skin = 4,
    AppearanceChange_Gender = 5
};

enum CharacterReply
{
    CharacterReply_RetryDialog31 = 1,
    CharacterReply_ClosedDialog32 = 2,
    CharacterReply_Unknown3 = 3,
    CharacterReply_RetryDialog33 = 4,
    CharacterReply_Created = 5,
    CharacterReply_ListChanged = 6,
    CharacterReply_SessionBase = 10
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
    GuildReply_Unknown14 = 14,
    GuildReply_Unknown15 = 15,
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
    LoginReply_FailedDialog21 = 1,
    LoginReply_FailedDialog22 = 2,
    LoginReply_Ok = 3,
    LoginReply_Banned = 4,
    LoginReply_RefusedDialog23 = 5,
    LoginReply_RetryLater = 6,
    LoginReply_LookupsRefused = 7,
    LoginReply_ServerBusy = 8
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

enum WelcomeReply
{
    WelcomeReply_FileCheck = 1,
    WelcomeReply_EnterGame = 2,
    WelcomeReply_RejectedDialog19 = 3,
    WelcomeReply_RejectedDialog20 = 4
};

// Reads every packet from the server and hands its contents to the controllers.
struct PacketProcessor
{
    char unused_bytes[8];

    PacketProcessor();
    ~PacketProcessor();
    void DispatchPacket(int action, int family, String packet);
    void ParseBook(String packet);
    void ParsePaperdoll(String packet);
    void ApplyAppearanceChange(String packet);
    void ParseWelcomeCharacter(String packet);
    void ParseSpellList(String packet);
    void ParseInventory(String packet);
    void ParseNearbyEntities(String packet);
    void ParseOnlineNames(String packet);
    void ParseOnlinePlayers(String packet);
    void ParsePartyMembers(String packet);
    void ParseCharacterList(String packet);
    String EncodeNumber(unsigned int value, int width);
    unsigned int DecodeNumber(String encoded);
    char ByteToChar(unsigned char value);
    String FormatPercent(int value);
};

#endif
