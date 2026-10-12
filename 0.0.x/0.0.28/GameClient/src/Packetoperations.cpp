#include <vcl.h>
#pragma hdrstop

#include "Packetoperations.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_menu.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

PacketOperator::PacketOperator()
{
}

PacketOperator::~PacketOperator()
{
}

// Sends the account name on its own, before SendAccountCreate sends the whole form.
void PacketOperator::RequestAccountNameCheck(String account_name)
{
    if (account_name.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_ACCOUNT, account_name);
}

// Asks where a player is; the caller sends names of 4 to 16 letters.
void PacketOperator::SendFindPlayer(String name)
{
    if (name.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_PLAYERS, name);
}

// Sends the new account form; the computer name and the serial number of drive c: go with it.
void PacketOperator::SendAccountCreate(String account_name, String password, String real_name, String location,
    String email)
{
    DWORD nSize;
    DWORD VolumeSerialNumber;
    DWORD MaximumComponentLength;
    DWORD FileSystemFlags;
    char Buffer[16];
    char VolumeNameBuffer[VOLUME_NAME_BUFFER_SIZE];
    char FileSystemNameBuffer[FILE_SYSTEM_NAME_BUFFER_SIZE];
    if (MainForm->net_client->GetSessionId() >= 1)
    {
        nSize = 16;
        Buffer[0] = 0;
        GetComputerNameA(Buffer, &nSize);
        String computer_name = Buffer;
        GetVolumeInformationA("c:\\", VolumeNameBuffer, 1000, &VolumeSerialNumber, &MaximumComponentLength,
            &FileSystemFlags, FileSystemNameBuffer, 1000);
        String serial_text = VolumeSerialNumber;
        // Each is cut to 32 characters.
        if (computer_name.Length() >= 33)
            computer_name = computer_name.SubString(1, 32);
        if (serial_text.Length() >= 33)
            serial_text = serial_text.SubString(1, 32);
        String payload = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + account_name;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + password;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + real_name;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + location;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + email;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + computer_name;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        payload = payload + serial_text;
        payload = payload + String(ByteToChar(TOKEN_BREAK_BYTE));
        if (payload.Length() >= 1)
            MainForm->net_client->SendPacket(AC_NEW, OP_ACCOUNT, payload);
    }
}

// Sends the account name, the old password and the new one, each ended by a byte of 255.
void PacketOperator::SendChangePassword(String account_name, String old_password, String new_password)
{
    String packet = packet + account_name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + old_password;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + new_password;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_UPDATE, OP_ACCOUNT, packet);
}

// Asks to open the new character form.
void PacketOperator::RequestCharacterCreate()
{
    String packet = "NEW";
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_CHARACTER, packet);
}

// Asks to delete a character; the server asks for a confirmation, which ConfirmCharacterDelete sends.
void PacketOperator::RequestCharacterDelete(int character_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, character_id, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_CHARACTER, packet);
}

// Confirms the delete of the character the menu asked about.
void PacketOperator::ConfirmCharacterDelete()
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->animation->menu->delete_character_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DELETE, OP_CHARACTER, packet);
}

// Sends the new character: gender, hair style, hair color and skin, then the name.
void PacketOperator::SendCharacterCreate(String name, unsigned int gender, unsigned int hair_style,
    unsigned int hair_color, unsigned int skin)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, gender, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, hair_style, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, hair_color, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, skin, 2);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_NEW, OP_CHARACTER, packet);
}

// Logs in with one character of the account.
void PacketOperator::SelectCharacter(int character_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, character_id, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAY, packet);
}

void PacketOperator::SendPrivateMessagesOn()
{
    MainForm->net_client->SendPacket(AC_DELETE, OP_PCHAT, "n");
}

void PacketOperator::SendPrivateMessagesOff()
{
    MainForm->net_client->SendPacket(AC_SET, OP_PCHAT, "y");
}

void PacketOperator::SendGlobalChatClose()
{
    MainForm->net_client->SendPacket(AC_CLOSE, OP_PCHAT, "n");
}

void PacketOperator::SendGlobalChatOpen()
{
    MainForm->net_client->SendPacket(AC_OPEN, OP_PCHAT, "y");
}

