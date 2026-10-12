#ifndef PlayersH
#define PlayersH

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

enum ItemSpecial
{
    ItemSpecial_Lore = 4,
    ItemSpecial_Cursed = 5
};
#define MAX_PLAYER_ID 100000

struct Player;
struct Settings;
struct mySQLdb;

#include "Player.h"

struct Players;

// Every connection, in order and by socket handle, with what is done to a player's items, spells and party.
struct Players
{
    vector<Player *> players;
    Player *by_id[MAX_PLAYER_ID];
    Settings *settings;
    mySQLdb *mysql_controls;
    bool dirty;
    int online_count;
    int peak_online;

    Players(Settings *settings, mySQLdb *mysql_controls);
    ~Players();
    void Tick();
    char Add(Scktcomp::TCustomWinSocket *socket);
    void MarkRemoving(Scktcomp::TCustomWinSocket *socket);
    void Remove(Scktcomp::TCustomWinSocket *socket);
    String Character_BuildSaveQuery(Player *player, int flags);
    char IsAccountIdentOnline(int account_ident);
    bool IsAccountNameTaken(String account_name, int player_id);
    char IsPlayerAt(int map_id, int x, int y);
    bool CharName_IsReserved(Player *player, String name);
    char Player_HasKeyItem(Player *player, int key_id);
    int Player_GetSpellLevel(Player *player, int spell_id);
    char Player_HasSpellId(Player *player, int spell_id);
    char Player_HasBankItem(Player *player, int item_id);
    char Player_EquipItem(Player *player, int item_id, int slot);
    char Player_UnequipItem(Player *player, int item_id, int slot);
    void Player_AddItem(Player *player, int item_id, int amount);
    char Player_RemoveItem(Player *player, int item_id, unsigned int amount);
    void Player_RemoveItemNoQuestRules(Player *player, int item_id, unsigned int amount);
    int GetItemAmount(Player *player, int item_id);
    void Player_AddBankItem(Player *player, int item_id, int amount);
    char Player_RemoveBankItem(Player *player, int item_id);
    char Player_AddTradeItem(Player *player, int item_id, int amount);
    char Player_RemoveTradeItem(Player *player, int item_id);
    void Player_AddSpell(Player *player, int spell_id);
    int Player_LevelUpSpell(Player *player, int spell_id);
    char Player_RemoveSpell(Player *player, int spell_id);
    void Player_ClearSpells(Player *player);
    char Player_RemoveCursedItems(Player *player);
    int CountArenaPlayers(int map_id);
    int CountGuildInvites(Player *player);
    int CountGuildlessOnMap(Player *player);
    void GuildSetMemberInfo(Player *player, String guild_name, String guild_tag);
    void Party_AddNewMember(Player *existing_member, Player *new_member);
    void Player_LeaveParty(Player *player);
    void Player_RegenHpTp(Player *player);
    Player *GetById(int player_id);
    Player *FindByName(String name);
    Player *GetByMapTile(int map_id, int x, int y);
    int Player_TryLevelUp(Player *player);
    int GetActiveCount();
    int GetOnlineCount();
    int GetPeakOnline();
    void MarkDirty();
    void UpdatePeakOnline();
    void Player_LevelUp(Player *player);
};

#endif
