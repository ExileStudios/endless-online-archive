#ifndef PacketoperationsH
#define PacketoperationsH

enum PaperdollSubSlot
{
    PaperdollSubSlot_First = 0,
    PaperdollSubSlot_Second = 1
};

#define TOKEN_BREAK_BYTE 255

// Builds every request the client sends to the server, one function per request.
// A request made in an NPC window carries session_value, the id the server sent when that window opened.
struct PacketOperator
{
    char unused_bytes[8];

    PacketOperator();
    ~PacketOperator();
    void RequestAccountNameCheck(String account_name);
    void SendFindPlayer(String name);
    void SendAccountCreate(String account_name, String password, String real_name, String location, String email);
    void SendChangePassword(String account_name, String old_password, String new_password);
    void RequestCharacterCreate();
    void RequestCharacterDelete(int character_id);
    void ConfirmCharacterDelete();
    void SendCharacterCreate(String name, int gender, int hair_style, int hair_color, int skin);
    void SelectCharacter(int character_id);
    void SendPrivateMessagesOn();
    void SendPrivateMessagesOff();
    void SendGlobalChatClose();
    void SendGlobalChatOpen();
    void SendWalkGhost(int direction, int x, int y, int blocking_player_id);
    void SendWalkNoWall(int direction, int x, int y);
    void SendWalk(int direction, int x, int y);
    void SendPrivateMessage(String name, String text);
    void SendAdminChat(String text);
    void SendGuildChat(String text);
    void SendAnnouncement(String text);
    void SendGlobalChat(String text);
    void SendPartyChat(String text);
    void SendPublicChat(String text);
    void SendReportPlayer(String name, String reason);
    void SendHelpQuestion(String question);
    void SendEmote(int emote);
    void SendTrainStat(int stat_id);
    void SendTrainSkill(int skill_id);
    void SendFace(int direction);
    void SendSpellChant(int spell_id);
    void SendSpellCastDirectional(int direction, int spell_id);
    void SendSpellCastGroup(int spell_id);
    void SendSpellCastOnPlayer(int spell_id, int player_id);
    void SendSpellCastOnNpc(int spell_id, int npc_index);
    void SendAttack(int direction);
    void SendChairSit(int x, int y);
    void SendFloorSit(int x, int y);
    void SendFloorStand();
    void SendChairStand();
    void RequestRefresh();
    void RequestPaperdoll(int player_id);
    void RequestBook(int player_id);
    void SendEquipItem(int item_id, int sub_slot);
    void SendUnequipItem(int item_id, int sub_slot);
    void RequestOnlinePlayers();
    void RequestOnlineNames();
    void RequestQuestProgress();
    void RequestQuestHistory();
    void RequestBankOpen(int npc_index);
    void SendGiveUpCitizenship(int session_id);
    void RequestInnSleep(int session_value);
    void ConfirmInnSleep(int session_value);
    void SendCitizenshipAnswers(int session_value, String answer_first, String answer_second, String answer_third);
    void RequestInnOpen(int npc_index);
    void SendBankWithdraw(int session_value, int amount);
    void SendBankDeposit(int session_value, int amount);
    void RequestPriestOpen(int npc_index);
    void RequestWedding(int request_id, String partner_name);
    void ConfirmWeddingVow(int request_id);
    void AcceptWeddingRequest(int request_id);
    void RequestBarberOpen(int npc_index);
    void SendBuyHairstyle(int session_value, int hair_style, int hair_color);
    void RequestQuestDialog(int npc_index, int quest_id);
    void SendQuestDialogOk(int dialog_echo_a, int quest_id, int dialog_echo_b);
    void SendQuestDialogLink(int dialog_echo_a, int quest_id, int dialog_echo_b, int link_id);
    void RequestSkillMasterOpen(int npc_index);
    void SendSkillReset(int session_value);
    void SendForgetSkill(int session_value, int skill_id);
    void SendLearnSkill(int session_value, int skill_id);
    void RequestLawOpen(int npc_index);
    void SendLawMenuOption1Request(int session_value, String name);
    void SendLawMenuOption2Request(int session_value, String name);
    void RequestGuildOpen(int npc_index);
    void RemoveGuildMember(int session_value, String member_name);
    void DisbandGuild(int session_value);
    void RequestGuildCreate(int session_value, String tag, String guild_name);
    void RequestGuildInfo(int session_value, String guild_name);
    void RequestGuildMemberList(int session_value, String guild_name);
    void SendGuildCreate(int session_value, String tag, String guild_name, String description);
    void AcceptGuildInvite(int session_value, int player_id);
    void RequestGuildDescription(int session_value, String guild_name);
    void SendGuildDeposit(int session_value, int amount);
    void RequestGuildBank(int session_value);
    void RequestGuildRanks(int session_value, String guild_name);
    void SetGuildMemberRank(int session_value, String member_name, int rank);
    void SetGuildRankNames(int session_value, String rank_name_first, String rank_name_second, String rank_name_third,
    String rank_name_fourth, String rank_name_fifth, String rank_name_sixth, String rank_name_seventh,
    String rank_name_eighth, String rank_name_ninth);
    void SetGuildDescription(int session_value, String description);
    void RequestGuildJoin(int session_value, String guild_tag, String recruiter_name);
    void AcceptGuildJoinRequest(int player_id);
    void LeaveGuild(int session_value, String guild_name);
    void SendBoardDeletePost(int board_id, int post_id);
    void SendBoardPost(int board_id, String subject, String body);
    void RequestBoardOpen(int board_id);
    void RequestBoardPost(int board_id, int post_id);
    void RequestShopOpen(int npc_index);
    void SendCraftItem(int session_value, int item_id);
    void SendBuyItem(int session_value, int item_id, int amount);
    void SendSellItem(int session_value, int item_id, int amount);
    void RequestJukeboxOpen(TPoint position);
    void SendJukeboxRequest(TPoint x, int y);
    void SendOpenDoor(int instrument_id, int note_id);
    void SendPlayInstrument(int instrument_id, int note_id);
    void SendDropItem(int item_id, int amount, int x, int y);
    void SendJunkItem(int item_id, int amount);
    void SendPickupItem(int item_index);
    void SendUseItem(int item_id);
    void SendLogin(String account_name, String password);
    void SendConnectionPong();
    void RequestGameData();
    void RequestChestOpen(TPoint player_id);
    void SendChestAddItem(TPoint position, int item_id, int amount);
    void SendChestTakeItem(TPoint item_id, int amount);
    void RequestLockerOpen(TPoint item_id);
    void SendBuyLockerUpgrade();
    void SendLockerAddItem(TPoint position, int item_id, int amount);
    void SendLockerTakeItem(TPoint position, int item_id);
    void SendTradeAgree();
    void SendTradeDisagree();
    void SendTradeClose();
    void RequestTrade(int player_id);
    void SendTradeAddItem(int item_id, int amount);
    void SendTradeRemoveItem(int item_id);
    void RequestPartyJoin(int player_id);
    void RequestPartyInvite(int player_id);
    void AcceptPartyJoin(int player_id);
    void AcceptTrade(int player_id);
    void AcceptPartyInvite(int player_id);
    void SendPingRequest();
    char ByteToChar(unsigned char value);
};

#endif