// A step through the player who has blocked the way for a while: the direction, the action tick, then the tile.
void PacketOperator::SendWalkGhost(unsigned int direction, unsigned int x, unsigned int y, int blocking_player_id)
{
    MainForm->net_client->CommitActionTime();
    String packet = packet + Client::EncodeNumber(MainForm->net_client, direction, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_HIT, OP_MOVE, packet);
}

// A step while walking through walls, which only admins of level 3 and up can turn on.
void PacketOperator::SendWalkNoWall(unsigned int direction, unsigned int x, unsigned int y)
{
    MainForm->net_client->CommitActionTime();
    String packet = packet + Client::EncodeNumber(MainForm->net_client, direction, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CAST, OP_MOVE, packet);
}

// A step: the direction, the action tick, then the tile the player walks to.
void PacketOperator::SendWalk(unsigned int direction, unsigned int x, unsigned int y)
{
    MainForm->net_client->CommitActionTime();
    String packet = packet + Client::EncodeNumber(MainForm->net_client, direction, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SET, OP_MOVE, packet);
}

// Sends a private message: the name, a byte of 255, then the text.
void PacketOperator::SendPrivateMessage(String name, String text)
{
    String packet = name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PRIVATE, OP_CHAT, packet);
}

void PacketOperator::SendAdminChat(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CAST, OP_CHAT, packet);
}

void PacketOperator::SendGuildChat(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_CHAT, packet);
}

void PacketOperator::SendAnnouncement(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GLOBAL, OP_CHAT, packet);
}

void PacketOperator::SendGlobalChat(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PLAY, OP_CHAT, packet);
}

void PacketOperator::SendPartyChat(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_CHAT, packet);
}

void PacketOperator::SendPublicChat(String text)
{
    String packet = text;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PUBLIC, OP_CHAT, packet);
}

// Reports a player: the name, a byte of 255, then the reason; nothing is sent when either is empty.
void PacketOperator::SendReportPlayer(String name, String reason)
{
    if (reason.Length() < 1)
        return;
    if (name.Length() < 1)
        return;
    String packet = name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + reason;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PUBLIC, OP_ADMIN, packet);
    return;
}

void PacketOperator::SendHelpQuestion(String question)
{
    if (question.Length() < 1)
        return;
    String packet = question;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PRIVATE, OP_ADMIN, packet);
}

// Shows an emote over the player.
void PacketOperator::SendEmote(int emote)
{
    String packet = Client::EncodeNumber(MainForm->net_client, emote, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PUBLIC, OP_EMOTE, packet);
}

// Trains a stat: kind 1, then the stat.
void PacketOperator::SendTrainStat(int stat_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, stat_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_SKILL, packet);
}

// Trains a skill: kind 2, then the skill.
void PacketOperator::SendTrainSkill(int skill_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, skill_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_SKILL, packet);
}

// Turns the player to face a direction without walking.
void PacketOperator::SendFace(int direction)
{
    String packet = Client::EncodeNumber(MainForm->net_client, direction, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SET, OP_DIR, packet);
}

// Starts a spell: its id and the time it was started.
void PacketOperator::SendSpellChant(int spell_id)
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    // Hundredths of a second since midnight.
    int tick = now.Time / 10;
    MainForm->net_client->CommitActionTime();
    String packet = Client::EncodeNumber(MainForm->net_client, spell_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_MAGIC, packet);
}

// Casts a spell in a direction: the direction, the spell and the time.
void PacketOperator::SendSpellCastDirectional(int direction, int spell_id)
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    int tick = now.Time / 10;
    String packet = Client::EncodeNumber(MainForm->net_client, direction, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, spell_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SELF, OP_MAGIC, packet);
}

// Casts a spell on the player's group: the spell and the time, with no target.
void PacketOperator::SendSpellCastGroup(int spell_id)
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    int tick = now.Time / 10;
    String packet = Client::EncodeNumber(MainForm->net_client, spell_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GROUP, OP_MAGIC, packet);
}

// Casts a spell on a player; the first byte says what the target is: 1 = a player, 2 = an npc.
void PacketOperator::SendSpellCastOnPlayer(unsigned int spell_id, unsigned int player_id)
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    int tick = now.Time / 10;
    MainForm->net_client->CommitActionTime();
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    packet = packet + Client::EncodeNumber(MainForm->net_client, spell_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_TARGET, OP_MAGIC, packet);
}

// Casts a spell on an npc, with target kind 2.
void PacketOperator::SendSpellCastOnNpc(unsigned int spell_id, unsigned int npc_index)
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    int tick = now.Time / 10;
    MainForm->net_client->CommitActionTime();
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    packet = packet + Client::EncodeNumber(MainForm->net_client, spell_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_TARGET, OP_MAGIC, packet);
}

// An attack in a direction, with the action tick.
void PacketOperator::SendAttack(int direction)
{
    MainForm->net_client->CommitActionTime();
    String packet = Client::EncodeNumber(MainForm->net_client, direction, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->last_action_tick, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DO, OP_ATTACK, packet);
}

// Sits on the chair at a tile: kind 1, then the tile. SendChairStand sends kind 2.
void PacketOperator::SendChairSit(int x, int y)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_SIT, packet);
}

// Sits on the floor: kind 1, then the tile. SendFloorStand sends kind 2.
void PacketOperator::SendFloorSit(int x, int y)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_REST, packet);
}

void PacketOperator::SendFloorStand()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_REST, packet);
}

void PacketOperator::SendChairStand()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_SIT, packet);
}

// Asks the server to send the map's players, npcs and items again.
void PacketOperator::RequestRefresh()
{
    String packet = String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_REFRESH, packet);
}

// Asks for the paperdoll of a player.
void PacketOperator::RequestPaperdoll(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_PAPERDOLL, packet);
}

// Asks for the book of a player.
void PacketOperator::RequestBook(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_BOOK, packet);
}

// Equips an item in slot 0 or 1 of its kind.
void PacketOperator::SendEquipItem(int item_id, int sub_slot)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, sub_slot, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_PAPERDOLL, packet);
}

// Takes off the item in slot 0 or 1 of its kind.
void PacketOperator::SendUnequipItem(int item_id, int sub_slot)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, sub_slot, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DELETE, OP_PAPERDOLL, packet);
}

// Asks for the list of players online, for the player list.
void PacketOperator::RequestOnlinePlayers()
{
    String packet = String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERS, packet);
}

// Asks for the names of players online, for the friend list.
void PacketOperator::RequestOnlineNames()
{
    String packet = String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_LIST, OP_PLAYERS, packet);
}

// Asks for the quests in progress: kind 1.
void PacketOperator::RequestQuestProgress()
{
    MainForm->net_client->SendPacket(AC_LIST, OP_QUEST, Client::EncodeNumber(MainForm->net_client, 1, 1));
}

// Asks for the quests done: kind 2.
void PacketOperator::RequestQuestHistory()
{
    MainForm->net_client->SendPacket(AC_LIST, OP_QUEST, Client::EncodeNumber(MainForm->net_client, 2, 1));
}

void PacketOperator::RequestBankOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_BANK, packet);
}

void PacketOperator::SendGiveUpCitizenship(int session_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_id, 2);
    MainForm->net_client->SendPacket(AC_DELETE, OP_INN, packet);
}

// Asks the inn for a sleep; ConfirmInnSleep answers the inn's question that follows.
void PacketOperator::RequestInnSleep(int session_value)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 2);
    MainForm->net_client->SendPacket(AC_REQUEST, OP_INN, packet);
}

void PacketOperator::ConfirmInnSleep(int session_value)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 2);
    MainForm->net_client->SendPacket(AC_CONFIRM, OP_INN, packet);
}

// Answers the three questions of the inn's citizenship form, each after a byte of 255.
void PacketOperator::SendCitizenshipAnswers(unsigned int session_value, String answer_first, String answer_second,
    String answer_third)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 2);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + answer_first;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + answer_second;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + answer_third;
    MainForm->net_client->SendPacket(AC_RESULT, OP_INN, packet);
}

void PacketOperator::RequestInnOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_INN, packet);
}

// Takes gold out of the bank: the amount, then session_value.
void PacketOperator::SendBankWithdraw(int session_value, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, amount, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_BANK, packet);
}

// Puts gold in the bank: the amount, then session_value.
void PacketOperator::SendBankDeposit(int session_value, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, amount, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_BANK, packet);
}

void PacketOperator::RequestPriestOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_PRIEST, packet);
}

// Asks the priest for a wedding with the named partner.
void PacketOperator::RequestWedding(int request_id, String partner_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, request_id, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + partner_name;
    MainForm->net_client->SendPacket(AC_REQUEST, OP_PRIEST, packet);
}

void PacketOperator::ConfirmWeddingVow(int request_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, request_id, 4);
    MainForm->net_client->SendPacket(AC_DO, OP_PRIEST, packet);
}

void PacketOperator::AcceptWeddingRequest(int request_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, request_id, 2);
    MainForm->net_client->SendPacket(AC_CONFIRM, OP_PRIEST, packet);
}

void PacketOperator::RequestBarberOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_BARBER, packet);
}

// Buys a hair style and color at the barber.
void PacketOperator::SendBuyHairstyle(int session_value, int hair_style, int hair_color)
{
    String packet = Client::EncodeNumber(MainForm->net_client, hair_style, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, hair_color, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_BUY, OP_BARBER, packet);
}

// Opens the quest dialog of an npc.
void PacketOperator::RequestQuestDialog(int npc_index, int quest_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, quest_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DO, OP_QUEST, packet);
}

// Answers a quest dialog with OK.
void PacketOperator::SendQuestDialogOk(unsigned int dialog_echo_a, unsigned int quest_id, unsigned int dialog_echo_b)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, dialog_echo_a, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, quest_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, dialog_echo_b, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 0, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_QUEST, packet);
}

// Answers a quest dialog with the link the player clicked; the values come back as the dialog sent them.
void PacketOperator::SendQuestDialogLink(unsigned int dialog_echo_a, unsigned int quest_id, unsigned int dialog_echo_b,
    unsigned int link_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, dialog_echo_a, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, quest_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, dialog_echo_b, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 2, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, link_id, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_QUEST, packet);
}

void PacketOperator::RequestSkillMasterOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_SKILL, packet);
}

// Asks the skill master for a reset, once the player has said yes.
void PacketOperator::SendSkillReset(int session_value)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 4)
        MainForm->net_client->SendPacket(AC_JUNK, OP_SKILL, packet);
}

// Asks the skill master to take a skill away.
void PacketOperator::SendForgetSkill(int session_value, int skill_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, skill_id, 2);
    if (packet.Length() >= 6)
        MainForm->net_client->SendPacket(AC_DELETE, OP_SKILL, packet);
}

// Buys a skill from the skill master.
void PacketOperator::SendLearnSkill(int session_value, int skill_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, skill_id, 2);
    if (packet.Length() >= 6)
        MainForm->net_client->SendPacket(AC_GET, OP_SKILL, packet);
}

void PacketOperator::RequestLawOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_LAW, packet);
}

// The first request of the law window: kind 1, session_value, then the text typed in, of 4 letters or more.
void PacketOperator::SendLawMenuOption1Request(unsigned int session_value, String name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_LAW, packet);
}

// The second request of the law window: kind 2, session_value, then the text typed in, of 4 letters or more.
void PacketOperator::SendLawMenuOption2Request(unsigned int session_value, String name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_LAW, packet);
}

void PacketOperator::RequestGuildOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_GUILD, packet);
}

// Takes a member out of the guild.
void PacketOperator::RemoveGuildMember(int session_value, String member_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + member_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REMOVE, OP_GUILD, packet);
}

// Ends the guild.
void PacketOperator::DisbandGuild(int session_value)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_JUNK, OP_GUILD, packet);
}

// The first step of making a guild: the tag and the name. SendGuildCreate is the last, with the description.
void PacketOperator::RequestGuildCreate(unsigned int session_value, String tag, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + tag;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + guild_name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_GUILD, packet);
}

// Asks for the information page of a guild.
void PacketOperator::RequestGuildInfo(int session_value, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + guild_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PUBLIC, OP_GUILD, packet);
}

// Asks for the member list of a guild.
void PacketOperator::RequestGuildMemberList(int session_value, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + guild_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PRIVATE, OP_GUILD, packet);
}

// Creates a guild: the tag, the name with a capital first letter, and the description.
void PacketOperator::SendGuildCreate(unsigned int session_value, String tag, String guild_name, String description)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + tag;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + Client::Capitalize(MainForm->net_client, guild_name);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + description;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_NEW, OP_GUILD, packet);
}

// Accepts an invite to a guild; its one caller passes 20202 for session_value.
void PacketOperator::AcceptGuildInvite(int session_value, int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_GUILD, packet);
}

// Asks for one part of the player's own guild, picked by the 2-byte kind after session_value:
// 1 = description (RequestGuildDescription)
// 2 = rank names (RequestGuildRanks)
// 3 = bank (RequestGuildBank)
void PacketOperator::RequestGuildDescription(int session_value, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 1, 2);
    packet = packet + guild_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_GUILD, packet);
}

// Puts gold in the guild bank.
void PacketOperator::SendGuildDeposit(int session_value, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_BUY, OP_GUILD, packet);
}

void PacketOperator::RequestGuildBank(int session_value)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 3, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_GUILD, packet);
}

void PacketOperator::RequestGuildRanks(int session_value, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 2, 2);
    packet = packet + guild_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_GUILD, packet);
}

// Gives a member a rank: session_value, the rank, then the name.
void PacketOperator::SetGuildMemberRank(int session_value, String member_name, int rank)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, rank, 1);
    packet = packet + member_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_RANK, OP_GUILD, packet);
}

// Sends the nine guild rank names, each ended by a byte of 255; SetGuildDescription uses the same request with kind 1.
void PacketOperator::SetGuildRankNames(unsigned int session_value, String rank_name_first, String rank_name_second, String rank_name_third, String rank_name_fourth, String rank_name_fifth, String rank_name_sixth,
    String rank_name_seventh, String rank_name_eighth, String rank_name_ninth)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 2, 2);
    packet = packet + rank_name_first;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_second;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_third;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_fourth;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_fifth;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_sixth;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_seventh;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_eighth;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + rank_name_ninth;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_UPDATE, OP_GUILD, packet);
}

void PacketOperator::SetGuildDescription(int session_value, String description)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, 1, 2);
    packet = packet + description;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_UPDATE, OP_GUILD, packet);
}

// Asks to join a guild with the two lines of the join form, the tag first.
void PacketOperator::RequestGuildJoin(unsigned int session_value, String guild_tag, String recruiter_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + guild_tag;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + recruiter_name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SET, OP_GUILD, packet);
}

// Lets in a player who asked to join the guild.
void PacketOperator::AcceptGuildJoinRequest(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DO, OP_GUILD, packet);
}

// Leaves the player's guild.
void PacketOperator::LeaveGuild(int session_value, String guild_name)
{
    String packet = Client::EncodeNumber(MainForm->net_client, session_value, 4);
    packet = packet + guild_name;
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DELETE, OP_GUILD, packet);
}

// Deletes a post from a message board.
void PacketOperator::SendBoardDeletePost(int board_id, int post_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, board_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, post_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DELETE, OP_MSGBOARD, packet);
}

// Posts on a message board: the subject and the text, each ended by a byte of 255.
void PacketOperator::SendBoardPost(unsigned int board_id, String subject, String body)
{
    String packet = Client::EncodeNumber(MainForm->net_client, board_id, 2);
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + subject;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + body;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_NEW, OP_MSGBOARD, packet);
}

void PacketOperator::RequestBoardOpen(int board_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, board_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_MSGBOARD, packet);
}

// Asks for one post of a message board.
void PacketOperator::RequestBoardPost(int board_id, int post_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, board_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, post_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_MSGBOARD, packet);
}

void PacketOperator::RequestShopOpen(int npc_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, npc_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_SHOP, packet);
}

// Crafts an item at the shop.
void PacketOperator::SendCraftItem(int session_value, int item_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_NEW, OP_SHOP, packet);
}

// Buys an amount of an item at the shop.
void PacketOperator::SendBuyItem(int session_value, int item_id, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_BUY, OP_SHOP, packet);
}

// Sells an amount of an item at the shop.
void PacketOperator::SendSellItem(int session_value, int item_id, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 4);
    packet = packet + Client::EncodeNumber(MainForm->net_client, session_value, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SELL, OP_SHOP, packet);
}

// Opens the jukebox at a tile.
void PacketOperator::RequestJukeboxOpen(Windows::TPoint position)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_MUSIC, packet);
}

// Asks the jukebox at a tile to play a song.
void PacketOperator::SendJukeboxRequest(Windows::TPoint position, int song_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, song_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PLAY, OP_MUSIC, packet);
}

// Opens the door at a tile.
void PacketOperator::SendOpenDoor(int x, int y)
{
    String packet = Client::EncodeNumber(MainForm->net_client, x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_DOOR, packet);
}

// Plays one note, up to 36, on an instrument.
void PacketOperator::SendPlayInstrument(int instrument_id, int note_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, instrument_id, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, note_id, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DO, OP_MUSIC, packet);
}

// Drops an item on a tile, or where the player stands when x or y is below 0.
void PacketOperator::SendDropItem(unsigned int item_id, signed int amount, signed int x, signed int y)
{
    char result = 1;
    if (item_id == 1 && MainForm->animation->interface_controller->gold_drop_warning_pending)
    {
        // The first big gold drop after entering the game is stopped once with a warning.
        if (1000 * MainForm->animation->interface_controller->status->level + 1000 <= amount)
        {
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_SafetyCommentOnceReally, false),
                MainForm->strings->GetDialogText(DialogText_SafetyCommentOnceReally, true));
            MainForm->redraw_needed = true;
            MainForm->animation->interface_controller->gold_drop_warning_pending = false;
            result = 0;
        }
    }
    if (result)
    {
        String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 3);
        if (x >= 0 && y >= 0)
        {
            packet = packet + Client::EncodeNumber(MainForm->net_client, x, 1);
            packet = packet + Client::EncodeNumber(MainForm->net_client, y, 1);
        }
        else
        {
            packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
            packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
        }
        if (packet.Length() >= 1)
            MainForm->net_client->SendPacket(AC_DROP, OP_ITEM, packet);
    }
}

// Destroys an amount of an item.
void PacketOperator::SendJunkItem(int item_id, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_JUNK, OP_ITEM, packet);
}

// Picks up the map item with this index.
void PacketOperator::SendPickupItem(int item_index)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_index, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_PICKUP, OP_ITEM, packet);
}

// Uses an item from the inventory.
void PacketOperator::SendUseItem(int item_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DO, OP_ITEM, packet);
}

// Logs in with the account name and password, each ended by a byte of 255.
void PacketOperator::SendLogin(String account_name, String password)
{
    String packet = packet + account_name;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    packet = packet + password;
    packet = packet + String(ByteToChar(TOKEN_BREAK_BYTE));
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_LOGIN, packet);
}

// Answers the server's connection check, after taking its new packet sequence.
void PacketOperator::SendConnectionPong()
{
    MainForm->net_client->SendPacket(AC_SYN1, OP_SECURITY, "k");
}

// Asks for the next needed part of the map or a data file; with none left, loads the files and enters the game.
void PacketOperator::RequestGameData()
{
    int action;
    int family;
    char request_built;
    action = AC_UPDATE;
    family = OP_PLAY;
    String packet;
    request_built = 0;
    // The first byte says what is asked for:
    // 1 = the map
    // 2 = an item file part
    // 3 = an npc file part
    // 4 = a skill file part
    // 5 = a class file part
    if (!request_built && MainForm->animation->menu->IsMapNeeded())
    {
        packet = packet + Client::EncodeNumber(MainForm->net_client, 1, 1);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->map->map_id, 2);
        request_built = 1;
    }
    if (!request_built && MainForm->animation->menu->IsItemFileNeeded())
    {
        packet = packet + Client::EncodeNumber(MainForm->net_client, 2, 1);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->item_values->received_part_count, 1);
        request_built = 1;
    }
    if (!request_built && MainForm->animation->menu->IsNpcFileNeeded())
    {
        packet = packet + Client::EncodeNumber(MainForm->net_client, 3, 1);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->npc_values->received_part_count, 1);
        request_built = 1;
    }
    if (!request_built && MainForm->animation->menu->IsSkillFileNeeded())
    {
        packet = packet + Client::EncodeNumber(MainForm->net_client, 4, 1);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->skill_values->received_part_count, 1);
        request_built = 1;
    }
    if (!request_built && MainForm->animation->menu->IsClassFileNeeded())
    {
        packet = packet + Client::EncodeNumber(MainForm->net_client, 5, 1);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 2);
        packet = packet + Client::EncodeNumber(MainForm->net_client, MainForm->class_values->received_part_count, 1);
        request_built = 1;
    }
    if (!request_built)
    {
        MainForm->item_values->Load();
        MainForm->skill_values->Load();
        MainForm->npc_values->Load();
        MainForm->class_values->Load();
        action = AC_PLAY;
        family = OP_PLAY;
        packet = Client::EncodeNumber(MainForm->net_client, MainForm->net_client->GetSessionId(), 3);
        packet = packet +
            Client::EncodeNumber(MainForm->net_client, MainForm->animation->menu->selected_character_id, 4);
    }
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(action, family, packet);
}

// Opens the chest at a tile.
void PacketOperator::RequestChestOpen(Windows::TPoint position)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_CHEST, packet);
}

// Puts an amount of an item in the chest at a tile.
void PacketOperator::SendChestAddItem(Windows::TPoint position, unsigned int item_id, unsigned int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_CHEST, packet);
}

// Takes an item out of the chest at a tile.
void PacketOperator::SendChestTakeItem(Windows::TPoint position, int item_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, item_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_CHEST, packet);
}

// Opens the locker at a tile.
void PacketOperator::RequestLockerOpen(Windows::TPoint position)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_OPEN, OP_LOCKER, packet);
}

// Buys more room in the locker.
void PacketOperator::SendBuyLockerUpgrade()
{
    MainForm->net_client->SendPacket(AC_BUY, OP_LOCKER, Client::EncodeNumber(MainForm->net_client, 1, 1));
}

// Puts an amount of an item in the locker at a tile.
void PacketOperator::SendLockerAddItem(Windows::TPoint position, unsigned int item_id, unsigned int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 3);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_LOCKER, packet);
}

// Takes an item out of the locker at a tile.
void PacketOperator::SendLockerTakeItem(Windows::TPoint position, int item_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, position.x, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, position.y, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, item_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_GET, OP_LOCKER, packet);
}

// Agrees to the trade as it stands; SendTradeDisagree takes the agreement back.
void PacketOperator::SendTradeAgree()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_UPDATE, OP_TRADE, packet);
}

void PacketOperator::SendTradeDisagree()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 0, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_UPDATE, OP_TRADE, packet);
}

void PacketOperator::SendTradeClose()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 0, 1);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CLOSE, OP_TRADE, packet);
}

// Asks a player to trade.
void PacketOperator::RequestTrade(int player_id)
{
    // A fixed first byte of 138.
    String packet = Client::EncodeNumber(MainForm->net_client, 138, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_TRADE, packet);
}

// Puts an amount of an item into the trade.
void PacketOperator::SendTradeAddItem(int item_id, int amount)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    packet = packet + Client::EncodeNumber(MainForm->net_client, amount, 4);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_ADD, OP_TRADE, packet);
}

void PacketOperator::SendTradeRemoveItem(int item_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, item_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_DELETE, OP_TRADE, packet);
}

// Asks to join a player's group: kind 0. RequestPartyInvite sends kind 1 to invite the player instead.
void PacketOperator::RequestPartyJoin(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 0, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_GROUP, packet);
}

void PacketOperator::RequestPartyInvite(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_REQUEST, OP_GROUP, packet);
}

// Answers a request to join the group with kind 0; AcceptPartyInvite answers an invite with kind 1.
void PacketOperator::AcceptPartyJoin(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 0, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_GROUP, packet);
}

// Accepts a player's trade request.
void PacketOperator::AcceptTrade(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 0, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_TRADE, packet);
}

void PacketOperator::AcceptPartyInvite(int player_id)
{
    String packet = Client::EncodeNumber(MainForm->net_client, 1, 1);
    packet = packet + Client::EncodeNumber(MainForm->net_client, player_id, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_CONFIRM, OP_GROUP, packet);
}

// Asks the server for a reply, to measure the time it takes.
void PacketOperator::SendPingRequest()
{
    String packet = Client::EncodeNumber(MainForm->net_client, 2, 2);
    if (packet.Length() >= 1)
        MainForm->net_client->SendPacket(AC_SYN1, OP_SYSTEM, packet);
}

char PacketOperator::ByteToChar(unsigned char value)
{
    return value;
}
